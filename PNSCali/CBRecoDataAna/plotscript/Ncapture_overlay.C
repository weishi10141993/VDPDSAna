// root -l -b -q  Ncapture_overlay.C

void Ncapture_overlay()
{
  int run_number = 1; // 25036, 25068, 25071, 1 (all runs but should never run 1?)
  std::cout << " run_number:   " << run_number << std::endl;
  // Overall sim normalization factor to PNS data SF (cosmic subtracted)
  //double FlukaMCsf = 6.0177; // normalize C3, Feb 2025 MC prod
  //double FlukaMCsf = 5.0511; // normalize C3, Aug 2025 MC prod, 100 PE thres
  //double FlukaMCsf = 4.8558; // normalize C3, Aug 2025 MC prod, adc+1 all chan
  //double FlukaMCsf = 5.319; // normalize C3, Aug 2025 MC prod, adc-1 all chan

  // PNS Run by run (cosmic subtracted) Fluka sim normalization factor
  // all normalize to C3 data
  double FlukaMCsf;
  if (run_number == 25036) FlukaMCsf = 2.5379;
  else if (run_number == 25068) FlukaMCsf = 1.9953;
  else if (run_number == 25071) FlukaMCsf = 0.5178;
  else if (run_number == 1) FlukaMCsf = 5.0511; // sim normalization for all 3 pns runs

  // PNS Data after selection
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib.root");

  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin100PE_syst_allchadcplus1.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin100PE_syst_allchadcminus1.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin50PE.root"); // have to use min50PE becaue C4 SF is 0.7
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min100PE_1200PEsplit_timing.root"); // Oct 29
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min50PE_max2100PE_1200PEsplit_timing.root"); // Oct 30
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_1200PEsplit_timing.root"); // Oct 30

  // JINS review //
  // Feb 13, 2026 JINST review comments
  // PNS RUN by RUN cosmic sf
  TFile *file0;
  double cosmicDatasf;
  if (run_number == 25036) {
    file0 = TFile::Open("CBAna_run25036_JINST_Timeslice_PE.root"); // time offset -305us
    cosmicDatasf = 0.2796; // for run 25036
  }
  else if (run_number == 25068) {
    file0 = TFile::Open("CBAna_run25068_JINST_Timeslice_PE.root"); // time offset -225us
    cosmicDatasf = 0.2121; // for run 25068
  }
  else if (run_number == 25071) {
    file0 = TFile::Open("CBAna_run25071_JINST_Timeslice_PE.root"); // time offset -305us
    cosmicDatasf = 0.1503; // for run 25068
  }
  else if (run_number == 1) {
    file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices.root");
    // OVERALL cosmic sf for all pns runs
    cosmicDatasf = 0.642;
  }


  TH1 *hpnsdataC4;
  TH1 *hpnsdataC4time;
  TH1 *hpnsdataC4timelowPE;
  TH1 *hpnsdataC4timehighPE;

  TH1 *hpnsdataC3;
  TH1 *hpnsdataC3time;
  TH1 *hpnsdataC3timelowPE;
  TH1 *hpnsdataC3timehighPE;

  TH1 *hpnsdataC2;
  TH1 *hpnsdataC2time;
  TH1 *hpnsdataC2timelowPE;
  TH1 *hpnsdataC2timehighPE;

  TH1 *hpnsdataC1;
  TH1 *hpnsdataC1time;
  TH1 *hpnsdataC1timelowPE;
  TH1 *hpnsdataC1timehighPE;

  TH1 *hcosmicdataC4;
  TH1 *hcosmicdataC4time;
  TH1 *hcosmicdataC4timelowPE;
  TH1 *hcosmicdataC4timehighPE;

  TH1 *hcosmicdataC3;
  TH1 *hcosmicdataC3time;
  TH1 *hcosmicdataC3timelowPE;
  TH1 *hcosmicdataC3timehighPE;

  TH1 *hcosmicdataC2;
  TH1 *hcosmicdataC2time;
  TH1 *hcosmicdataC2timelowPE;
  TH1 *hcosmicdataC2timehighPE;

  TH1 *hcosmicdataC1;
  TH1 *hcosmicdataC1time;
  TH1 *hcosmicdataC1timelowPE;
  TH1 *hcosmicdataC1timehighPE;

  TCanvas *c1 = new TCanvas();
  c1->cd();


  TTree *myMatchedC4Peaks_pns = (TTree*)file0->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE>>hpnsdataC4(50, 100, 2100)");
  hpnsdataC4 = (TH1*)gPad->GetPrimitive("hpnsdataC4");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time>>hpnsdataC4time(100, 0, 70000)");
  hpnsdataC4time = (TH1*)gPad->GetPrimitive("hpnsdataC4time");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time_100_1200PE>>hpnsdataC4timelowPE(100, 0, 70000)");
  hpnsdataC4timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC4timelowPE");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time_above_1200PE>>hpnsdataC4timehighPE(100, 0, 70000)");
  hpnsdataC4timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC4timehighPE");

  TTree *myMatchedC3Peaks_pns = (TTree*)file0->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE>>hpnsdataC3(50, 100, 2100)");
  hpnsdataC3 = (TH1*)gPad->GetPrimitive("hpnsdataC3");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time>>hpnsdataC3time(100, 0, 70000)");
  hpnsdataC3time = (TH1*)gPad->GetPrimitive("hpnsdataC3time");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time_100_1200PE>>hpnsdataC3timelowPE(100, 0, 70000)");
  hpnsdataC3timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC3timelowPE");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time_above_1200PE>>hpnsdataC3timehighPE(100, 0, 70000)");
  hpnsdataC3timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC3timehighPE");

  TTree *myMatchedC2Peaks_pns = (TTree*)file0->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE>>hpnsdataC2(50, 100, 2100)");
  hpnsdataC2 = (TH1*)gPad->GetPrimitive("hpnsdataC2");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time>>hpnsdataC2time(100, 0, 70000)");
  hpnsdataC2time = (TH1*)gPad->GetPrimitive("hpnsdataC2time");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time_100_1200PE>>hpnsdataC2timelowPE(100, 0, 70000)");
  hpnsdataC2timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC2timelowPE");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time_above_1200PE>>hpnsdataC2timehighPE(100, 0, 70000)");
  hpnsdataC2timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC2timehighPE");

  TTree *myMatchedC1Peaks_pns = (TTree*)file0->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE>>hpnsdataC1(50, 100, 2100)");
  hpnsdataC1 = (TH1*)gPad->GetPrimitive("hpnsdataC1");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time>>hpnsdataC1time(100, 0, 70000)");
  hpnsdataC1time = (TH1*)gPad->GetPrimitive("hpnsdataC1time");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time_100_1200PE>>hpnsdataC1timelowPE(100, 0, 70000)");
  hpnsdataC1timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC1timelowPE");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time_above_1200PE>>hpnsdataC1timehighPE(100, 0, 70000)");
  hpnsdataC1timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC1timehighPE");

  // cosmic bkg: data driven
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin100PE_syst_allchadcplus1.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin100PE_syst_allchadcminus1.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib_perXAtotPEmin50PE.root"); // have to use min50PE becaue C4 SF is 0.7
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_min100PE_1200PEsplit_timing.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_1200PEsplit_timing.root");

  // JINS review //
  TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices.root");  // Feb 13, 2026 JINST review comments

  TTree *myMatchedC4Peaks_cosmic = (TTree*)file1->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE>>hcosmicdataC4(50, 100, 2100)");
  hcosmicdataC4 = (TH1*)gPad->GetPrimitive("hcosmicdataC4");
  hcosmicdataC4->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time>>hcosmicdataC4time(100, 0, 70000)");
  hcosmicdataC4time = (TH1*)gPad->GetPrimitive("hcosmicdataC4time");
  hcosmicdataC4time->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time_100_1200PE>>hcosmicdataC4timelowPE(100, 0, 70000)");
  hcosmicdataC4timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC4timelowPE");
  hcosmicdataC4timelowPE->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time_above_1200PE>>hcosmicdataC4timehighPE(100, 0, 70000)");
  hcosmicdataC4timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC4timehighPE");
  hcosmicdataC4timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC3Peaks_cosmic = (TTree*)file1->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE>>hcosmicdataC3(50, 100, 2100)");
  hcosmicdataC3 = (TH1*)gPad->GetPrimitive("hcosmicdataC3");
  hcosmicdataC3->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time>>hcosmicdataC3time(100, 0, 70000)");
  hcosmicdataC3time = (TH1*)gPad->GetPrimitive("hcosmicdataC3time");
  hcosmicdataC3time->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time_100_1200PE>>hcosmicdataC3timelowPE(100, 0, 70000)");
  hcosmicdataC3timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC3timelowPE");
  hcosmicdataC3timelowPE->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time_above_1200PE>>hcosmicdataC3timehighPE(100, 0, 70000)");
  hcosmicdataC3timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC3timehighPE");
  hcosmicdataC3timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC2Peaks_cosmic = (TTree*)file1->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE>>hcosmicdataC2(50, 100, 2100)");
  hcosmicdataC2 = (TH1*)gPad->GetPrimitive("hcosmicdataC2");
  hcosmicdataC2->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time>>hcosmicdataC2time(100, 0, 70000)");
  hcosmicdataC2time = (TH1*)gPad->GetPrimitive("hcosmicdataC2time");
  hcosmicdataC2time->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time_100_1200PE>>hcosmicdataC2timelowPE(100, 0, 70000)");
  hcosmicdataC2timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC2timelowPE");
  hcosmicdataC2timelowPE->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time_above_1200PE>>hcosmicdataC2timehighPE(100, 0, 70000)");
  hcosmicdataC2timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC2timehighPE");
  hcosmicdataC2timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC1Peaks_cosmic = (TTree*)file1->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE>>hcosmicdataC1(50, 100, 2100)");
  hcosmicdataC1 = (TH1*)gPad->GetPrimitive("hcosmicdataC1");
  hcosmicdataC1->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time>>hcosmicdataC1time(100, 0, 70000)");
  hcosmicdataC1time = (TH1*)gPad->GetPrimitive("hcosmicdataC1time");
  hcosmicdataC1time->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time_100_1200PE>>hcosmicdataC1timelowPE(100, 0, 70000)");
  hcosmicdataC1timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC1timelowPE");
  hcosmicdataC1timelowPE->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time_above_1200PE>>hcosmicdataC1timehighPE(100, 0, 70000)");
  hcosmicdataC1timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC1timehighPE");
  hcosmicdataC1timehighPE->Scale(cosmicDatasf);


  // Fluka simulated inactive region bkg
  // 20 million sim neutrons/events
  // Recorded events correspond to about 1.5% of generated neutrons.
  // closest to PNS is 2
  // furthest to PNS is 1
  //TFile *file3 = TFile::Open("vbox_sep24_pe.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut_calibrated2peak.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut_calibrated2peak_Aug2025AdjustLY.root");

  // JINST review comments
  TFile *file3;
  if (run_number == 25036 || run_number == 25071 || run_number == 1  ) { // for all runs, it's tricky if timing is involved beacuse of different beam timing offset in runs
    file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset305us.root"); // time offset -305us
  }
  else if (run_number == 25068) {
    file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset225us.root"); // time offset -225us
  }

  TCanvas *c04_pe = (TCanvas*)file3->Get("c04_pe");
  TH1F *hinactivebkgC4 = (TH1F*)c04_pe->GetPrimitive("petotothC4");
  hinactivebkgC4->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC4 = (TH1F*)c04_pe->GetPrimitive("petotineC4");
  hinelasticbkgC4->Scale(FlukaMCsf);

  TCanvas *c03_pe = (TCanvas*)file3->Get("c03_pe");
  TH1F *hinactivebkgC3 = (TH1F*)c03_pe->GetPrimitive("petotothC3");
  hinactivebkgC3->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC3 = (TH1F*)c03_pe->GetPrimitive("petotineC3");
  hinelasticbkgC3->Scale(FlukaMCsf);

  TCanvas *c02_pe = (TCanvas*)file3->Get("c02_pe");
  TH1F *hinactivebkgC2 = (TH1F*)c02_pe->GetPrimitive("petotothC2");
  hinactivebkgC2->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC2 = (TH1F*)c02_pe->GetPrimitive("petotineC2");
  hinelasticbkgC2->Scale(FlukaMCsf);

  TCanvas *c01_pe = (TCanvas*)file3->Get("c01_pe");
  TH1F *hinactivebkgC1 = (TH1F*)c01_pe->GetPrimitive("petotothC1");
  hinactivebkgC1->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC1 = (TH1F*)c01_pe->GetPrimitive("petotineC1");
  hinelasticbkgC1->Scale(FlukaMCsf);

  TH1 *hsimsignalC4;
  TH1 *hsimsignalC3;
  TH1 *hsimsignalC2;
  TH1 *hsimsignalC1;
  // G4 simulated PNS signal: 1M captures (1 capture/event)
  // there is a swap of order due to G4 z axis definition...
  // XA5 shoule be C2 XA5Det_CaponXA5top_drift5_20_yz_abs_30
  // XA4 shoule be C3 XA4Det_CaponXA4top_drift5_20_yz_abs_30
  // XA3 shoule be C1 XA3Det_CaponXA3top_drift5_20_yz_abs_30
  // XA2 shoule be C4 XA2Det_CaponXA2top_drift5_20_yz_abs_30

  // Fluka sim PNS signal
  // same fluka scale factor
  //hsimsignalC4 = (TH1F*)c04_pe->GetPrimitive("petotcapontileC4");
  hsimsignalC4 = (TH1F*)c04_pe->GetPrimitive("petotcapC4");
  hsimsignalC4->Scale(FlukaMCsf);
  //hsimsignalC3 = (TH1F*)c03_pe->GetPrimitive("petotcapontileC3");
  hsimsignalC3 = (TH1F*)c03_pe->GetPrimitive("petotcapC3");
  hsimsignalC3->Scale(FlukaMCsf);
  //hsimsignalC2 = (TH1F*)c02_pe->GetPrimitive("petotcapontileC2");
  hsimsignalC2 = (TH1F*)c02_pe->GetPrimitive("petotcapC2");
  hsimsignalC2->Scale(FlukaMCsf);
  //hsimsignalC1 = (TH1F*)c01_pe->GetPrimitive("petotcapontileC1");
  hsimsignalC1 = (TH1F*)c01_pe->GetPrimitive("petotcapC1");
  hsimsignalC1->Scale(FlukaMCsf);


  TCanvas *can = new TCanvas();
  can->cd();

  THStack *stc4 = new THStack();
  stc4->SetMaximum(100000.);
  hcosmicdataC4->SetFillColor(2);
  hinactivebkgC4->SetFillColor(4);
  hinelasticbkgC4->SetFillColor(7);
  hsimsignalC4->SetFillColor(6);
  stc4->Add(hcosmicdataC4);
  stc4->Add(hinactivebkgC4);
  stc4->Add(hinelasticbkgC4);
  stc4->Add(hsimsignalC4);
  stc4->Draw("hist");
  stc4->GetXaxis()->SetTitle("PE on C4");
  can->Modified();


  hpnsdataC4->SetLineColor(1);
  hpnsdataC4->SetMarkerStyle(20);
  hpnsdataC4->SetMarkerSize(0.6);
  hpnsdataC4->Draw("E1 X0 SAME");

  std::cout << " hcosmicdataC4:   " << hcosmicdataC4->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC4:  " << hinactivebkgC4->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC4: " << hinelasticbkgC4->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC4:    " << hsimsignalC4->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC4:      " << hpnsdataC4->Integral(1, 50)     << std::endl;

  TLegend *legc4 = new TLegend(0.24,0.63,0.89,0.89);
  legc4->SetTextSize(0.05);
  legc4->AddEntry(hcosmicdataC4, "Cosmics (data driven)", "f");
  legc4->AddEntry(hinactivebkgC4, "All inactive materials neutron bkg (Fluka)", "f");
  legc4->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  legc4->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");
  legc4->AddEntry(hpnsdataC4, "Data", "ep");
  legc4->SetBorderSize(0);
  legc4->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_logY.root");
  can->SaveAs("PNSAnaC4_minimumcut_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut.root");
  can->SaveAs("PNSAnaC4_minimumcut.pdf");

  // only plot cosmics
  hcosmicdataC4->SetStats(0);
  hcosmicdataC4->GetXaxis()->SetTitle("PE on C4");
  hcosmicdataC4->SetTitle("");
  hcosmicdataC4->Draw("hist");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics.pdf");

  // subtract cosmics from pns data
  THStack *stc4nocosmic = new THStack();
  stc4nocosmic->SetMaximum(60000.);
  stc4nocosmic->Add(hinactivebkgC4);
  stc4nocosmic->Add(hinelasticbkgC4);
  stc4nocosmic->Add(hsimsignalC4);
  stc4nocosmic->Draw("hist");
  stc4nocosmic->GetXaxis()->SetTitle("PE on C4");
  can->Modified();

  hpnsdataC4->Add(hcosmicdataC4, -1);
  hpnsdataC4->SetLineColor(1);
  hpnsdataC4->SetMarkerStyle(20);
  hpnsdataC4->SetMarkerSize(0.6);
  hpnsdataC4->Draw("E1 X0 SAME");

  std::cout << " C4 data-cosmic mean PE:   " << hpnsdataC4->GetMean() << std::endl;

  TLegend *legc4nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc4nocosmic->SetTextSize(0.05);
  legc4nocosmic->AddEntry(hinactivebkgC4, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmic->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmic->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");
  legc4nocosmic->AddEntry(hpnsdataC4, "Data (cosmics subtracted)", "ep");
  legc4nocosmic->SetBorderSize(0);
  legc4nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC4_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic.pdf");

  // plot cosmic only timing
  gPad->SetLogy(0);
  gStyle->SetOptStat(0);
  hcosmicdataC4time->SetLineColor(1);
  hcosmicdataC4time->SetMarkerStyle(20);
  hcosmicdataC4time->SetMarkerSize(0.6);
  hcosmicdataC4time->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing.pdf");

  hcosmicdataC4timelowPE->SetLineColor(1);
  hcosmicdataC4timelowPE->SetMarkerStyle(20);
  hcosmicdataC4timelowPE->SetMarkerSize(0.6);
  hcosmicdataC4timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing_lowPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing_lowPE.pdf");

  hcosmicdataC4timehighPE->SetLineColor(1);
  hcosmicdataC4timehighPE->SetMarkerStyle(20);
  hcosmicdataC4timehighPE->SetMarkerSize(0.6);
  hcosmicdataC4timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing_highPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmiconly_timing_highPE.pdf");
  std::cout << " C4 time cosmic only tot:   "             << hcosmicdataC4time->Integral(1, 100)         << std::endl;
  std::cout << " C4 time cosmic only PE<1200 tot:   "     << hcosmicdataC4timelowPE->Integral(1, 100)    << std::endl;
  std::cout << " C4 time cosmic only PE>1200 tot:   "     << hcosmicdataC4timehighPE->Integral(1, 100)   << std::endl;

  // plot pns run timing only
  hpnsdataC4time->SetLineColor(1);
  hpnsdataC4time->SetMarkerStyle(20);
  hpnsdataC4time->SetMarkerSize(0.6);
  hpnsdataC4time->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing.root");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing.pdf");

  hpnsdataC4timelowPE->SetLineColor(1);
  hpnsdataC4timelowPE->SetMarkerStyle(20);
  hpnsdataC4timelowPE->SetMarkerSize(0.6);
  hpnsdataC4timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing_lowPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing_lowPE.pdf");

  hpnsdataC4timehighPE->SetLineColor(1);
  hpnsdataC4timehighPE->SetMarkerStyle(20);
  hpnsdataC4timehighPE->SetMarkerSize(0.6);
  hpnsdataC4timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing_highPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_pnsrun_timing_highPE.pdf");
  std::cout << " C4 time pns run tot:   "         << hpnsdataC4time->Integral(1, 100)        << std::endl;
  std::cout << " C4 time pns run PE<1200 tot:   " << hpnsdataC4timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C4 time pns run PE>1200 tot:   " << hpnsdataC4timehighPE->Integral(1, 100)  << std::endl;

  hpnsdataC4time->Add(hcosmicdataC4time, -1);
  hpnsdataC4time->SetLineColor(1);
  hpnsdataC4time->SetMarkerStyle(20);
  hpnsdataC4time->SetMarkerSize(0.6);
  hpnsdataC4time->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing.pdf");

  hpnsdataC4timelowPE->Add(hcosmicdataC4timelowPE, -1);
  hpnsdataC4timelowPE->SetLineColor(1);
  hpnsdataC4timelowPE->SetMarkerStyle(20);
  hpnsdataC4timelowPE->SetMarkerSize(0.6);
  hpnsdataC4timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_lowPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_lowPE.pdf");

  hpnsdataC4timehighPE->Add(hcosmicdataC4timehighPE, -1);
  hpnsdataC4timehighPE->SetLineColor(1);
  hpnsdataC4timehighPE->SetMarkerStyle(20);
  hpnsdataC4timehighPE->SetMarkerSize(0.6);
  hpnsdataC4timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_highPE.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_highPE.pdf");
  std::cout << " C4 time (pns - cosmic) tot:   "         << hpnsdataC4time->Integral(1, 100)        << std::endl;
  std::cout << " C4 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC4timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C4 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC4timehighPE->Integral(1, 100)  << std::endl;

  gPad->SetLogy(0);
  THStack *stc3 = new THStack();
  stc3->SetMaximum(30000.);
  hcosmicdataC3->SetFillColor(2);
  hinactivebkgC3->SetFillColor(4);
  hinelasticbkgC3->SetFillColor(7);
  hsimsignalC3->SetFillColor(6);
  stc3->Add(hcosmicdataC3);
  stc3->Add(hinactivebkgC3);
  stc3->Add(hinelasticbkgC3);
  stc3->Add(hsimsignalC3);
  stc3->Draw("hist");
  stc3->GetXaxis()->SetTitle("PE on C3");
  can->Modified();


  hpnsdataC3->SetLineColor(1);
  hpnsdataC3->SetMarkerStyle(20);
  hpnsdataC3->SetMarkerSize(0.6);
  hpnsdataC3->Draw("E1 X0 SAME");

  std::cout << " hcosmicdataC3:   " << hcosmicdataC3->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC3:  " << hinactivebkgC3->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC3: " << hinelasticbkgC3->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC3:    " << hsimsignalC3->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC3:      " << hpnsdataC3->Integral(1, 50)      << std::endl;

  TLegend *legc3 = new TLegend(0.24,0.63,0.89,0.89);
  legc3->SetTextSize(0.05);
  legc3->AddEntry(hcosmicdataC3, "Cosmics (data driven)", "f");
  legc3->AddEntry(hinactivebkgC3, "All inactive materials neutron bkg (Fluka)", "f");
  legc3->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  legc3->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");
  legc3->AddEntry(hpnsdataC3, "Data", "ep");
  legc3->SetBorderSize(0);
  legc3->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_logY.root");
  can->SaveAs("PNSAnaC3_minimumcut_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut.root");
  can->SaveAs("PNSAnaC3_minimumcut.pdf");

  // only plot cosmics
  hcosmicdataC3->SetStats(0);
  hcosmicdataC3->GetXaxis()->SetTitle("PE on C3");
  hcosmicdataC3->SetTitle("");
  hcosmicdataC3->Draw("hist");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics.pdf");

  // subtract cosmics from pns data
  THStack *stc3nocosmic = new THStack();
  stc3nocosmic->SetMaximum(10000.);
  stc3nocosmic->Add(hinactivebkgC3);
  stc3nocosmic->Add(hinelasticbkgC3);
  stc3nocosmic->Add(hsimsignalC3);
  stc3nocosmic->Draw("hist");
  stc3nocosmic->GetXaxis()->SetTitle("PE on C3");
  can->Modified();

  hpnsdataC3->Add(hcosmicdataC3, -1);
  hpnsdataC3->SetLineColor(1);
  hpnsdataC3->SetMarkerStyle(20);
  hpnsdataC3->SetMarkerSize(0.6);
  hpnsdataC3->Draw("E1 X0 SAME");

  std::cout << " C3 data-cosmic mean PE:   " << hpnsdataC3->GetMean() << std::endl;

  TLegend *legc3nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc3nocosmic->SetTextSize(0.05);
  legc3nocosmic->AddEntry(hinactivebkgC3, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmic->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmic->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");
  legc3nocosmic->AddEntry(hpnsdataC3, "Data (cosmics subtracted)", "ep");
  legc3nocosmic->SetBorderSize(0);
  legc3nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC3_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic.pdf");

  gPad->SetLogy(0);
  gStyle->SetOptStat(0);
  hcosmicdataC3time->SetLineColor(1);
  hcosmicdataC3time->SetMarkerStyle(20);
  hcosmicdataC3time->SetMarkerSize(0.6);
  hcosmicdataC3time->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing.pdf");

  hcosmicdataC3timelowPE->SetLineColor(1);
  hcosmicdataC3timelowPE->SetMarkerStyle(20);
  hcosmicdataC3timelowPE->SetMarkerSize(0.6);
  hcosmicdataC3timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing_lowPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing_lowPE.pdf");

  hcosmicdataC3timehighPE->SetLineColor(1);
  hcosmicdataC3timehighPE->SetMarkerStyle(20);
  hcosmicdataC3timehighPE->SetMarkerSize(0.6);
  hcosmicdataC3timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing_highPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmiconly_timing_highPE.pdf");
  std::cout << " C3 time cosmic only tot:   "             << hcosmicdataC3time->Integral(1, 100)         << std::endl;
  std::cout << " C3 time cosmic only PE<1200 tot:   "     << hcosmicdataC3timelowPE->Integral(1, 100)    << std::endl;
  std::cout << " C3 time cosmic only PE>1200 tot:   "     << hcosmicdataC3timehighPE->Integral(1, 100)   << std::endl;

  // plot pns run timing only
  hpnsdataC3time->SetLineColor(1);
  hpnsdataC3time->SetMarkerStyle(20);
  hpnsdataC3time->SetMarkerSize(0.6);
  hpnsdataC3time->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing.root");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing.pdf");

  hpnsdataC3timelowPE->SetLineColor(1);
  hpnsdataC3timelowPE->SetMarkerStyle(20);
  hpnsdataC3timelowPE->SetMarkerSize(0.6);
  hpnsdataC3timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing_lowPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing_lowPE.pdf");

  hpnsdataC3timehighPE->SetLineColor(1);
  hpnsdataC3timehighPE->SetMarkerStyle(20);
  hpnsdataC3timehighPE->SetMarkerSize(0.6);
  hpnsdataC3timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing_highPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_pnsrun_timing_highPE.pdf");
  std::cout << " C3 time pns run tot:   "         << hpnsdataC3time->Integral(1, 100)        << std::endl;
  std::cout << " C3 time pns run PE<1200 tot:   " << hpnsdataC3timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C3 time pns run PE>1200 tot:   " << hpnsdataC3timehighPE->Integral(1, 100)  << std::endl;

  hpnsdataC3time->Add(hcosmicdataC3time, -1);
  hpnsdataC3time->SetLineColor(1);
  hpnsdataC3time->SetMarkerStyle(20);
  hpnsdataC3time->SetMarkerSize(0.6);
  hpnsdataC3time->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing.pdf");

  hpnsdataC3timelowPE->Add(hcosmicdataC3timelowPE, -1);
  hpnsdataC3timelowPE->SetLineColor(1);
  hpnsdataC3timelowPE->SetMarkerStyle(20);
  hpnsdataC3timelowPE->SetMarkerSize(0.6);
  hpnsdataC3timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_lowPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_lowPE.pdf");

  hpnsdataC3timehighPE->Add(hcosmicdataC3timehighPE, -1);
  hpnsdataC3timehighPE->SetLineColor(1);
  hpnsdataC3timehighPE->SetMarkerStyle(20);
  hpnsdataC3timehighPE->SetMarkerSize(0.6);
  hpnsdataC3timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_highPE.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_highPE.pdf");
  std::cout << " C3 time (pns - cosmic) tot:   "         << hpnsdataC3time->Integral(1, 100)        << std::endl;
  std::cout << " C3 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC3timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C3 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC3timehighPE->Integral(1, 100)  << std::endl;


  gPad->SetLogy(0);
  THStack *stc2 = new THStack();
  stc2->SetMaximum(30000.);
  hcosmicdataC2->SetFillColor(2);
  hinactivebkgC2->SetFillColor(4);
  hinelasticbkgC2->SetFillColor(7);
  hsimsignalC2->SetFillColor(6);
  stc2->Add(hcosmicdataC2);
  stc2->Add(hinactivebkgC2);
  stc2->Add(hinelasticbkgC2);
  stc2->Add(hsimsignalC2);
  stc2->Draw("hist");
  stc2->GetXaxis()->SetTitle("PE on C2");
  can->Modified();

  hpnsdataC2->SetLineColor(1);
  hpnsdataC2->SetMarkerStyle(20);
  hpnsdataC2->SetMarkerSize(0.6);
  hpnsdataC2->Draw("E1 X0 SAME");

  std::cout << " hcosmicdataC2:   " << hcosmicdataC2->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC2:  " << hinactivebkgC2->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC2: " << hinelasticbkgC2->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC2:    " << hsimsignalC2->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC2:      " << hpnsdataC2->Integral(1, 50)      << std::endl;

  TLegend *legc2 = new TLegend(0.24,0.63,0.89,0.89);
  legc2->SetTextSize(0.05);
  legc2->AddEntry(hcosmicdataC2, "Cosmics (data driven)", "f");
  legc2->AddEntry(hinactivebkgC2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  legc2->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");
  legc2->AddEntry(hpnsdataC2, "Data", "ep");
  legc2->SetBorderSize(0);
  legc2->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_logY.root");
  can->SaveAs("PNSAnaC2_minimumcut_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut.root");
  can->SaveAs("PNSAnaC2_minimumcut.pdf");

  // only plot cosmics
  hcosmicdataC2->SetStats(0);
  hcosmicdataC2->GetXaxis()->SetTitle("PE on C2");
  hcosmicdataC2->SetTitle("");
  hcosmicdataC2->Draw("hist");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics.pdf");

  // subtract cosmics from pns data
  THStack *stc2nocosmic = new THStack();
  stc2nocosmic->SetMaximum(10000.);
  stc2nocosmic->Add(hinactivebkgC2);
  stc2nocosmic->Add(hinelasticbkgC2);
  stc2nocosmic->Add(hsimsignalC2);
  stc2nocosmic->Draw("hist");
  stc2nocosmic->GetXaxis()->SetTitle("PE on C2");
  can->Modified();


  hpnsdataC2->Add(hcosmicdataC2, -1);
  hpnsdataC2->SetLineColor(1);
  hpnsdataC2->SetMarkerStyle(20);
  hpnsdataC2->SetMarkerSize(0.6);
  hpnsdataC2->Draw("E1 X0 SAME");

  std::cout << " C2 data-cosmic mean PE:   " << hpnsdataC2->GetMean() << std::endl;

  TLegend *legc2nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc2nocosmic->SetTextSize(0.05);
  legc2nocosmic->AddEntry(hinactivebkgC2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmic->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmic->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");
  legc2nocosmic->AddEntry(hpnsdataC2, "Data (cosmics subtracted)", "ep");
  legc2nocosmic->SetBorderSize(0);
  legc2nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC2_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic.pdf");

  gPad->SetLogy(0);
  gStyle->SetOptStat(0);
  hcosmicdataC2time->SetLineColor(1);
  hcosmicdataC2time->SetMarkerStyle(20);
  hcosmicdataC2time->SetMarkerSize(0.6);
  hcosmicdataC2time->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing.pdf");

  hcosmicdataC2timelowPE->SetLineColor(1);
  hcosmicdataC2timelowPE->SetMarkerStyle(20);
  hcosmicdataC2timelowPE->SetMarkerSize(0.6);
  hcosmicdataC2timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing_lowPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing_lowPE.pdf");

  hcosmicdataC2timehighPE->SetLineColor(1);
  hcosmicdataC2timehighPE->SetMarkerStyle(20);
  hcosmicdataC2timehighPE->SetMarkerSize(0.6);
  hcosmicdataC2timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing_highPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmiconly_timing_highPE.pdf");
  std::cout << " C2 time cosmic only tot:   "             << hcosmicdataC2time->Integral(1, 100)         << std::endl;
  std::cout << " C2 time cosmic only PE<1200 tot:   "     << hcosmicdataC2timelowPE->Integral(1, 100)    << std::endl;
  std::cout << " C2 time cosmic only PE>1200 tot:   "     << hcosmicdataC2timehighPE->Integral(1, 100)   << std::endl;

  // plot pns run timing only
  hpnsdataC2time->SetLineColor(1);
  hpnsdataC2time->SetMarkerStyle(20);
  hpnsdataC2time->SetMarkerSize(0.6);
  hpnsdataC2time->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing.root");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing.pdf");

  hpnsdataC2timelowPE->SetLineColor(1);
  hpnsdataC2timelowPE->SetMarkerStyle(20);
  hpnsdataC2timelowPE->SetMarkerSize(0.6);
  hpnsdataC2timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing_lowPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing_lowPE.pdf");

  hpnsdataC2timehighPE->SetLineColor(1);
  hpnsdataC2timehighPE->SetMarkerStyle(20);
  hpnsdataC2timehighPE->SetMarkerSize(0.6);
  hpnsdataC2timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing_highPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_pnsrun_timing_highPE.pdf");
  std::cout << " C2 time pns run tot:   "         << hpnsdataC2time->Integral(1, 100)        << std::endl;
  std::cout << " C2 time pns run PE<1200 tot:   " << hpnsdataC2timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C2 time pns run PE>1200 tot:   " << hpnsdataC2timehighPE->Integral(1, 100)  << std::endl;

  hpnsdataC2time->Add(hcosmicdataC2time, -1);
  hpnsdataC2time->SetLineColor(1);
  hpnsdataC2time->SetMarkerStyle(20);
  hpnsdataC2time->SetMarkerSize(0.6);
  hpnsdataC2time->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing.pdf");

  hpnsdataC2timelowPE->Add(hcosmicdataC2timelowPE, -1);
  hpnsdataC2timelowPE->SetLineColor(1);
  hpnsdataC2timelowPE->SetMarkerStyle(20);
  hpnsdataC2timelowPE->SetMarkerSize(0.6);
  hpnsdataC2timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_lowPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_lowPE.pdf");

  hpnsdataC2timehighPE->Add(hcosmicdataC2timehighPE, -1);
  hpnsdataC2timehighPE->SetLineColor(1);
  hpnsdataC2timehighPE->SetMarkerStyle(20);
  hpnsdataC2timehighPE->SetMarkerSize(0.6);
  hpnsdataC2timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_highPE.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_highPE.pdf");
  std::cout << " C2 time (pns - cosmic) tot:   "         << hpnsdataC2time->Integral(1, 100)        << std::endl;
  std::cout << " C2 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC2timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C2 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC2timehighPE->Integral(1, 100)  << std::endl;


  gPad->SetLogy(0);
  THStack *stc1 = new THStack();
  stc1->SetMaximum(50000.);
  hcosmicdataC1->SetFillColor(2);
  hinactivebkgC1->SetFillColor(4);
  hinelasticbkgC1->SetFillColor(7);
  hsimsignalC1->SetFillColor(6);
  stc1->Add(hcosmicdataC1);
  stc1->Add(hinactivebkgC1);
  stc1->Add(hinelasticbkgC1);
  stc1->Add(hsimsignalC1);
  stc1->Draw("hist");
  stc1->GetXaxis()->SetTitle("PE on C1");
  can->Modified();


  hpnsdataC1->SetLineColor(1);
  hpnsdataC1->SetMarkerStyle(20);
  hpnsdataC1->SetMarkerSize(0.6);
  hpnsdataC1->Draw("E1 X0 SAME");

  std::cout << " hcosmicdataC1:   " << hcosmicdataC1->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC1:  " << hinactivebkgC1->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC1: " << hinelasticbkgC1->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC1:    " << hsimsignalC1->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC1:      " << hpnsdataC1->Integral(1, 50)      << std::endl;

  TLegend *legc1 = new TLegend(0.24,0.63,0.89,0.89);
  legc1->SetTextSize(0.05);
  legc1->AddEntry(hcosmicdataC1, "Cosmics (data driven)", "f");
  legc1->AddEntry(hinactivebkgC1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  legc1->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");
  legc1->AddEntry(hpnsdataC1, "Data", "ep");
  legc1->SetBorderSize(0);
  legc1->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_logY.root");
  can->SaveAs("PNSAnaC1_minimumcut_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut.root");
  can->SaveAs("PNSAnaC1_minimumcut.pdf");

  // only plot cosmics
  hcosmicdataC1->SetStats(0);
  hcosmicdataC1->GetXaxis()->SetTitle("PE on C1");
  hcosmicdataC1->SetTitle("");
  hcosmicdataC1->Draw("hist");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics.pdf");

  // subtract cosmics from pns data
  THStack *stc1nocosmic = new THStack();
  stc1nocosmic->SetMaximum(60000.);
  stc1nocosmic->Add(hinactivebkgC1);
  stc1nocosmic->Add(hinelasticbkgC1);
  stc1nocosmic->Add(hsimsignalC1);
  stc1nocosmic->Draw("hist");
  stc1nocosmic->GetXaxis()->SetTitle("PE on C1");
  can->Modified();

  hpnsdataC1->Add(hcosmicdataC1, -1);
  hpnsdataC1->SetLineColor(1);
  hpnsdataC1->SetMarkerStyle(20);
  hpnsdataC1->SetMarkerSize(0.6);
  hpnsdataC1->Draw("E1 X0 SAME");

  std::cout << " C1 data-cosmic mean PE:   " << hpnsdataC1->GetMean() << std::endl;

  TLegend *legc1nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc1nocosmic->SetTextSize(0.05);
  legc1nocosmic->AddEntry(hinactivebkgC1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmic->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmic->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");
  legc1nocosmic->AddEntry(hpnsdataC1, "Data (cosmics subtracted)", "ep");
  legc1nocosmic->SetBorderSize(0);
  legc1nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC1_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic.pdf");

  gPad->SetLogy(0);
  gStyle->SetOptStat(0);
  hcosmicdataC1time->SetLineColor(1);
  hcosmicdataC1time->SetMarkerStyle(20);
  hcosmicdataC1time->SetMarkerSize(0.6);
  hcosmicdataC1time->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing.pdf");

  hcosmicdataC1timelowPE->SetLineColor(1);
  hcosmicdataC1timelowPE->SetMarkerStyle(20);
  hcosmicdataC1timelowPE->SetMarkerSize(0.6);
  hcosmicdataC1timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing_lowPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing_lowPE.pdf");

  hcosmicdataC1timehighPE->SetLineColor(1);
  hcosmicdataC1timehighPE->SetMarkerStyle(20);
  hcosmicdataC1timehighPE->SetMarkerSize(0.6);
  hcosmicdataC1timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing_highPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmiconly_timing_highPE.pdf");
  std::cout << " C1 time cosmic only tot:   "             << hcosmicdataC1time->Integral(1, 100)         << std::endl;
  std::cout << " C1 time cosmic only PE<1200 tot:   "     << hcosmicdataC1timelowPE->Integral(1, 100)    << std::endl;
  std::cout << " C1 time cosmic only PE>1200 tot:   "     << hcosmicdataC1timehighPE->Integral(1, 100)   << std::endl;

  // plot pns run timing only
  hpnsdataC1time->SetLineColor(1);
  hpnsdataC1time->SetMarkerStyle(20);
  hpnsdataC1time->SetMarkerSize(0.6);
  hpnsdataC1time->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing.root");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing.pdf");

  hpnsdataC1timelowPE->SetLineColor(1);
  hpnsdataC1timelowPE->SetMarkerStyle(20);
  hpnsdataC1timelowPE->SetMarkerSize(0.6);
  hpnsdataC1timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing_lowPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing_lowPE.pdf");

  hpnsdataC1timehighPE->SetLineColor(1);
  hpnsdataC1timehighPE->SetMarkerStyle(20);
  hpnsdataC1timehighPE->SetMarkerSize(0.6);
  hpnsdataC1timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing_highPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_pnsrun_timing_highPE.pdf");
  std::cout << " C1 time pns run tot:   "         << hpnsdataC1time->Integral(1, 100)        << std::endl;
  std::cout << " C1 time pns run PE<1200 tot:   " << hpnsdataC1timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C1 time pns run PE>1200 tot:   " << hpnsdataC1timehighPE->Integral(1, 100)  << std::endl;

  hpnsdataC1time->Add(hcosmicdataC1time, -1);
  hpnsdataC1time->SetLineColor(1);
  hpnsdataC1time->SetMarkerStyle(20);
  hpnsdataC1time->SetMarkerSize(0.6);
  hpnsdataC1time->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing.pdf");

  hpnsdataC1timelowPE->Add(hcosmicdataC1timelowPE, -1);
  hpnsdataC1timelowPE->SetLineColor(1);
  hpnsdataC1timelowPE->SetMarkerStyle(20);
  hpnsdataC1timelowPE->SetMarkerSize(0.6);
  hpnsdataC1timelowPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_lowPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_lowPE.pdf");

  hpnsdataC1timehighPE->Add(hcosmicdataC1timehighPE, -1);
  hpnsdataC1timehighPE->SetLineColor(1);
  hpnsdataC1timehighPE->SetMarkerStyle(20);
  hpnsdataC1timehighPE->SetMarkerSize(0.6);
  hpnsdataC1timehighPE->Draw("E1 X0");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_highPE.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_highPE.pdf");
  std::cout << " C1 time (pns - cosmic) tot:   "         << hpnsdataC1time->Integral(1, 100)        << std::endl;
  std::cout << " C1 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC1timelowPE->Integral(1, 100)   << std::endl;
  std::cout << " C1 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC1timehighPE->Integral(1, 100)  << std::endl;

}
