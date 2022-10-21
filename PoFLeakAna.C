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
  // Choose one of these samples: "B++_v3_high", "B++_v3_low", "B++_v2"
  TString runname = "B++_v2"; // runname_detector_[power]
  Bool_t filternoise = true; // apply noise filter if true (most likely you need)
  int example_waveform = 3; // example waveform number you want to plot from 1st dataset: can't exceed maximum waveforms
  int maxwavenum = 20000; // max number of waveforms want to look at
  double pre_trig_frac; // fraction of waveform we are interested: starting from the beginning of the waveform (run/dataset dependent)

  int nadcthrs = 8; // number of ADC thresholds
  double delta_ADC[20] = {0}; // put 20 as default length
  // delta ADC above baseline as threshold
  if ( nadcthrs > 20 ) {
    std::cout << "nadcthrs > 20, please increase array length of delta_ADC, Count, and Gain" << endl;
    exit(1);
  }
  delta_ADC[0] = 50; // normally depends on channel, set at roughly 1PE amplitude
  for(int ithres=0; ithres<nadcthrs; ++ithres) delta_ADC[ithres] = delta_ADC[0] + 25*ithres;

  vector<TString> adcdataset;
  TString dirname = "/afs/cern.ch/work/s/shiw/public/ColdBoxVD";
  TString channame = "wave5.dat";

  Bool_t got_mem_depth = true; // so we can initilize stuff, usually true

  // up to 7 set of data, can be more, but plot will get messy
  int color[7] = {1, 2, 3, 41, 6, 4, 9}; // preferred color
  int markerstyle[7] = {21, 21, 21, 21, 3, 21, 21};
  TString legendname[7];
  double Count[7][20] = {0}; // put 20 as default length
  double Gain[7][20] = {0};

  // Samples

  // =========
  // Run CRP3
  // =========
  // Same v3, v2 XA detectors on cathode (with copper box shield the DCEMs)
  // Wall junction box covered with metal shield
  // 37V miniarapuca biased by DCEM+Parma DCDC (instead of directly powered by PS)
  // 47V miniarapuca same as Run B++, same Argon2x2 channels

  // =========
  // Run B++
  // =========
  // v3, v2 XA on cathode
  // wave5 is 37V mini-arapuca w/ Argon2x2 (ch1)
  // wave6 is 47V mini-arapuca w/ Argon2x2 (ch2)
  // each dataset should have/stop at 20k waveforms each with 2500 sampling points

  if ( runname == "B++_v3_high" ) {
    // Sep 15 v3 cathode high power 2W
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220915_v3_laser1_705_laser2_737_laser3_740/xarapuca_V3_cosmic_trigger50_no_laser");            // No laser (ref)
  	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220915_v3_laser1_705_laser2_737_laser3_740/xarapuca_V3_cosmic_trigger50_laser_1");             // v3 laser 1
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220915_v3_laser1_705_laser2_737_laser3_740/xarapuca_V3_cosmic_trigger50_laser_2");             // v3 laser 2
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220915_v3_laser1_705_laser2_737_laser3_740/xarapuca_V3_cosmic_trigger50_laser_3");             // v3 laser 3
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220915_v3_laser1_705_laser2_737_laser3_740/xarapuca_V3_cosmic_trigger50_laser_1_and_2_and_3"); // v3 laser 1,2,3
    legendname[0] = "No Laser";
    legendname[1] = "v3 Laser 1 [705mW]";
    legendname[2] = "v3 Laser 2 [737mW]";
    legendname[3] = "v3 Laser 3 [740mW]";
    legendname[4] = "v3 Laser 1+2+3 [2182mW]";
    pre_trig_frac = 0.7;
  } else if ( runname == "B++_v3_low" ) {
    // Sep 17 v3 cathode low power 1W
  	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/argon_only");                          // No laser (ref)
  	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/cathode_v3_laser_1_and_argon_read");   // v3 laser 1
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/cathode_v3_laser_2_and_argon_read");   // v3 laser 2
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/cathode_v3_laser_3_and_argon_read");   // v3 laser 3
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/cathode_v3_laser_123_and_argon_read"); // v3 laser 1,2,3
    legendname[0] = "No Laser";
    legendname[1] = "v3 Laser 1 [388mW]";
    legendname[2] = "v3 Laser 2 [410mW]";
    legendname[3] = "v3 Laser 3 [250mW]";
    legendname[4] = "v3 Laser 1+2+3 [1048mW]";
    pre_trig_frac = 0.7;
  } else if ( runname == "B++_v2" ) {
    // Sep 17 v2 cathode 2W
  	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v3_laser1_388_laser2_410_laser3_250/argon_only");                          // No laser (ref)
  	adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_argon_read");               // v2 switch A
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchB_argon_read");               // v2 switch B
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_swtichA_1458_switchB_581/cathode_v2_switchA_and_switchB_argon_read");   // v2 switch A&B
    adcdataset.push_back("Coldbox_Sep2022_leakage_check/20220917_v2_and_v3_all_lasers");                                                    // v2 & v3
    legendname[0] = "No Laser";
    legendname[1] = "v2 SwitchA [1458mW]";
    legendname[2] = "v2 SwitchB [581mW]";
    legendname[3] = "v2 SwitchA & SwitchB [2039mW]";
    legendname[4] = "All Lasers of v2 & v3 on [3087mW]";
    pre_trig_frac = 0.7;
  } else {
    std::cout << "Unknown runname" << endl;
    exit(1);
  }

  int ndat = adcdataset.size();

  if ( ndat > 7 ) {
    std::cout << "ndat > 7, please increase array length of color, markerstyle, legendname, Count, and Gain" << endl;
    exit(1);
  }

  //==========================
  // CAEN Digitizer config
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

  // Single waveform
  TH1D *h1;
  TH1D *h2; // denoised
  TCanvas *cbaseline = new TCanvas();
  TLegend *legdataset = new TLegend(0.55, 0.65, 0.9, 0.9);

  //
  // At this point, printing a lot of stuff for the reader to check
  //

  std::cout << " === Printing user config === " << std::endl;
  std::cout << " * Run name: " << runname << std::endl;
  std::cout << " * Number of samples: " << ndat << std::endl;
  std::cout << " * Apply noise filter: " << filternoise << std::endl;
  std::cout << " * Max No. of events per sample: " << maxwavenum << std::endl;
  std::cout << " * Fraction of pre-trigger baseline: " << pre_trig_frac << std::endl;

  ifstream fin;

  for ( int idat = 0; idat < ndat; idat++ ) {
    std::cout << "Looking at dataset " << idat << ": " << adcdataset[idat] << std::endl;

    fin.open(TString::Format("%s/%s/%s", dirname.Data(), adcdataset[idat].Data(), channame.Data()), ios::in | ios::binary);

    if( !( fin.good() && fin.is_open() ) ) {
      std::cout << "File did not open!!" << endl;
      exit(1);
    };

    int waveNum = 0; // num of waveforms, set to 0 for each new dataset
    TH1F *BaselineADCHist = new TH1F("BaselineADCHist", "BaselineADCHist", 100, 3300, 3400);

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
      waveNum += 1;
      if( waveNum > maxwavenum ) break; // otherwise it may access non-existing memory
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

      // Fill baseline ADC of all waveforms in each dataset (one distribution per dataset)
      BaselineADCHist->Fill(baseline_ADC);

      //
      // Count up-crossings in each waveform
      //

      for(int ithres=0; ithres<nadcthrs; ++ithres){ // all thresholds
        for(int jpoint=0; jpoint<memorydepth*pre_trig_frac; jpoint++){ // range of points we are interested in
          if( ( denoised[jpoint] - (baseline_ADC+delta_ADC[ithres]) )<0 && ( denoised[jpoint+1] - (baseline_ADC+delta_ADC[ithres]) )>0 ){
            Count[idat][ithres] += 1;
          }
        }
      }

      // Integrate peak region
      // TODO

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

    fin.close();

    cout<<"Tot number of waveforms: " << waveNum <<endl;

    cout<<"========== Result =========== "<<endl;
    for(int ithres=0; ithres<nadcthrs; ++ithres){
      cout<<"Counts at delta ADC " << delta_ADC[ithres]<<": "<<Count[idat][ithres]<<endl;
    }

    // Plot baseline ADC distribution of each dataset on canvas
    cbaseline->cd();
    gStyle->SetOptStat(0);
    BaselineADCHist->SetLineColor(color[idat]);
    if ( idat == 0 ) {
      BaselineADCHist->GetXaxis()->SetTitle("Baseline ADC");
      BaselineADCHist->GetYaxis()->SetTitle("Events");
      BaselineADCHist->SetMaximum(maxwavenum);
      BaselineADCHist->Draw("HIST");
    } else {
      BaselineADCHist->Draw("HIST SAME");
    } // end plot
    legdataset->AddEntry(BaselineADCHist, TString::Format("%s", legendname[idat].Data()));

  } // end loop over dataset

  legdataset->Draw();
  gPad->RedrawAxis();
  cbaseline->SaveAs("Baseline_ADC_distribution_all_dataset.png");

  //
  // Calculate average (extra) upcrossings
  //

  for ( int idat = 0; idat < ndat; idat++ ) {
    for ( int ithres = 0; ithres < nadcthrs; ++ithres ) {
      if ( idat == 0 ) Gain[idat][ithres] = Count[idat][ithres]/pre_trig_frac/maxwavenum; // reference avg count
      else Gain[idat][ithres] = (Count[idat][ithres] - Count[0][ithres])/pre_trig_frac/maxwavenum; // extra upcrossings due to laser on
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
    dis[idat] = new TGraph(nadcthrs, delta_ADC, Count[idat]);
    dis[idat]->SetMarkerSize(0.8);
    dis[idat]->SetMarkerStyle(markerstyle[idat]);
    dis[idat]->SetMarkerColor(color[idat]);
    dis[idat]->SetLineColor(color[idat]);
    mg->Add(dis[idat]);
    leg->AddEntry(dis[idat], TString::Format("%s", legendname[idat].Data()));
  }

  mg->GetXaxis()->SetTitle("#Delta (ADC)");
  mg->GetYaxis()->SetTitle("Counts");
  mg->SetMaximum(Count[ndat-1][0]*2.); // highest laser power and lowest threshold
  mg->SetMinimum(Count[0][nadcthrs-1]/2.); // no laser and highest threshold
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
    diff[idat] = new TGraph(nadcthrs, delta_ADC, Gain[idat+1]);
    diff[idat]->SetMarkerSize(0.8);
    diff[idat]->SetMarkerStyle(markerstyle[idat+1]);
    diff[idat]->SetMarkerColor(color[idat+1]);
    diff[idat]->SetLineColor(color[idat+1]);
    mg2->Add(diff[idat]);
    leg2->AddEntry(diff[idat], TString::Format("%s", legendname[idat+1].Data()));
  }

  mg2->GetXaxis()->SetTitle("#Delta (ADC)");
  mg2->GetYaxis()->SetTitle("Extra counts per 10#mus");
  mg2->Draw("APL");

  leg2->Draw();
  c2->SaveAs("extra_up-crossing_per10us_count.png");

  // Plot the reference upcrossing counts per 10us
  TCanvas *c4 = new TCanvas();
  c4->SetGridx(); c4->SetGridy();
  TGraph *reference = new TGraph(nadcthrs, delta_ADC, Gain[0]);
  reference->SetMarkerSize(0.8);
  reference->SetMarkerStyle(21);
  reference->SetMarkerColor(1);
  reference->SetLineColor(1);
  reference->SetTitle("No Laser;#Delta (ADC);Counts per 10#mus");
  reference->Draw("APL");
  c4->SaveAs("up-crossing_per10us_reference.png");

}
