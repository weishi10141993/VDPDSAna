#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

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

void PoFLeakAna(){
  //=============
  // User config
  //=============
  Bool_t got_mem_depth = true; // so we can initilize stuff
  Bool_t filternoise = true; // apply noise filter if true (most likely you need)
  int example_waveform = 3; // example waveform number you want to plot from 1st dataset: can't exceed maximum waveforms
  // Samples
  // Sep 17
  // No laser:      /afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/argon_only/wave5.dat
  // v2 switch A:   /afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_argon_read/wave5.dat
  // v2 switch B:   /afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchB_argon_read/wave5.dat
  // v2 switch A&B: /afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_and_switchB_argon_read/wave5.dat
  // v2 & v3:       /afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check/20220917_v2_and_v3_all_lasers/wave5.dat

  // Sep 14
  // No laser:
  // v3 laser 1:
  // v3 laser 2:
  // v3 laser 3:
  // v3 laser 1,2,3:
  TString dirname = "/afs/cern.ch/work/s/shiw/public/ColdBoxVD";
  vector<TString> adcdataset;
	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/argon_only/wave5.dat");
	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_argon_read/wave5.dat");
  adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchB_argon_read/wave5.dat");
  adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_and_switchB_argon_read/wave5.dat");
  adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_and_v3_all_lasers/wave5.dat");

  //==========================
  // Digitizer config
  //     usually no change
  //==========================
  int headbin; // to store headers
  uint32_t valbin = 0; // to read data
  int headers = 6; // total number of headers
  int nbytes_headers = 4; // 4 bytes (32 bits) for each header
  int nbytes_data = 2; // 2 bytes (16 bits) per sample
  int memorydepth = 0; // size of waveforms
  double percentage = 0.7; // fraction of the waveform of interest from beginning, usually trigger is put fairly late and is excluded

  Int_t nbits = 14; // ADC is a 14 bits, 2 Vpp
  Double_t samplingRate = 250.e6; // 250 MSamples/s for DT5725
  Int_t nADCs = pow(2, nbits); // number of digital channels
  Double_t inVolts = 2./nADCs; // Multiply by this number to have the amplitude in volts;
  Double_t dtime = (1/samplingRate)*1e9; // steps in nanoseconds

  //===================
  // General Variables
  //===================
  vector<Double_t> raw; // waveform as vector
  vector<Double_t> denoised; // denoised waveform as vector
  Double_t baseline_ADC;
  Int_t baseline_ADC_bin;

  double delta_ADC[8] = {0};
  double Count[5][8] = {0};
  double Gain[4][8] = {0};

  // delta ADC above baseline as threshold
  delta_ADC[0] = 50;
  for(int ithres=0; ithres<8; ++ithres) delta_ADC[ithres] = delta_ADC[0] + 25*ithres;

  // Single waveform
  TH1D *h1;
  TH1D *h2; // denoised

  ifstream fin;

  for ( int idat = 0; idat < adcdataset.size(); idat++ ) {
    std::cout << "Looking at dataset " << idat << ": " << adcdataset[idat] << std::endl;

    fin.open(TString::Format("%s/%s", dirname.Data(), adcdataset[idat].Data()), ios::in | ios::binary);

    if( !( fin.good() && fin.is_open() ) ) {
      std::cout << "File did not open!!" << endl;
      exit(1);
    };

    int waveNum = 0; // num of waveforms, set to 0 for each new dataset

    // Loop over waveforms in the dataset
    while(!fin.fail()){
      // stream all headers
      for(Int_t ln=0; ln<headers; ln++){
        fin.read((char *) &headbin, nbytes_headers);
        // header0 will be EventSize, so: you can do
        if( ln == 0 ){
          memorydepth = headbin;
          // the result is in bytes, so 2*NSamples+4*headers
          memorydepth = (memorydepth - nbytes_headers*headers)/nbytes_data;
        }
      } // end stream all headers

      if(got_mem_depth && idat == 0){ // initialization based on memorydepth
        got_mem_depth=false;
        printf("Waveform points: %d \n", memorydepth);
        raw.resize(memorydepth);
        denoised.resize(memorydepth); // this is necessary for some reason
        h1 = new TH1D("h1", "h1", memorydepth, 0, memorydepth);
        h2 = new TH1D("h2", "h2", memorydepth, 0, memorydepth);
      }

      // Store each waveform
      for(int ipoint = 0; ipoint < memorydepth; ipoint++) {
          fin.read((char *) &valbin, nbytes_data); // 2 bytes (16 bits) per sample
          if(fin.bad() || fin.fail()) break;
          raw[ipoint] = valbin;
          denoised[ipoint] = raw[ipoint];
      }

      //
      // Apply noise filter
      //

      if (filternoise) TV1D_denoise(raw, denoised, memorydepth, 50);

      //
      // Evaluate single waveform baseline
      //

      baseline_ADC = 0.;
      baseline_ADC_bin = -1;
      TH1F *ADC_hist = new TH1F("ADC_hist", "ADC_hist", nADCs/4, 0, nADCs); // this bin can't be too fine, 4 adc binning
      for(int ipoint = 0; ipoint < memorydepth; ipoint++) ADC_hist->Fill(denoised[ipoint]);
      baseline_ADC_bin = ADC_hist->GetMaximumBin();
      baseline_ADC = ADC_hist->GetXaxis()->GetBinCenter(baseline_ADC_bin);
      if(waveNum == example_waveform && idat == 0) {
        cout <<"Dataset "<< idat <<" example waveform # "<< example_waveform <<" baseline ADC: "<< baseline_ADC <<endl;
        TCanvas *tmp = new TCanvas();
        ADC_hist->Draw();
        tmp->SaveAs( TString::Format("example_ADC_hist_from_dataset%d.png", idat) );
        delete tmp;
      }
      delete ADC_hist;

      //
      // Count up-crossings in each waveform
      //

      for(int ithres=0; ithres<8; ++ithres){ // all thresholds
        for(int jpoint=0; jpoint<1750; jpoint++){ // range of points we are interested in
          if( ( denoised[jpoint] - (baseline_ADC+delta_ADC[ithres]) )<0 && ( denoised[jpoint+1] - (baseline_ADC+delta_ADC[ithres]) )>0 ){
            Count[idat][ithres] += 1;
          }
        }
      }

      // Integrate peak region
      // TODO

      waveNum += 1;
      if (waveNum % 10000 == 0) printf("Wavenum: %d\n", waveNum);

      //
      // Plot example single waveform
      //
      if(waveNum == example_waveform && idat == 0) {
        for(int ipoint=0; ipoint<memorydepth; ipoint++){
          h1->Fill(ipoint, raw[ipoint]);
          h2->Fill(ipoint, denoised[ipoint]);
        }
        TCanvas *c3 = new TCanvas();
        h1->SetLineColor(2);
        h1->Draw("HIST");
        h1->GetXaxis()->SetTitle("Time tick (4ns)");
        h1->GetYaxis()->SetTitle("Amplitude (ADC Channels)");
        h2->SetLineColor(4);
        h2->SetLineStyle(7);
        h2->Draw("HIST SAME");
        c3->SaveAs( TString::Format("example_single_waveform_from_dataset%d.png", idat) );
        delete c3;
      } // end plot single waveform

    } // End loop over waveforms

    cout<<"========== Result =========== "<<endl;
    for(int ithres=0; ithres<8; ++ithres){
      cout<<"Counts at delta ADC " << delta_ADC[ithres]<<": "<<Count[idat][ithres]<<endl;
    }

    fin.close();

  } // end loop over dataset

  //
  // Calculate extra upcrossings
  //
  for(int i=0; i<8; ++i){
      Gain[0][i] = (Count[1][i] - Count[0][i])/20000*5000/3500;
      Gain[1][i] = (Count[2][i] - Count[0][i])/20000*5000/3500;
      Gain[2][i] = (Count[3][i] - Count[0][i])/20000*5000/3500;
      Gain[3][i] = (Count[4][i] - Count[0][i])/20000*5000/3500;
  }

  //
  // Draw count of upcrossings
  //
  TGraph *dis0 = new TGraph(8, delta_ADC, Count[0]);
  TGraph *dis1 = new TGraph(8, delta_ADC, Count[1]);
  TGraph *dis2 = new TGraph(8, delta_ADC, Count[2]);
  TGraph *dis3 = new TGraph(8, delta_ADC, Count[3]);
  TGraph *dis123 = new TGraph(8, delta_ADC, Count[4]);

  TCanvas *c = new TCanvas();
  c->SetLogy(); c->SetGridx(); c->SetGridy();
  TMultiGraph *mg = new TMultiGraph();

  dis0->SetMarkerSize(0.8);
  dis0->SetMarkerStyle(21);
  dis0->SetMarkerColor(1);
  dis0->SetLineColor(1);

  dis1->SetMarkerSize(0.8);
  dis1->SetMarkerStyle(21);
  dis1->SetMarkerColor(2);
  dis1->SetLineColor(2);

  dis2->SetMarkerSize(0.8);
  dis2->SetMarkerStyle(21);
  dis2->SetMarkerColor(3);
  dis2->SetLineColor(3);

  dis3->SetMarkerSize(0.8);
  dis3->SetMarkerStyle(21);
  dis3->SetMarkerColor(41);
  dis3->SetLineColor(41);

  dis123->SetMarkerSize(0.8);
  dis123->SetMarkerStyle(3);
  dis123->SetMarkerColor(6);
  dis123->SetLineColor(6);

  mg->Add(dis0);
  mg->Add(dis1);
  mg->Add(dis2);
  mg->Add(dis3);
  mg->Add(dis123);
  mg->GetXaxis()->SetTitle("#Delta (ADC)");
  mg->GetYaxis()->SetTitle("Counts");
  mg->SetMaximum(Count[4][0]*2.); // highest laser power and lowest threshold
  mg->SetMinimum(Count[0][7]/2.); // no laser and highest threshold
  mg->Draw("AP");

  TLegend *leg = new TLegend(0.55, 0.65, 0.9, 0.9);
  leg->AddEntry(dis0, "No Laser");
  leg->AddEntry(dis1, "v2 SwitchA [1458mW]");
  leg->AddEntry(dis2, "v2 SwitchB [581mW]");
  leg->AddEntry(dis3, "v2 SwitchA & SwitchB [2039mW]");
  leg->AddEntry(dis123, "All Lasers of v2 & v3 on [4221mW]");
  leg->Draw();

  c->SaveAs("up-crossing_count.png");


  //
  // Draw extra upcrossings
  //
  TCanvas *c2 = new TCanvas();
  c2->SetGridx(); c2->SetGridy();
  TMultiGraph *mg2 = new TMultiGraph();

  TGraph *diff1 = new TGraph(8, delta_ADC, Gain[0]);
  TGraph *diff2 = new TGraph(8, delta_ADC, Gain[1]);
  TGraph *diff3 = new TGraph(8, delta_ADC, Gain[2]);
  TGraph *diff123 = new TGraph(8, delta_ADC, Gain[3]);

  diff1->SetMarkerSize(0.8);
  diff1->SetMarkerStyle(21);
  diff1->SetMarkerColor(2);
  diff1->SetLineColor(2);

  diff2->SetMarkerSize(0.8);
  diff2->SetMarkerStyle(21);
  diff2->SetMarkerColor(3);
  diff2->SetLineColor(3);

  diff3->SetMarkerSize(0.8);
  diff3->SetMarkerStyle(21);
  diff3->SetMarkerColor(41);
  diff3->SetLineColor(41);

  diff123->SetMarkerSize(0.8);
  diff123->SetMarkerStyle(3);
  diff123->SetMarkerColor(6);
  diff123->SetLineColor(6);

  mg2->Add(diff1);
  mg2->Add(diff2);
  mg2->Add(diff3);
  mg2->Add(diff123);

  TLegend *leg2 = new TLegend(0.55, 0.65, 0.9, 0.9);
  leg2->AddEntry(diff1, "v2 SwitchA [1458mW]");
  leg2->AddEntry(diff2, "v2 SwitchB [581mW]");
  leg2->AddEntry(diff3, "v2 SwitchA & SwitchB [2039mW]");
  leg2->AddEntry(diff123, "All Lasers of v2 & v3 on [4221mW]");

  mg2->GetXaxis()->SetTitle("#Delta (ADC)");
  mg2->GetYaxis()->SetTitle("Counts per 10#mus");

  mg2->Draw("AP");
  leg2->Draw();
  c2->SaveAs("extra_up-crossing_per10us_count.png");

}
