#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
//
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
//

// Total variation noise filter: https://hal.archives-ouvertes.fr/hal-00675043v2/document
void TV1D_denoise(vector<Double_t>& input, vector<Double_t>& output, const int width, const Double_t lambda){
  if (width>0) {                /*to avoid invalid memory access to input[0]*/
    int k=0, k0=0;            /*k: current sample location, k0: beginning of current segment*/
    Double_t umin=lambda, umax=-lambda;    /*u is the dual variable*/
    Double_t vmin=input[0]-lambda, vmax=input[0]+lambda;    /*bounds for the segment's value*/
    int kplus=0, kminus=0;     /*last positions where umax=-lambda, umin=lambda, respectively*/
    const Double_t twolambda=2.0*lambda;    /*auxiliary variable*/
    const Double_t minlambda=-lambda;        /*auxiliary variable*/
    for (;;) {                /*simple loop, the exit test is inside*/
      while (k==width-1) {    /*we use the right boundary condition*/
        if (umin<0.0) {            /*vmin is too high -> negative jump necessary*/
          do output[k0++]=vmin; while (k0<=kminus);
          umax=(vmin=input[kminus=k=k0])+(umin=lambda)-vmax;
        } else if (umax>0.0) {    /*vmax is too low -> positive jump necessary*/
          do output[k0++]=vmax; while (k0<=kplus);
          umin=(vmax=input[kplus=k=k0])+(umax=minlambda)-vmin;
        } else {
          vmin+=umin/(k-k0+1);
          do output[k0++]=vmin; while(k0<=k);
          return;
        }
      }
      if ((umin+=input[k+1]-vmin)<minlambda) {        /*negative jump necessary*/
        do output[k0++]=vmin; while (k0<=kminus);
        vmax=(vmin=input[kplus=kminus=k=k0])+twolambda;
        umin=lambda; umax=minlambda;
      } else if ((umax+=input[k+1]-vmax)>lambda) {    /*positive jump necessary*/
        do output[k0++]=vmax; while (k0<=kplus);
        vmin=(vmax=input[kplus=kminus=k=k0])-twolambda;
        umin=lambda; umax=minlambda;
      } else {     /*no jump necessary, we continue*/
        k++;
        if (umin>=lambda) {        /*update of vmin*/
          vmin+=(umin-lambda)/((kminus=k)-k0+1);
          umin=lambda;
        }
        if (umax<=minlambda) {    /*update of vmax*/
          vmax+=(umax+lambda)/((kplus=k)-k0+1);
          umax=minlambda;
        }
      }
    }
  }
}

