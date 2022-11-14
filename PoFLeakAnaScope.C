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

void PoFLeakAnaScope(){
  // Dataset (reference + compare)

  // SPE Analyzable:
  // nolaser-reference-1mohm-dc-4nsResolution-nov8.txt (nothing on SiPM, closed box)

  // SiPM overcurrent:
  // chineselaser-reference4power200mW-dc1megaohm-4nsResolution-nov8.txt
  // chineselaser-200mW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov8.txt

  // SiPM overcurrent:
  // chineselaser-reference4power400mW-dc1megaohm-4nsResolution-nov8.txt
  // chineselaser-400mW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov8.txt

  // SPE Analyzable:
  // chineselaser-reference4power0p1-1uW-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-0p1uW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-0p3uW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-1uW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov9.txt

  // SPE Analyzable:
  // chineselaser-reference-62p5micro-blackfiber-blackboxdiagonal-to-opc-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-200mW-62p5micro-blackfiber-blackboxdiagonal-to-opc-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-400mW-62p5micro-blackfiber-blackboxdiagonal-to-opc-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-600mW-62p5micro-blackfiber-blackboxdiagonal-to-opc-dc1megaohm-4nsResolution-nov9.txt
  // chineselaser-800mW-62p5micro-blackfiber-blackboxdiagonal-to-opc-dc1megaohm-4nsResolution-nov9.txt

  // FC-FC connector, SiPM overcurrent
  // chineselaser-reference-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-after40m-power5p3mW-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-after40m-power10mW-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-after40m-power48mW-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-after40m-power154mW-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-after40m-power350mW-FC-FCconnector-blackboxdiagonal-to-meter-dc1megaohm-4nsResolution-nov10.txt

  // 40-m spool 62.5um black fiber, SPE analyzable, a lot of current at 350mW & 200mW
  // chineselaser-reference-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-5mW-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-10mW-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-48mW-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-156mW-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt
  // chineselaser-350mW-40mspoolfiber-62p5micronblack-opensipm-tometer-dc1megaohm-4nsResolution-nov10.txt

  // Scope Keysight
  // Sample rate 250M/s (4ns resolution per point), each dataset points 200 million
  // save as 1 column in txt file, unit: Volt

  TString dirname = "/pnfs/dune/persistent/users/weishi/PoFLeakageTest";
  Bool_t filternoise = true; // apply noise filter if true (most likely you need)
  vector<TString> adcdataset;
  TString legendname[7];
  int color[7] = {1, 2, 3, 41, 6, 4, 9}; // preferred color
  int markerstyle[7] = {21, 21, 21, 21, 3, 21, 21};
  double Count[7][20] = {0}; // put 20 as default length
  double Gain[7][20] = {0};
  double ChargeIntegral[7] = {0};
  int startpoint = 2500; // example waveform starting point and end point
  int endpoint = 5000;

  int nadcthrs = 8; // number of amplitude thresholds
  double delta_amp[20] = {0}; // put 20 as default length
  // delta ADC above baseline as threshold
  if ( nadcthrs > 20 ) {
    std::cout << "nadcthrs > 20, please increase array length of delta_amp, Count, and Gain" << endl;
    exit(1);
  }
  delta_amp[0] = 0.02; // normally depends on channel, set at roughly SPE amplitude unit is Volt
  for(int ithres=0; ithres<nadcthrs; ++ithres) delta_amp[ithres] = delta_amp[0] + 0.025*ithres;

  // Samples
  if ( true ) {
    adcdataset.push_back("chineselaser-reference4power0p1-1uW-dc1megaohm-4nsResolution-nov9.txt"); // ref
    adcdataset.push_back("chineselaser-0p1uW-GaAsOPCreceiver-dc1megaohm-4nsResolution-nov9.txt");
    legendname[0] = "No Laser";
    legendname[1] = "0.1uW GaAs OPC no potting";
  }

  int ndat = adcdataset.size();

  Double_t samplingRate = 250.e6; // 250 MSamples/s for Keysight scope
  Double_t dtime = (1/samplingRate)*1e9; // steps in nanoseconds

  // waveform plot
  Double_t baseline;
  Int_t baseline_bin;
  TH1D *h1 = new TH1D("h1", "h1", endpoint-startpoint, startpoint, endpoint);
  TH1D *h2 = new TH1D("h2", "h2", endpoint-startpoint, startpoint, endpoint);
  TH1F** amplitude_hist = new TH1F*[ndat];
  int memorydepth = 200000000;

  for ( int idat = 0; idat < ndat; idat++ ) {
    std::cout << "Looking at dataset " << idat << ": " << adcdataset[idat] << std::endl;

    int line=0;
    double amplitude;

    vector<Double_t> raw; // waveform as vector
    vector<Double_t> denoised; // denoised waveform as vector

    amplitude_hist[idat] = new TH1F("", "", 1000, 0, 1); // 1mV

    ifstream file( TString::Format("%s/%s", dirname.Data(), adcdataset[idat].Data()) );

    while (!file.eof()){
      file>>amplitude;

      if (line % 10000000 == 0) std::cout << "line "<< line << ": " << amplitude << endl;

      raw.push_back(amplitude);
      denoised.push_back(amplitude);

      line++;

    } // end while

    std::cout << "last line "<< line << endl;

    //
    // Apply noise filter
    //

    if (filternoise) TV1D_denoise(raw, denoised, memorydepth, 0.02);

    //
    // Evaluate waveform baseline
    //

    baseline = 0.;
    baseline_bin = -1;
    for(int ipoint = 0; ipoint < memorydepth; ipoint++) amplitude_hist[idat]->Fill(denoised[ipoint]);
    baseline_bin = amplitude_hist[idat]->GetMaximumBin();
    baseline = amplitude_hist[idat]->GetXaxis()->GetBinCenter(baseline_bin);

    cout <<"Dataset "<< idat <<" baseline ADC: "<< baseline <<endl;

    double charge = 0; // initilize to zero for each dataset

    // Loop over points in each waveform
    for(int jpoint=0; jpoint<memorydepth; jpoint++){

      // Integrate all memory points
      charge += (denoised[jpoint] - baseline); // relative to baseline

      //
      // Count up-crossings in each waveform
      //

      for(int ithres=0; ithres<nadcthrs; ++ithres){ // all thresholds
        if( ( denoised[jpoint] - (baseline+delta_amp[ithres]) )<0 && ( denoised[jpoint+1] - (baseline+delta_amp[ithres]) )>0 ){
          Count[idat][ithres] += 1;
        } // end if
      } // end loop over thrs
    } // end loop over waveform points in a dataset

    cout <<"Dataset "<< idat <<" total charge integral [V*ns]: "<< charge*dtime <<endl;
    cout <<"Dataset "<< idat <<" every 10 us charge integral [V*ns]: "<< charge*(10*1000)/memorydepth <<endl;

    //
    // Plot example waveform
    //
    if(idat == 1) {
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
      c3->SaveAs( TString::Format("example_10us_waveform_from_dataset%d.png", idat) );
      delete c3;
    } // end plot example waveform

    cout<<"========== Result =========== "<<endl;
    for(int ithres=0; ithres<nadcthrs; ++ithres){
      cout<<"Counts at delta amplitude " << delta_amp[ithres]<<": "<<Count[idat][ithres]<<endl;
    }

  } // end loop over dataset

  // Plot amplitude distribution
  TCanvas *camplitude = new TCanvas();
  camplitude->SetLogy(); camplitude->SetGridx(); camplitude->SetGridy();
  gStyle->SetOptStat(0);
  TLegend *legdataset = new TLegend(0.55, 0.65, 0.9, 0.9);
  for ( int idat = 0; idat < ndat; idat++ ) {
    amplitude_hist[idat]->SetLineColor(color[idat]);
    if ( idat == 0 ) {
      amplitude_hist[idat]->GetXaxis()->SetTitle("Baseline ADC");
      amplitude_hist[idat]->GetYaxis()->SetTitle("Events");
      amplitude_hist[idat]->SetMaximum(memorydepth);
      amplitude_hist[idat]->Draw("HIST");
    } else {
      amplitude_hist[idat]->Draw("HIST SAME");
    } // end plot
    legdataset->AddEntry(amplitude_hist[idat], TString::Format("%s", legendname[idat].Data()));
  }

  legdataset->Draw();
  gPad->RedrawAxis();
  camplitude->SaveAs("amplitude_distribution_all_dataset.png");

  //
  // TO BE UPDATED
  //

  //
  // Calculate average (extra) upcrossings per 10us
  //

  // per 10 [us]

  for ( int idat = 0; idat < ndat; idat++ ) {
    for ( int ithres = 0; ithres < nadcthrs; ++ithres ) {
      if ( idat == 0 ) Gain[idat][ithres] = Count[idat][ithres]*10*1000/dtime/memorydepth; // reference avg count
      else Gain[idat][ithres] = (Count[idat][ithres] - Count[0][ithres])*10*1000/dtime/memorydepth; // extra upcrossings due to noise light
    }
  }

  //
  // Draw count of upcrossings
  //

  TCanvas *c = new TCanvas();
  c->SetLogy(); c->SetGridx(); c->SetGridy();

  TGraph** dis = new TGraph*[ndat];
  TMultiGraph *mg = new TMultiGraph();
  TLegend *leg = new TLegend(0.55, 0.65, 0.9, 0.9);

  for ( int idat = 0; idat < ndat; idat++ ) {
    dis[idat] = new TGraph(nadcthrs, delta_amp, Count[idat]);
    dis[idat]->SetMarkerSize(0.8);
    dis[idat]->SetMarkerStyle(markerstyle[idat]);
    dis[idat]->SetMarkerColor(color[idat]);
    dis[idat]->SetLineColor(color[idat]);
    mg->Add(dis[idat]);
    leg->AddEntry(dis[idat], TString::Format("%s", legendname[idat].Data()));
  }

  mg->GetXaxis()->SetTitle("#Delta amplitude (Volt)");
  mg->GetYaxis()->SetTitle("Counts");
  mg->SetMaximum(Count[ndat-1][0]*2.); // highest light noise and lowest threshold
  mg->SetMinimum(0.1); // reference and highest threshold
  mg->Draw("APL");

  leg->Draw();
  c->SaveAs("up-crossing_count.png");

  //
  // Draw extra upcrossings
  //
  TCanvas *c2 = new TCanvas();
  c2->SetGridx(); c2->SetGridy();
  TMultiGraph *mg2 = new TMultiGraph();
  TLegend *leg2 = new TLegend(0.55, 0.65, 0.9, 0.9);

  TGraph** diff = new TGraph*[ndat-1];
  // require more than 1 dat
  for ( int idat = 0; idat < ndat-1; idat++ ) { // start from first dat
    diff[idat] = new TGraph(nadcthrs, delta_amp, Gain[idat+1]);
    diff[idat]->SetMarkerSize(0.8);
    diff[idat]->SetMarkerStyle(markerstyle[idat+1]);
    diff[idat]->SetMarkerColor(color[idat+1]);
    diff[idat]->SetLineColor(color[idat+1]);
    mg2->Add(diff[idat]);
    leg2->AddEntry(diff[idat], TString::Format("%s", legendname[idat+1].Data()));
  }

  mg2->GetXaxis()->SetTitle("#Delta amplitude (Volt)");
  mg2->GetYaxis()->SetTitle("Extra counts per 10#mus");
  mg2->Draw("APL");

  leg2->Draw();
  c2->SaveAs("extra_up-crossing_per10us_count.png");

  // Plot the reference upcrossing counts per 10us
  TCanvas *c4 = new TCanvas();
  c4->SetGridx(); c4->SetGridy();
  TGraph *reference = new TGraph(nadcthrs, delta_amp, Gain[0]);
  reference->SetMarkerSize(0.8);
  reference->SetMarkerStyle(21);
  reference->SetMarkerColor(1);
  reference->SetLineColor(1);
  reference->SetTitle("No Laser;#Delta amplitude (Volt);Counts per 10#mus");
  reference->Draw("APL");
  c4->SaveAs("up-crossing_per10us_reference.png");

} // end main