void SNRAnaScope(){
  // =====================
  // User edit area start
  // =====================
  // Dataset: 2 columns in txt file, triger + signal, unit: Volt
  //TString dirname = "/pnfs/dune/persistent/users/weishi/SPEAna/OPCPT30BiasHamamatsu20ArrayLN2SimpX3LED";
  TString dirname = "/pnfs/dune/persistent/users/weishi/SPEAna/BroadcomSiPMData";
  Bool_t filternoise = true; // apply noise filter if true (most likely you need)
  //TString adcdataset = "led-pulse-4p6-feb27-2023-hamamatsu-36Vbd-LN2.txt"; // with low pass filter < 60MHz
  TString adcdataset = "LN2_BroadcomSiPM_PoFfullchainPT30OPC38p2Vbias-HighfreqLED4p5-Simp3X-ch2-22nF_C1-6-RemoveR7Row.txt";

  // Dataset
  // SPE analysis for Simpx3+hamamatsu (linac setup for light leak test):
  // /pnfs/dune/persistent/users/weishi/SPEAna/Hamamatsu20ArrayVbd36ColdSimpX3LED/led-pulse-4p6-feb27-2023-hamamatsu-36Vbd-LN2.txt

  // Scope setting
  Double_t samplingRate = 250.e6; // 250 MSamples/s for Keysight scope is 4ns per point
  Double_t dtime = (1/samplingRate)*1e9; // steps in nanoseconds
  int memorydepth = 199999999; // number of saved data points in dataset
  int margin = 10; // number of saved data points in dataset

  // Led trigger and signal setting
  Double_t LED_halfheight = 2.5; // volts
  int trggerrefoffset = 32; // find this number of data points since trigger rise edge before integrating signal (>0 signal is after, <0 signal is before rise edge)
  int signalwidth = 500; // signal integrate window size (number of 4ns points)
  double baselinemin = 0.3; // unit V, this range should be set based on raw wfm, a wrong baseline will shift your SPE spectrum
  double baselinemax = 0.36;
  Double_t filter_lamda = 0.03; // unit Volt, this is roughly the SPE amplitude

  // Plot example waveform
  int startpoint = 0;
  int endpoint = 10000;
  double integralQmin = -30;
  double integralQmax = 200;
  double AmpMax = 0.05; // unit: V
  double AmpMin = -0.05;

  // ===================
  // User edit area end
  // ===================

  // waveform plot
  Double_t baseline;
  Int_t baseline_bin;
  Double_t baseline_local;
  Int_t baseline_bin_local;
  TH1D *h1 = new TH1D("h1", "h1", endpoint-startpoint, startpoint, endpoint);
  TH1D *h2 = new TH1D("h2", "h2", endpoint-startpoint, startpoint, endpoint);
  TH1F *amplitude_hist = new TH1F("amplitude_hist", "amplitude_hist", 100, baselinemin, baselinemax);
  TH1F *IntegralHist = new TH1F("IntegralHist", "IntegralHist", 2000, integralQmin, integralQmax);
  TH1F *MaxAmpHist = new TH1F("MaxAmpHist", "MaxAmpHist", 100, AmpMin, AmpMax);

  int line=0;
  double trigger; // LED trigger
  double amplitude;

  vector<Double_t> trig; // trig as vector
  vector<Double_t> raw; // waveform as vector
  vector<Double_t> denoised; // denoised waveform as vector

  ifstream file( TString::Format("%s/%s", dirname.Data(), adcdataset.Data()) );

  while (!file.eof()){
    //
    // Here need to specify which line is which
    //
    file>>amplitude>>trigger;
    //file>>trigger>>amplitude;
    if (line % 100000000 == 0) std::cout << "line "<< line << ": " << amplitude << "  " << trigger << endl;
    trig.push_back(trigger);
    raw.push_back(amplitude);
    denoised.push_back(amplitude);
    line++;
  } // end while

  std::cout << "Last line "<< line << endl;
  std::cout << "Input memory depth: "<< memorydepth << endl;

  //
  // Apply noise filter
  //

  if (filternoise) TV1D_denoise(raw, denoised, memorydepth, filter_lamda);

  //
  // Evaluate waveform baseline
  //

  baseline = 0.;
  baseline_bin = -1;
  for(int ipoint = 0; ipoint < memorydepth; ipoint++) amplitude_hist->Fill(denoised[ipoint]);
  baseline_bin = amplitude_hist->GetMaximumBin();
  baseline = amplitude_hist->GetXaxis()->GetBinCenter(baseline_bin);

  TCanvas *cbaselineamp = new TCanvas();
  amplitude_hist->Draw("HIST");
  cbaselineamp->SaveAs("BaselineAmp.png");

  cout <<"Dataset baseline [V]: "<< baseline <<endl;

  bool currenttrig = false; // is this the rising edge of the trig
  double charge = 0.; // initilize to zero for each dataset
  double amp = 0.; // initilize to zero for each dataset
  double maxcharge = 0.; // initilize to zero for each dataset
  int countwaveforms = 0; // total number of triggered signals


  // Loop over points
  for(int jpoint=margin; jpoint<memorydepth-signalwidth; jpoint++){
    // Trigger rising edge
    if ( trig[jpoint] < LED_halfheight && trig[jpoint+1] > LED_halfheight ) currenttrig = true;

    // Integrate signal starting from rising edge + trggerrefoffset, integrate width is signalwidth
    //   if trggerrefoffset is negative, signal comes before rising edge
    //   if trggerrefoffset is positive, signal comes after rising edge

    if ( currenttrig == true ) {

      // integrate
      for(int lpoint = jpoint + trggerrefoffset; lpoint < jpoint + trggerrefoffset + signalwidth; lpoint++) {
        amp = (denoised[lpoint] - baseline); // relative to baseline
        charge += amp;
        if (amp > maxcharge) maxcharge = amp;
      }

      // finish integrate, fill it
      IntegralHist->Fill(charge*dtime);
      MaxAmpHist->Fill(maxcharge);
      countwaveforms++;

      // initilize for next triggered signal
      currenttrig = false;
      charge = 0.;
      maxcharge = 0.;
      amp = 0.;

    } else {
      continue;
    }

  } // end loop over all points in a dataset

  cout <<"Total triggers: "<< countwaveforms <<endl;

  //
  // Plot example waveform
  //
  for(int ipoint=startpoint; ipoint<endpoint; ipoint++){
    h1->Fill(ipoint, raw[ipoint]);
    h2->Fill(ipoint, denoised[ipoint]);
  }
  TCanvas *c3 = new TCanvas();
  h1->SetLineColor(2);
  h1->Draw("HIST");
  h1->GetXaxis()->SetTitle("Time tick (4ns)");
  h1->GetYaxis()->SetTitle("Amplitude (Volt)");
  h2->SetLineColor(4);
  h2->SetLineStyle(7);
  h2->Draw("HIST SAME");
  c3->SaveAs("Example_waveform_raw_and_denoised.png");

  // Plot amplitude distribution
  TCanvas *camplitude = new TCanvas();
  //camplitude->SetLogy();
  camplitude->SetGridx(); camplitude->SetGridy();

  // plot
  IntegralHist->GetXaxis()->SetTitle("V*ns");
  IntegralHist->GetYaxis()->SetTitle("Events");
  IntegralHist->Draw("HIST");

  camplitude->SaveAs("Integral_Q_distribution.root");

  MaxAmpHist->GetXaxis()->SetTitle("V");
  MaxAmpHist->GetYaxis()->SetTitle("Events");
  MaxAmpHist->Draw("HIST");

  camplitude->SaveAs("MaxAmp_distribution.png");
} // end main
