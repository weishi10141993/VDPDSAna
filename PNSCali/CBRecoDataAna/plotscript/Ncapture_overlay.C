// root -l -b -q  Ncapture_overlay.C

void Ncapture_overlay()
{
  int run_number = 1; // 25036, 25068, 25071, 1 (all runs)
  std::cout << " run_number:   " << run_number << std::endl;

  // PNS Run by run (cosmic subtracted) Fluka sim normalization factor
  // all normalize to C3 data
  // Overall sim normalization factor to PNS data SF (cosmic subtracted)
  double FlukaMCsf = 0.0, FlukaMCsfoffset200us = 0.0, FlukaMCsfoffset280us  = 0.0;
  //double FlukaMCsf = 5.0511; // normalize C3, Aug 2025 MC prod, 100 PE thres
  if (run_number == 25036) FlukaMCsf = 3.5197;
  else if (run_number == 25068) FlukaMCsf = 2.4439;
  else if (run_number == 25071) FlukaMCsf = 0.7182;
  else if (run_number == 1) {
    FlukaMCsfoffset280us = 4.2379;
    FlukaMCsfoffset200us = 2.4439;
  }

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
    //file0 = TFile::Open("CBAna_run25036_JINST_Timeslice_PE.root");
    file0 = TFile::Open("CBAna_run25036_JINST_Timeslice_PE_DAQtcut.root");
    cosmicDatasf = 0.2796; // for run 25036
  }
  else if (run_number == 25068) {
    //file0 = TFile::Open("CBAna_run25068_JINST_Timeslice_PE.root");
    file0 = TFile::Open("CBAna_run25068_JINST_Timeslice_PE_DAQtcut.root");
    cosmicDatasf = 0.2121; // for run 25068
  }
  else if (run_number == 25071) {
    //file0 = TFile::Open("CBAna_run25071_JINST_Timeslice_PE.root");
    file0 = TFile::Open("CBAna_run25071_JINST_Timeslice_PE_DAQtcut.root");
    cosmicDatasf = 0.1503; // for run 25068
  }
  else if (run_number == 1) {
    //file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices.root");
    file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices_DAQtimecut.root");
    // OVERALL cosmic sf for all pns runs
    cosmicDatasf = 0.642;
  }


  TH1 *hpnsdataC4;
  TH1 *hpnsdataC4petimeslice0;
  TH1 *hpnsdataC4petimeslice1;
  TH1 *hpnsdataC4petimeslice2;
  TH1 *hpnsdataC4time;
  TH1 *hpnsdataC4timelowPE;
  TH1 *hpnsdataC4timehighPE;

  TH1 *hpnsdataC3;
  TH1 *hpnsdataC3petimeslice0;
  TH1 *hpnsdataC3petimeslice1;
  TH1 *hpnsdataC3petimeslice2;
  TH1 *hpnsdataC3time;
  TH1 *hpnsdataC3timelowPE;
  TH1 *hpnsdataC3timehighPE;

  TH1 *hpnsdataC2;
  TH1 *hpnsdataC2petimeslice0;
  TH1 *hpnsdataC2petimeslice1;
  TH1 *hpnsdataC2petimeslice2;
  TH1 *hpnsdataC2time;
  TH1 *hpnsdataC2timelowPE;
  TH1 *hpnsdataC2timehighPE;

  TH1 *hpnsdataC1;
  TH1 *hpnsdataC1petimeslice0;
  TH1 *hpnsdataC1petimeslice1;
  TH1 *hpnsdataC1petimeslice2;
  TH1 *hpnsdataC1time;
  TH1 *hpnsdataC1timelowPE;
  TH1 *hpnsdataC1timehighPE;

  TH1 *hcosmicdataC4;
  TH1 *hcosmicdataC4petimeslice0;
  TH1 *hcosmicdataC4petimeslice1;
  TH1 *hcosmicdataC4petimeslice2;
  TH1 *hcosmicdataC4time;
  TH1 *hcosmicdataC4timelowPE;
  TH1 *hcosmicdataC4timehighPE;

  TH1 *hcosmicdataC3;
  TH1 *hcosmicdataC3petimeslice0;
  TH1 *hcosmicdataC3petimeslice1;
  TH1 *hcosmicdataC3petimeslice2;
  TH1 *hcosmicdataC3time;
  TH1 *hcosmicdataC3timelowPE;
  TH1 *hcosmicdataC3timehighPE;

  TH1 *hcosmicdataC2;
  TH1 *hcosmicdataC2petimeslice0;
  TH1 *hcosmicdataC2petimeslice1;
  TH1 *hcosmicdataC2petimeslice2;
  TH1 *hcosmicdataC2time;
  TH1 *hcosmicdataC2timelowPE;
  TH1 *hcosmicdataC2timehighPE;

  TH1 *hcosmicdataC1;
  TH1 *hcosmicdataC1petimeslice0;
  TH1 *hcosmicdataC1petimeslice1;
  TH1 *hcosmicdataC1petimeslice2;
  TH1 *hcosmicdataC1time;
  TH1 *hcosmicdataC1timelowPE;
  TH1 *hcosmicdataC1timehighPE;

  TCanvas *c1 = new TCanvas();
  c1->cd();

  TTree *myMatchedC4Peaks_pns = (TTree*)file0->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE>>hpnsdataC4(50, 100, 2100)");
  hpnsdataC4 = (TH1*)gPad->GetPrimitive("hpnsdataC4");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE_0_10kpdticks>>hpnsdataC4petimeslice0(50, 100, 2100)");
  hpnsdataC4petimeslice0 = (TH1*)gPad->GetPrimitive("hpnsdataC4petimeslice0");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE_10k_30kpdticks>>hpnsdataC4petimeslice1(50, 100, 2100)");
  hpnsdataC4petimeslice1 = (TH1*)gPad->GetPrimitive("hpnsdataC4petimeslice1");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE_30kpluspdticks>>hpnsdataC4petimeslice2(50, 100, 2100)");
  hpnsdataC4petimeslice2 = (TH1*)gPad->GetPrimitive("hpnsdataC4petimeslice2");

  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time>>hpnsdataC4time(112, 0, 70000)"); // note this is with PE cut 100-2100
  hpnsdataC4time = (TH1*)gPad->GetPrimitive("hpnsdataC4time");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time_100_1200PE>>hpnsdataC4timelowPE(112, 0, 70000)");
  hpnsdataC4timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC4timelowPE");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_Time_above_1200PE>>hpnsdataC4timehighPE(112, 0, 70000)");
  hpnsdataC4timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC4timehighPE");

  TTree *myMatchedC3Peaks_pns = (TTree*)file0->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE>>hpnsdataC3(50, 100, 2100)");
  hpnsdataC3 = (TH1*)gPad->GetPrimitive("hpnsdataC3");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE_0_10kpdticks>>hpnsdataC3petimeslice0(50, 100, 2100)");
  hpnsdataC3petimeslice0 = (TH1*)gPad->GetPrimitive("hpnsdataC3petimeslice0");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE_10k_30kpdticks>>hpnsdataC3petimeslice1(50, 100, 2100)");
  hpnsdataC3petimeslice1 = (TH1*)gPad->GetPrimitive("hpnsdataC3petimeslice1");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE_30kpluspdticks>>hpnsdataC3petimeslice2(50, 100, 2100)");
  hpnsdataC3petimeslice2 = (TH1*)gPad->GetPrimitive("hpnsdataC3petimeslice2");

  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time>>hpnsdataC3time(112, 0, 70000)"); // note this is with PE cut 100-2100
  hpnsdataC3time = (TH1*)gPad->GetPrimitive("hpnsdataC3time");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time_100_1200PE>>hpnsdataC3timelowPE(112, 0, 70000)");
  hpnsdataC3timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC3timelowPE");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_Time_above_1200PE>>hpnsdataC3timehighPE(112, 0, 70000)");
  hpnsdataC3timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC3timehighPE");

  TTree *myMatchedC2Peaks_pns = (TTree*)file0->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE>>hpnsdataC2(50, 100, 2100)");
  hpnsdataC2 = (TH1*)gPad->GetPrimitive("hpnsdataC2");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE_0_10kpdticks>>hpnsdataC2petimeslice0(50, 100, 2100)");
  hpnsdataC2petimeslice0 = (TH1*)gPad->GetPrimitive("hpnsdataC2petimeslice0");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE_10k_30kpdticks>>hpnsdataC2petimeslice1(50, 100, 2100)");
  hpnsdataC2petimeslice1 = (TH1*)gPad->GetPrimitive("hpnsdataC2petimeslice1");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE_30kpluspdticks>>hpnsdataC2petimeslice2(50, 100, 2100)");
  hpnsdataC2petimeslice2 = (TH1*)gPad->GetPrimitive("hpnsdataC2petimeslice2");

  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time>>hpnsdataC2time(112, 0, 70000)"); // note this is with PE cut 100-2100
  hpnsdataC2time = (TH1*)gPad->GetPrimitive("hpnsdataC2time");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time_100_1200PE>>hpnsdataC2timelowPE(112, 0, 70000)");
  hpnsdataC2timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC2timelowPE");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_Time_above_1200PE>>hpnsdataC2timehighPE(112, 0, 70000)");
  hpnsdataC2timehighPE = (TH1*)gPad->GetPrimitive("hpnsdataC2timehighPE");

  TTree *myMatchedC1Peaks_pns = (TTree*)file0->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE>>hpnsdataC1(50, 100, 2100)");
  hpnsdataC1 = (TH1*)gPad->GetPrimitive("hpnsdataC1");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE_0_10kpdticks>>hpnsdataC1petimeslice0(50, 100, 2100)");
  hpnsdataC1petimeslice0 = (TH1*)gPad->GetPrimitive("hpnsdataC1petimeslice0");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE_10k_30kpdticks>>hpnsdataC1petimeslice1(50, 100, 2100)");
  hpnsdataC1petimeslice1 = (TH1*)gPad->GetPrimitive("hpnsdataC1petimeslice1");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE_30kpluspdticks>>hpnsdataC1petimeslice2(50, 100, 2100)");
  hpnsdataC1petimeslice2 = (TH1*)gPad->GetPrimitive("hpnsdataC1petimeslice2");

  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time>>hpnsdataC1time(112, 0, 70000)"); // note this is with PE cut 100-2100
  hpnsdataC1time = (TH1*)gPad->GetPrimitive("hpnsdataC1time");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time_100_1200PE>>hpnsdataC1timelowPE(112, 0, 70000)");
  hpnsdataC1timelowPE = (TH1*)gPad->GetPrimitive("hpnsdataC1timelowPE");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_Time_above_1200PE>>hpnsdataC1timehighPE(112, 0, 70000)");
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
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices.root");  // Feb 13, 2026 JINST review comments
  TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_min50PEprescale_min100PEpostscale_max2100PE_JINSTaddtimingslices_DAQtimecut.root");

  TTree *myMatchedC4Peaks_cosmic = (TTree*)file1->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE>>hcosmicdataC4(50, 100, 2100)");
  hcosmicdataC4 = (TH1*)gPad->GetPrimitive("hcosmicdataC4");
  hcosmicdataC4->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE_0_10kpdticks>>hcosmicdataC4petimeslice0(50, 100, 2100)");
  hcosmicdataC4petimeslice0 = (TH1*)gPad->GetPrimitive("hcosmicdataC4petimeslice0");
  hcosmicdataC4petimeslice0->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE_10k_30kpdticks>>hcosmicdataC4petimeslice1(50, 100, 2100)");
  hcosmicdataC4petimeslice1 = (TH1*)gPad->GetPrimitive("hcosmicdataC4petimeslice1");
  hcosmicdataC4petimeslice1->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE_30kpluspdticks>>hcosmicdataC4petimeslice2(50, 100, 2100)");
  hcosmicdataC4petimeslice2 = (TH1*)gPad->GetPrimitive("hcosmicdataC4petimeslice2");
  hcosmicdataC4petimeslice2->Scale(cosmicDatasf);

  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time>>hcosmicdataC4time(112, 0, 70000)");
  hcosmicdataC4time = (TH1*)gPad->GetPrimitive("hcosmicdataC4time");
  hcosmicdataC4time->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time_100_1200PE>>hcosmicdataC4timelowPE(112, 0, 70000)");
  hcosmicdataC4timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC4timelowPE");
  hcosmicdataC4timelowPE->Scale(cosmicDatasf);
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_Time_above_1200PE>>hcosmicdataC4timehighPE(112, 0, 70000)");
  hcosmicdataC4timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC4timehighPE");
  hcosmicdataC4timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC3Peaks_cosmic = (TTree*)file1->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE>>hcosmicdataC3(50, 100, 2100)");
  hcosmicdataC3 = (TH1*)gPad->GetPrimitive("hcosmicdataC3");
  hcosmicdataC3->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE_0_10kpdticks>>hcosmicdataC3petimeslice0(50, 100, 2100)");
  hcosmicdataC3petimeslice0 = (TH1*)gPad->GetPrimitive("hcosmicdataC3petimeslice0");
  hcosmicdataC3petimeslice0->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE_10k_30kpdticks>>hcosmicdataC3petimeslice1(50, 100, 2100)");
  hcosmicdataC3petimeslice1 = (TH1*)gPad->GetPrimitive("hcosmicdataC3petimeslice1");
  hcosmicdataC3petimeslice1->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE_30kpluspdticks>>hcosmicdataC3petimeslice2(50, 100, 2100)");
  hcosmicdataC3petimeslice2 = (TH1*)gPad->GetPrimitive("hcosmicdataC3petimeslice2");
  hcosmicdataC3petimeslice2->Scale(cosmicDatasf);

  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time>>hcosmicdataC3time(112, 0, 70000)");
  hcosmicdataC3time = (TH1*)gPad->GetPrimitive("hcosmicdataC3time");
  hcosmicdataC3time->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time_100_1200PE>>hcosmicdataC3timelowPE(112, 0, 70000)");
  hcosmicdataC3timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC3timelowPE");
  hcosmicdataC3timelowPE->Scale(cosmicDatasf);
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_Time_above_1200PE>>hcosmicdataC3timehighPE(112, 0, 70000)");
  hcosmicdataC3timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC3timehighPE");
  hcosmicdataC3timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC2Peaks_cosmic = (TTree*)file1->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE>>hcosmicdataC2(50, 100, 2100)");
  hcosmicdataC2 = (TH1*)gPad->GetPrimitive("hcosmicdataC2");
  hcosmicdataC2->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE_0_10kpdticks>>hcosmicdataC2petimeslice0(50, 100, 2100)");
  hcosmicdataC2petimeslice0 = (TH1*)gPad->GetPrimitive("hcosmicdataC2petimeslice0");
  hcosmicdataC2petimeslice0->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE_10k_30kpdticks>>hcosmicdataC2petimeslice1(50, 100, 2100)");
  hcosmicdataC2petimeslice1 = (TH1*)gPad->GetPrimitive("hcosmicdataC2petimeslice1");
  hcosmicdataC2petimeslice1->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE_30kpluspdticks>>hcosmicdataC2petimeslice2(50, 100, 2100)");
  hcosmicdataC2petimeslice2 = (TH1*)gPad->GetPrimitive("hcosmicdataC2petimeslice2");
  hcosmicdataC2petimeslice2->Scale(cosmicDatasf);

  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time>>hcosmicdataC2time(112, 0, 70000)");
  hcosmicdataC2time = (TH1*)gPad->GetPrimitive("hcosmicdataC2time");
  hcosmicdataC2time->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time_100_1200PE>>hcosmicdataC2timelowPE(112, 0, 70000)");
  hcosmicdataC2timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC2timelowPE");
  hcosmicdataC2timelowPE->Scale(cosmicDatasf);
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_Time_above_1200PE>>hcosmicdataC2timehighPE(112, 0, 70000)");
  hcosmicdataC2timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC2timehighPE");
  hcosmicdataC2timehighPE->Scale(cosmicDatasf);

  TTree *myMatchedC1Peaks_cosmic = (TTree*)file1->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE>>hcosmicdataC1(50, 100, 2100)");
  hcosmicdataC1 = (TH1*)gPad->GetPrimitive("hcosmicdataC1");
  hcosmicdataC1->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE_0_10kpdticks>>hcosmicdataC1petimeslice0(50, 100, 2100)");
  hcosmicdataC1petimeslice0 = (TH1*)gPad->GetPrimitive("hcosmicdataC1petimeslice0");
  hcosmicdataC1petimeslice0->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE_10k_30kpdticks>>hcosmicdataC1petimeslice1(50, 100, 2100)");
  hcosmicdataC1petimeslice1 = (TH1*)gPad->GetPrimitive("hcosmicdataC1petimeslice1");
  hcosmicdataC1petimeslice1->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE_30kpluspdticks>>hcosmicdataC1petimeslice2(50, 100, 2100)");
  hcosmicdataC1petimeslice2 = (TH1*)gPad->GetPrimitive("hcosmicdataC1petimeslice2");
  hcosmicdataC1petimeslice2->Scale(cosmicDatasf);

  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time>>hcosmicdataC1time(112, 0, 70000)");
  hcosmicdataC1time = (TH1*)gPad->GetPrimitive("hcosmicdataC1time");
  hcosmicdataC1time->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time_100_1200PE>>hcosmicdataC1timelowPE(112, 0, 70000)");
  hcosmicdataC1timelowPE = (TH1*)gPad->GetPrimitive("hcosmicdataC1timelowPE");
  hcosmicdataC1timelowPE->Scale(cosmicDatasf);
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_Time_above_1200PE>>hcosmicdataC1timehighPE(112, 0, 70000)");
  hcosmicdataC1timehighPE = (TH1*)gPad->GetPrimitive("hcosmicdataC1timehighPE");
  hcosmicdataC1timehighPE->Scale(cosmicDatasf);


  // Fluka simulated inactive region bkg
  // 20 million sim neutrons/events
  // Recorded events correspond to about 1.5% of generated neutrons.
  // closest to PNS is 2
  // furthest to PNS is 1
  TH1 *hsimsignalC4;
  TH1F *hinactivebkgC4;
  TH1F *hinelasticbkgC4;
  TH1 *hsimsignalC4_pe_tslice0;
  TH1F *hinactivebkgC4_pe_tslice0;
  TH1F *hinelasticbkgC4_pe_tslice0;
  TH1 *hsimsignalC4_pe_tslice1;
  TH1F *hinactivebkgC4_pe_tslice1;
  TH1F *hinelasticbkgC4_pe_tslice1;
  TH1 *hsimsignalC4_pe_tslice2;
  TH1F *hinactivebkgC4_pe_tslice2;
  TH1F *hinelasticbkgC4_pe_tslice2;
  TH1F *hsimsignalC4time;
  TH1F *hinactivebkgC4time;
  TH1F *hinelasticbkgC4time;
  TH1 *hsimsignalC4_offset280us;
  TH1F *hinactivebkgC4_offset280us;
  TH1F *hinelasticbkgC4_offset280us;
  TH1 *hsimsignalC4_pe_tslice0_offset280us;
  TH1F *hinactivebkgC4_pe_tslice0_offset280us;
  TH1F *hinelasticbkgC4_pe_tslice0_offset280us;
  TH1 *hsimsignalC4_pe_tslice1_offset280us;
  TH1F *hinactivebkgC4_pe_tslice1_offset280us;
  TH1F *hinelasticbkgC4_pe_tslice1_offset280us;
  TH1 *hsimsignalC4_pe_tslice2_offset280us;
  TH1F *hinactivebkgC4_pe_tslice2_offset280us;
  TH1F *hinelasticbkgC4_pe_tslice2_offset280us;
  TH1F *hsimsignalC4time_offset280us;
  TH1F *hinactivebkgC4time_offset280us;
  TH1F *hinelasticbkgC4time_offset280us;
  TH1 *hsimsignalC4_offset200us;
  TH1F *hinactivebkgC4_offset200us;
  TH1F *hinelasticbkgC4_offset200us;
  TH1 *hsimsignalC4_pe_tslice0_offset200us;
  TH1F *hinactivebkgC4_pe_tslice0_offset200us;
  TH1F *hinelasticbkgC4_pe_tslice0_offset200us;
  TH1 *hsimsignalC4_pe_tslice1_offset200us;
  TH1F *hinactivebkgC4_pe_tslice1_offset200us;
  TH1F *hinelasticbkgC4_pe_tslice1_offset200us;
  TH1 *hsimsignalC4_pe_tslice2_offset200us;
  TH1F *hinactivebkgC4_pe_tslice2_offset200us;
  TH1F *hinelasticbkgC4_pe_tslice2_offset200us;
  TH1F *hsimsignalC4time_offset200us;
  TH1F *hinactivebkgC4time_offset200us;
  TH1F *hinelasticbkgC4time_offset200us;

  TH1 *hsimsignalC3;
  TH1F *hinactivebkgC3;
  TH1F *hinelasticbkgC3;
  TH1 *hsimsignalC3_pe_tslice0;
  TH1F *hinactivebkgC3_pe_tslice0;
  TH1F *hinelasticbkgC3_pe_tslice0;
  TH1 *hsimsignalC3_pe_tslice1;
  TH1F *hinactivebkgC3_pe_tslice1;
  TH1F *hinelasticbkgC3_pe_tslice1;
  TH1 *hsimsignalC3_pe_tslice2;
  TH1F *hinactivebkgC3_pe_tslice2;
  TH1F *hinelasticbkgC3_pe_tslice2;
  TH1F *hsimsignalC3time;
  TH1F *hinactivebkgC3time;
  TH1F *hinelasticbkgC3time;
  TH1 *hsimsignalC3_offset280us;
  TH1F *hinactivebkgC3_offset280us;
  TH1F *hinelasticbkgC3_offset280us;
  TH1 *hsimsignalC3_pe_tslice0_offset280us;
  TH1F *hinactivebkgC3_pe_tslice0_offset280us;
  TH1F *hinelasticbkgC3_pe_tslice0_offset280us;
  TH1 *hsimsignalC3_pe_tslice1_offset280us;
  TH1F *hinactivebkgC3_pe_tslice1_offset280us;
  TH1F *hinelasticbkgC3_pe_tslice1_offset280us;
  TH1 *hsimsignalC3_pe_tslice2_offset280us;
  TH1F *hinactivebkgC3_pe_tslice2_offset280us;
  TH1F *hinelasticbkgC3_pe_tslice2_offset280us;
  TH1F *hsimsignalC3time_offset280us;
  TH1F *hinactivebkgC3time_offset280us;
  TH1F *hinelasticbkgC3time_offset280us;
  TH1 *hsimsignalC3_offset200us;
  TH1F *hinactivebkgC3_offset200us;
  TH1F *hinelasticbkgC3_offset200us;
  TH1 *hsimsignalC3_pe_tslice0_offset200us;
  TH1F *hinactivebkgC3_pe_tslice0_offset200us;
  TH1F *hinelasticbkgC3_pe_tslice0_offset200us;
  TH1 *hsimsignalC3_pe_tslice1_offset200us;
  TH1F *hinactivebkgC3_pe_tslice1_offset200us;
  TH1F *hinelasticbkgC3_pe_tslice1_offset200us;
  TH1 *hsimsignalC3_pe_tslice2_offset200us;
  TH1F *hinactivebkgC3_pe_tslice2_offset200us;
  TH1F *hinelasticbkgC3_pe_tslice2_offset200us;
  TH1F *hsimsignalC3time_offset200us;
  TH1F *hinactivebkgC3time_offset200us;
  TH1F *hinelasticbkgC3time_offset200us;

  TH1 *hsimsignalC2;
  TH1F *hinactivebkgC2;
  TH1F *hinelasticbkgC2;
  TH1 *hsimsignalC2_pe_tslice0;
  TH1F *hinactivebkgC2_pe_tslice0;
  TH1F *hinelasticbkgC2_pe_tslice0;
  TH1 *hsimsignalC2_pe_tslice1;
  TH1F *hinactivebkgC2_pe_tslice1;
  TH1F *hinelasticbkgC2_pe_tslice1;
  TH1 *hsimsignalC2_pe_tslice2;
  TH1F *hinactivebkgC2_pe_tslice2;
  TH1F *hinelasticbkgC2_pe_tslice2;
  TH1F *hsimsignalC2time;
  TH1F *hinactivebkgC2time;
  TH1F *hinelasticbkgC2time;
  TH1 *hsimsignalC2_offset280us;
  TH1F *hinactivebkgC2_offset280us;
  TH1F *hinelasticbkgC2_offset280us;
  TH1 *hsimsignalC2_pe_tslice0_offset280us;
  TH1F *hinactivebkgC2_pe_tslice0_offset280us;
  TH1F *hinelasticbkgC2_pe_tslice0_offset280us;
  TH1 *hsimsignalC2_pe_tslice1_offset280us;
  TH1F *hinactivebkgC2_pe_tslice1_offset280us;
  TH1F *hinelasticbkgC2_pe_tslice1_offset280us;
  TH1 *hsimsignalC2_pe_tslice2_offset280us;
  TH1F *hinactivebkgC2_pe_tslice2_offset280us;
  TH1F *hinelasticbkgC2_pe_tslice2_offset280us;
  TH1F *hsimsignalC2time_offset280us;
  TH1F *hinactivebkgC2time_offset280us;
  TH1F *hinelasticbkgC2time_offset280us;
  TH1 *hsimsignalC2_offset200us;
  TH1F *hinactivebkgC2_offset200us;
  TH1F *hinelasticbkgC2_offset200us;
  TH1 *hsimsignalC2_pe_tslice0_offset200us;
  TH1F *hinactivebkgC2_pe_tslice0_offset200us;
  TH1F *hinelasticbkgC2_pe_tslice0_offset200us;
  TH1 *hsimsignalC2_pe_tslice1_offset200us;
  TH1F *hinactivebkgC2_pe_tslice1_offset200us;
  TH1F *hinelasticbkgC2_pe_tslice1_offset200us;
  TH1 *hsimsignalC2_pe_tslice2_offset200us;
  TH1F *hinactivebkgC2_pe_tslice2_offset200us;
  TH1F *hinelasticbkgC2_pe_tslice2_offset200us;
  TH1F *hsimsignalC2time_offset200us;
  TH1F *hinactivebkgC2time_offset200us;
  TH1F *hinelasticbkgC2time_offset200us;

  TH1 *hsimsignalC1;
  TH1F *hinactivebkgC1;
  TH1F *hinelasticbkgC1;
  TH1 *hsimsignalC1_pe_tslice0;
  TH1F *hinactivebkgC1_pe_tslice0;
  TH1F *hinelasticbkgC1_pe_tslice0;
  TH1 *hsimsignalC1_pe_tslice1;
  TH1F *hinactivebkgC1_pe_tslice1;
  TH1F *hinelasticbkgC1_pe_tslice1;
  TH1 *hsimsignalC1_pe_tslice2;
  TH1F *hinactivebkgC1_pe_tslice2;
  TH1F *hinelasticbkgC1_pe_tslice2;
  TH1F *hsimsignalC1time;
  TH1F *hinactivebkgC1time;
  TH1F *hinelasticbkgC1time;
  TH1 *hsimsignalC1_offset280us;
  TH1F *hinactivebkgC1_offset280us;
  TH1F *hinelasticbkgC1_offset280us;
  TH1 *hsimsignalC1_pe_tslice0_offset280us;
  TH1F *hinactivebkgC1_pe_tslice0_offset280us;
  TH1F *hinelasticbkgC1_pe_tslice0_offset280us;
  TH1 *hsimsignalC1_pe_tslice1_offset280us;
  TH1F *hinactivebkgC1_pe_tslice1_offset280us;
  TH1F *hinelasticbkgC1_pe_tslice1_offset280us;
  TH1 *hsimsignalC1_pe_tslice2_offset280us;
  TH1F *hinactivebkgC1_pe_tslice2_offset280us;
  TH1F *hinelasticbkgC1_pe_tslice2_offset280us;
  TH1F *hsimsignalC1time_offset280us;
  TH1F *hinactivebkgC1time_offset280us;
  TH1F *hinelasticbkgC1time_offset280us;
  TH1 *hsimsignalC1_offset200us;
  TH1F *hinactivebkgC1_offset200us;
  TH1F *hinelasticbkgC1_offset200us;
  TH1 *hsimsignalC1_pe_tslice0_offset200us;
  TH1F *hinactivebkgC1_pe_tslice0_offset200us;
  TH1F *hinelasticbkgC1_pe_tslice0_offset200us;
  TH1 *hsimsignalC1_pe_tslice1_offset200us;
  TH1F *hinactivebkgC1_pe_tslice1_offset200us;
  TH1F *hinelasticbkgC1_pe_tslice1_offset200us;
  TH1 *hsimsignalC1_pe_tslice2_offset200us;
  TH1F *hinactivebkgC1_pe_tslice2_offset200us;
  TH1F *hinelasticbkgC1_pe_tslice2_offset200us;
  TH1F *hsimsignalC1time_offset200us;
  TH1F *hinactivebkgC1time_offset200us;
  TH1F *hinelasticbkgC1time_offset200us;

  //TFile *file3 = TFile::Open("vbox_sep24_pe.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut_calibrated2peak.root");
  //TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut_calibrated2peak_Aug2025AdjustLY.root");

  // JINST review comments
  TFile *file3;
  TFile *file3_offset280us;
  TFile *file3_offset200us;
  TCanvas *c04_pe;
  TCanvas *c04_pe_tslice0;
  TCanvas *c04_pe_tslice1;
  TCanvas *c04_pe_tslice2;
  TCanvas *c04_timing;
  TCanvas *c04_pe_offset280us;
  TCanvas *c04_pe_tslice0_offset280us;
  TCanvas *c04_pe_tslice1_offset280us;
  TCanvas *c04_pe_tslice2_offset280us;
  TCanvas *c04_timing_offset280us;
  TCanvas *c04_pe_offset200us;
  TCanvas *c04_pe_tslice0_offset200us;
  TCanvas *c04_pe_tslice1_offset200us;
  TCanvas *c04_pe_tslice2_offset200us;
  TCanvas *c04_timing_offset200us;
  TCanvas *c03_pe;
  TCanvas *c03_pe_tslice0;
  TCanvas *c03_pe_tslice1;
  TCanvas *c03_pe_tslice2;
  TCanvas *c03_timing;
  TCanvas *c03_pe_offset280us;
  TCanvas *c03_pe_tslice0_offset280us;
  TCanvas *c03_pe_tslice1_offset280us;
  TCanvas *c03_pe_tslice2_offset280us;
  TCanvas *c03_timing_offset280us;
  TCanvas *c03_pe_offset200us;
  TCanvas *c03_pe_tslice0_offset200us;
  TCanvas *c03_pe_tslice1_offset200us;
  TCanvas *c03_pe_tslice2_offset200us;
  TCanvas *c03_timing_offset200us;
  TCanvas *c02_pe;
  TCanvas *c02_pe_tslice0;
  TCanvas *c02_pe_tslice1;
  TCanvas *c02_pe_tslice2;
  TCanvas *c02_timing;
  TCanvas *c02_pe_offset280us;
  TCanvas *c02_pe_tslice0_offset280us;
  TCanvas *c02_pe_tslice1_offset280us;
  TCanvas *c02_pe_tslice2_offset280us;
  TCanvas *c02_timing_offset280us;
  TCanvas *c02_pe_offset200us;
  TCanvas *c02_pe_tslice0_offset200us;
  TCanvas *c02_pe_tslice1_offset200us;
  TCanvas *c02_pe_tslice2_offset200us;
  TCanvas *c02_timing_offset200us;
  TCanvas *c01_pe;
  TCanvas *c01_pe_tslice0;
  TCanvas *c01_pe_tslice1;
  TCanvas *c01_pe_tslice2;
  TCanvas *c01_timing;
  TCanvas *c01_pe_offset280us;
  TCanvas *c01_pe_tslice0_offset280us;
  TCanvas *c01_pe_tslice1_offset280us;
  TCanvas *c01_pe_tslice2_offset280us;
  TCanvas *c01_timing_offset280us;
  TCanvas *c01_pe_offset200us;
  TCanvas *c01_pe_tslice0_offset200us;
  TCanvas *c01_pe_tslice1_offset200us;
  TCanvas *c01_pe_tslice2_offset200us;
  TCanvas *c01_timing_offset200us;

  if (run_number == 25036 || run_number == 25071) { // for all runs, it's tricky if timing is involved beacuse of different beam timing offset in runs
    //file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset280us_PEcut4timing.root"); // time offset -280us
    file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset280us_PEcut4timing_DAQtcut4PE.root"); // time offset -280us
  }
  if (run_number == 25068) {
    //file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset200us_PEcut4timing.root"); // time offset -200us
    file3 = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset200us_PEcut4timing_DAQtcut4PE.root"); // time offset -200us
  }
  if (run_number == 1) {
    // here we need both sim offset files due to two different time offset in three runs
    file3_offset280us = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset280us_PEcut4timing_DAQtcut4PE.root"); // time offset -280us
    file3_offset200us = TFile::Open("vbox_sep24_sim_JINSTreview_timeoffset200us_PEcut4timing_DAQtcut4PE.root"); // time offset -200us
  }


  if (run_number == 25036 || run_number == 25071 || run_number == 25068) {
    // process with one file
    c04_pe = (TCanvas*)file3->Get("c04_pe");
    hinactivebkgC4 = (TH1F*)c04_pe->GetPrimitive("petotothC4"); // inactive material
    hinactivebkgC4->Scale(FlukaMCsf);
    hinelasticbkgC4 = (TH1F*)c04_pe->GetPrimitive("petotineC4"); // inelastic
    hinelasticbkgC4->Scale(FlukaMCsf);
    // Fluka sim PNS signal
    // same fluka scale factor
    hsimsignalC4 = (TH1F*)c04_pe->GetPrimitive("petotcapC4");
    hsimsignalC4->Scale(FlukaMCsf);
    // time slice 0
    c04_pe_tslice0 = (TCanvas*)file3->Get("c04_pe_tslice0");
    hinactivebkgC4_pe_tslice0 = (TH1F*)c04_pe_tslice0->GetPrimitive("petotothC4_t0_160us");
    hinactivebkgC4_pe_tslice0->Scale(FlukaMCsf);
    hinelasticbkgC4_pe_tslice0 = (TH1F*)c04_pe_tslice0->GetPrimitive("petotineC4_t0_160us");
    hinelasticbkgC4_pe_tslice0->Scale(FlukaMCsf);
    hsimsignalC4_pe_tslice0 = (TH1F*)c04_pe_tslice0->GetPrimitive("petotcapC4_t0_160us");
    hsimsignalC4_pe_tslice0->Scale(FlukaMCsf);
    // time slice 1
    c04_pe_tslice1 = (TCanvas*)file3->Get("c04_pe_tslice1");
    hinactivebkgC4_pe_tslice1 = (TH1F*)c04_pe_tslice1->GetPrimitive("petotothC4_t160_480us");
    hinactivebkgC4_pe_tslice1->Scale(FlukaMCsf);
    hinelasticbkgC4_pe_tslice1 = (TH1F*)c04_pe_tslice1->GetPrimitive("petotineC4_t160_480us");
    hinelasticbkgC4_pe_tslice1->Scale(FlukaMCsf);
    hsimsignalC4_pe_tslice1 = (TH1F*)c04_pe_tslice1->GetPrimitive("petotcapC4_t160_480us");
    hsimsignalC4_pe_tslice1->Scale(FlukaMCsf);
    // time slice 2
    c04_pe_tslice2 = (TCanvas*)file3->Get("c04_pe_tslice2");
    hinactivebkgC4_pe_tslice2 = (TH1F*)c04_pe_tslice2->GetPrimitive("petotothC4_t480_1050us");
    hinactivebkgC4_pe_tslice2->Scale(FlukaMCsf);
    hinelasticbkgC4_pe_tslice2 = (TH1F*)c04_pe_tslice2->GetPrimitive("petotineC4_t480_1050us");
    hinelasticbkgC4_pe_tslice2->Scale(FlukaMCsf);
    hsimsignalC4_pe_tslice2 = (TH1F*)c04_pe_tslice2->GetPrimitive("petotcapC4_t480_1050us");
    hsimsignalC4_pe_tslice2->Scale(FlukaMCsf);
    // timing
    c04_timing = (TCanvas*)file3->Get("c04_timing");
    hsimsignalC4time = (TH1F*)c04_timing->GetPrimitive("offlinetimecapC4");
    hsimsignalC4time->Scale(FlukaMCsf);
    hinactivebkgC4time = (TH1F*)c04_timing->GetPrimitive("offlinetimeothC4");
    hinactivebkgC4time->Scale(FlukaMCsf);
    hinelasticbkgC4time = (TH1F*)c04_timing->GetPrimitive("offlinetimeineC4");
    hinelasticbkgC4time->Scale(FlukaMCsf);

    c03_pe = (TCanvas*)file3->Get("c03_pe");
    hinactivebkgC3 = (TH1F*)c03_pe->GetPrimitive("petotothC3");
    hinactivebkgC3->Scale(FlukaMCsf);
    hinelasticbkgC3 = (TH1F*)c03_pe->GetPrimitive("petotineC3");
    hinelasticbkgC3->Scale(FlukaMCsf);
    hsimsignalC3 = (TH1F*)c03_pe->GetPrimitive("petotcapC3");
    hsimsignalC3->Scale(FlukaMCsf);
    // time slice 0
    c03_pe_tslice0 = (TCanvas*)file3->Get("c03_pe_tslice0");
    hinactivebkgC3_pe_tslice0 = (TH1F*)c03_pe_tslice0->GetPrimitive("petotothC3_t0_160us");
    hinactivebkgC3_pe_tslice0->Scale(FlukaMCsf);
    hinelasticbkgC3_pe_tslice0 = (TH1F*)c03_pe_tslice0->GetPrimitive("petotineC3_t0_160us");
    hinelasticbkgC3_pe_tslice0->Scale(FlukaMCsf);
    hsimsignalC3_pe_tslice0 = (TH1F*)c03_pe_tslice0->GetPrimitive("petotcapC3_t0_160us");
    hsimsignalC3_pe_tslice0->Scale(FlukaMCsf);
    // time slice 1
    c03_pe_tslice1 = (TCanvas*)file3->Get("c03_pe_tslice1");
    hinactivebkgC3_pe_tslice1 = (TH1F*)c03_pe_tslice1->GetPrimitive("petotothC3_t160_480us");
    hinactivebkgC3_pe_tslice1->Scale(FlukaMCsf);
    hinelasticbkgC3_pe_tslice1 = (TH1F*)c03_pe_tslice1->GetPrimitive("petotineC3_t160_480us");
    hinelasticbkgC3_pe_tslice1->Scale(FlukaMCsf);
    hsimsignalC3_pe_tslice1 = (TH1F*)c03_pe_tslice1->GetPrimitive("petotcapC3_t160_480us");
    hsimsignalC3_pe_tslice1->Scale(FlukaMCsf);
    // time slice 2
    c03_pe_tslice2 = (TCanvas*)file3->Get("c03_pe_tslice2");
    hinactivebkgC3_pe_tslice2 = (TH1F*)c03_pe_tslice2->GetPrimitive("petotothC3_t480_1050us");
    hinactivebkgC3_pe_tslice2->Scale(FlukaMCsf);
    hinelasticbkgC3_pe_tslice2 = (TH1F*)c03_pe_tslice2->GetPrimitive("petotineC3_t480_1050us");
    hinelasticbkgC3_pe_tslice2->Scale(FlukaMCsf);
    hsimsignalC3_pe_tslice2 = (TH1F*)c03_pe_tslice2->GetPrimitive("petotcapC3_t480_1050us");
    hsimsignalC3_pe_tslice2->Scale(FlukaMCsf);
    // timing
    c03_timing = (TCanvas*)file3->Get("c03_timing");
    hsimsignalC3time = (TH1F*)c03_timing->GetPrimitive("offlinetimecapC3");
    hsimsignalC3time->Scale(FlukaMCsf);
    hinactivebkgC3time = (TH1F*)c03_timing->GetPrimitive("offlinetimeothC3");
    hinactivebkgC3time->Scale(FlukaMCsf);
    hinelasticbkgC3time = (TH1F*)c03_timing->GetPrimitive("offlinetimeineC3");
    hinelasticbkgC3time->Scale(FlukaMCsf);

    c02_pe = (TCanvas*)file3->Get("c02_pe");
    hinactivebkgC2 = (TH1F*)c02_pe->GetPrimitive("petotothC2");
    hinactivebkgC2->Scale(FlukaMCsf);
    hinelasticbkgC2 = (TH1F*)c02_pe->GetPrimitive("petotineC2");
    hinelasticbkgC2->Scale(FlukaMCsf);
    hsimsignalC2 = (TH1F*)c02_pe->GetPrimitive("petotcapC2");
    hsimsignalC2->Scale(FlukaMCsf);
    // time slice 0
    c02_pe_tslice0 = (TCanvas*)file3->Get("c02_pe_tslice0");
    hinactivebkgC2_pe_tslice0 = (TH1F*)c02_pe_tslice0->GetPrimitive("petotothC2_t0_160us");
    hinactivebkgC2_pe_tslice0->Scale(FlukaMCsf);
    hinelasticbkgC2_pe_tslice0 = (TH1F*)c02_pe_tslice0->GetPrimitive("petotineC2_t0_160us");
    hinelasticbkgC2_pe_tslice0->Scale(FlukaMCsf);
    hsimsignalC2_pe_tslice0 = (TH1F*)c02_pe_tslice0->GetPrimitive("petotcapC2_t0_160us");
    hsimsignalC2_pe_tslice0->Scale(FlukaMCsf);
    // time slice 1
    c02_pe_tslice1 = (TCanvas*)file3->Get("c02_pe_tslice1");
    hinactivebkgC2_pe_tslice1 = (TH1F*)c02_pe_tslice1->GetPrimitive("petotothC2_t160_480us");
    hinactivebkgC2_pe_tslice1->Scale(FlukaMCsf);
    hinelasticbkgC2_pe_tslice1 = (TH1F*)c02_pe_tslice1->GetPrimitive("petotineC2_t160_480us");
    hinelasticbkgC2_pe_tslice1->Scale(FlukaMCsf);
    hsimsignalC2_pe_tslice1 = (TH1F*)c02_pe_tslice1->GetPrimitive("petotcapC2_t160_480us");
    hsimsignalC2_pe_tslice1->Scale(FlukaMCsf);
    // time slice 2
    c02_pe_tslice2 = (TCanvas*)file3->Get("c02_pe_tslice2");
    hinactivebkgC2_pe_tslice2 = (TH1F*)c02_pe_tslice2->GetPrimitive("petotothC2_t480_1050us");
    hinactivebkgC2_pe_tslice2->Scale(FlukaMCsf);
    hinelasticbkgC2_pe_tslice2 = (TH1F*)c02_pe_tslice2->GetPrimitive("petotineC2_t480_1050us");
    hinelasticbkgC2_pe_tslice2->Scale(FlukaMCsf);
    hsimsignalC2_pe_tslice2 = (TH1F*)c02_pe_tslice2->GetPrimitive("petotcapC2_t480_1050us");
    hsimsignalC2_pe_tslice2->Scale(FlukaMCsf);
    // timing
    c02_timing = (TCanvas*)file3->Get("c02_timing");
    hsimsignalC2time = (TH1F*)c02_timing->GetPrimitive("offlinetimecapC2");
    hsimsignalC2time->Scale(FlukaMCsf);
    hinactivebkgC2time = (TH1F*)c02_timing->GetPrimitive("offlinetimeothC2");
    hinactivebkgC2time->Scale(FlukaMCsf);
    hinelasticbkgC2time = (TH1F*)c02_timing->GetPrimitive("offlinetimeineC2");
    hinelasticbkgC2time->Scale(FlukaMCsf);

    c01_pe = (TCanvas*)file3->Get("c01_pe");
    hinactivebkgC1 = (TH1F*)c01_pe->GetPrimitive("petotothC1");
    hinactivebkgC1->Scale(FlukaMCsf);
    hinelasticbkgC1 = (TH1F*)c01_pe->GetPrimitive("petotineC1");
    hinelasticbkgC1->Scale(FlukaMCsf);
    hsimsignalC1 = (TH1F*)c01_pe->GetPrimitive("petotcapC1");
    hsimsignalC1->Scale(FlukaMCsf);
    // time slice 0
    c01_pe_tslice0 = (TCanvas*)file3->Get("c01_pe_tslice0");
    hinactivebkgC1_pe_tslice0 = (TH1F*)c01_pe_tslice0->GetPrimitive("petotothC1_t0_160us");
    hinactivebkgC1_pe_tslice0->Scale(FlukaMCsf);
    hinelasticbkgC1_pe_tslice0 = (TH1F*)c01_pe_tslice0->GetPrimitive("petotineC1_t0_160us");
    hinelasticbkgC1_pe_tslice0->Scale(FlukaMCsf);
    hsimsignalC1_pe_tslice0 = (TH1F*)c01_pe_tslice0->GetPrimitive("petotcapC1_t0_160us");
    hsimsignalC1_pe_tslice0->Scale(FlukaMCsf);
    // time slice 1
    c01_pe_tslice1 = (TCanvas*)file3->Get("c01_pe_tslice1");
    hinactivebkgC1_pe_tslice1 = (TH1F*)c01_pe_tslice1->GetPrimitive("petotothC1_t160_480us");
    hinactivebkgC1_pe_tslice1->Scale(FlukaMCsf);
    hinelasticbkgC1_pe_tslice1 = (TH1F*)c01_pe_tslice1->GetPrimitive("petotineC1_t160_480us");
    hinelasticbkgC1_pe_tslice1->Scale(FlukaMCsf);
    hsimsignalC1_pe_tslice1 = (TH1F*)c01_pe_tslice1->GetPrimitive("petotcapC1_t160_480us");
    hsimsignalC1_pe_tslice1->Scale(FlukaMCsf);
    // time slice 2
    c01_pe_tslice2 = (TCanvas*)file3->Get("c01_pe_tslice2");
    hinactivebkgC1_pe_tslice2 = (TH1F*)c01_pe_tslice2->GetPrimitive("petotothC1_t480_1050us");
    hinactivebkgC1_pe_tslice2->Scale(FlukaMCsf);
    hinelasticbkgC1_pe_tslice2 = (TH1F*)c01_pe_tslice2->GetPrimitive("petotineC1_t480_1050us");
    hinelasticbkgC1_pe_tslice2->Scale(FlukaMCsf);
    hsimsignalC1_pe_tslice2 = (TH1F*)c01_pe_tslice2->GetPrimitive("petotcapC1_t480_1050us");
    hsimsignalC1_pe_tslice2->Scale(FlukaMCsf);
    // timing
    c01_timing = (TCanvas*)file3->Get("c01_timing");
    hsimsignalC1time = (TH1F*)c01_timing->GetPrimitive("offlinetimecapC1");
    hsimsignalC1time->Scale(FlukaMCsf);
    hinactivebkgC1time = (TH1F*)c01_timing->GetPrimitive("offlinetimeothC1");
    hinactivebkgC1time->Scale(FlukaMCsf);
    hinelasticbkgC1time= (TH1F*)c01_timing->GetPrimitive("offlinetimeineC1");
    hinelasticbkgC1time->Scale(FlukaMCsf);
  } // end individual runs


  if (run_number == 1) {
    // proceed with two sim files,
    // scale for each file, and then add two histograms from two sim files
    // C1
    c01_pe_offset280us = (TCanvas*)file3_offset280us->Get("c01_pe");
    hinactivebkgC1_offset280us  = (TH1F*)c01_pe_offset280us->GetPrimitive("petotothC1"); hinactivebkgC1_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC1_offset280us = (TH1F*)c01_pe_offset280us->GetPrimitive("petotineC1"); hinelasticbkgC1_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC1_offset280us    = (TH1F*)c01_pe_offset280us->GetPrimitive("petotcapC1"); hsimsignalC1_offset280us->Scale(FlukaMCsfoffset280us);
    c01_pe_offset200us = (TCanvas*)file3_offset200us->Get("c01_pe");
    hinactivebkgC1_offset200us  = (TH1F*)c01_pe_offset200us->GetPrimitive("petotothC1"); hinactivebkgC1_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC1_offset200us = (TH1F*)c01_pe_offset200us->GetPrimitive("petotineC1"); hinelasticbkgC1_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC1_offset200us    = (TH1F*)c01_pe_offset200us->GetPrimitive("petotcapC1"); hsimsignalC1_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC1 = (TH1F*)hinactivebkgC1_offset280us->Clone("hinactivebkgC1"); // clone to make sure two hist has same axis binning
    hinactivebkgC1->Add(hinactivebkgC1_offset200us);
    hinelasticbkgC1 = (TH1F*)hinelasticbkgC1_offset280us->Clone("hinelasticbkgC1");
    hinelasticbkgC1->Add(hinelasticbkgC1_offset200us);
    hsimsignalC1 = (TH1F*)hsimsignalC1_offset280us->Clone("hsimsignalC1");
    hsimsignalC1->Add(hsimsignalC1_offset200us);
    // time slice 0
    c01_pe_tslice0_offset280us             = (TCanvas*)file3_offset280us->Get("c01_pe_tslice0");
    hinactivebkgC1_pe_tslice0_offset280us  = (TH1F*)c01_pe_tslice0_offset280us->GetPrimitive("petotothC1_t0_160us"); hinactivebkgC1_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC1_pe_tslice0_offset280us = (TH1F*)c01_pe_tslice0_offset280us->GetPrimitive("petotineC1_t0_160us"); hinelasticbkgC1_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC1_pe_tslice0_offset280us    = (TH1F*)c01_pe_tslice0_offset280us->GetPrimitive("petotcapC1_t0_160us"); hsimsignalC1_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    c01_pe_tslice0_offset200us             = (TCanvas*)file3_offset200us->Get("c01_pe_tslice0");
    hinactivebkgC1_pe_tslice0_offset200us  = (TH1F*)c01_pe_tslice0_offset200us->GetPrimitive("petotothC1_t0_160us"); hinactivebkgC1_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC1_pe_tslice0_offset200us = (TH1F*)c01_pe_tslice0_offset200us->GetPrimitive("petotineC1_t0_160us"); hinelasticbkgC1_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC1_pe_tslice0_offset200us    = (TH1F*)c01_pe_tslice0_offset200us->GetPrimitive("petotcapC1_t0_160us"); hsimsignalC1_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC1_pe_tslice0 = (TH1F*)hinactivebkgC1_pe_tslice0_offset280us->Clone("hinactivebkgC1_pe_tslice0");
    hinactivebkgC1_pe_tslice0->Add(hinactivebkgC1_pe_tslice0_offset200us);
    hinelasticbkgC1_pe_tslice0 = (TH1F*)hinelasticbkgC1_pe_tslice0_offset280us->Clone("hinelasticbkgC1_pe_tslice0");
    hinelasticbkgC1_pe_tslice0->Add(hinelasticbkgC1_pe_tslice0_offset200us);
    hsimsignalC1_pe_tslice0 = (TH1F*)hsimsignalC1_pe_tslice0_offset280us->Clone("hsimsignalC1_pe_tslice0");
    hsimsignalC1_pe_tslice0->Add(hsimsignalC1_pe_tslice0_offset200us);
    // time slice 1
    c01_pe_tslice1_offset280us             = (TCanvas*)file3_offset280us->Get("c01_pe_tslice1");
    hinactivebkgC1_pe_tslice1_offset280us  = (TH1F*)c01_pe_tslice1_offset280us->GetPrimitive("petotothC1_t160_480us"); hinactivebkgC1_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC1_pe_tslice1_offset280us = (TH1F*)c01_pe_tslice1_offset280us->GetPrimitive("petotineC1_t160_480us"); hinelasticbkgC1_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC1_pe_tslice1_offset280us    = (TH1F*)c01_pe_tslice1_offset280us->GetPrimitive("petotcapC1_t160_480us"); hsimsignalC1_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    c01_pe_tslice1_offset200us             = (TCanvas*)file3_offset200us->Get("c01_pe_tslice1");
    hinactivebkgC1_pe_tslice1_offset200us  = (TH1F*)c01_pe_tslice1_offset200us->GetPrimitive("petotothC1_t160_480us"); hinactivebkgC1_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC1_pe_tslice1_offset200us = (TH1F*)c01_pe_tslice1_offset200us->GetPrimitive("petotineC1_t160_480us"); hinelasticbkgC1_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC1_pe_tslice1_offset200us    = (TH1F*)c01_pe_tslice1_offset200us->GetPrimitive("petotcapC1_t160_480us"); hsimsignalC1_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC1_pe_tslice1 = (TH1F*)hinactivebkgC1_pe_tslice1_offset280us->Clone("hinactivebkgC1_pe_tslice1");
    hinactivebkgC1_pe_tslice1->Add(hinactivebkgC1_pe_tslice1_offset200us);
    hinelasticbkgC1_pe_tslice1 = (TH1F*)hinelasticbkgC1_pe_tslice1_offset280us->Clone("hinelasticbkgC1_pe_tslice1");
    hinelasticbkgC1_pe_tslice1->Add(hinelasticbkgC1_pe_tslice1_offset200us);
    hsimsignalC1_pe_tslice1 = (TH1F*)hsimsignalC1_pe_tslice1_offset280us->Clone("hsimsignalC1_pe_tslice1");
    hsimsignalC1_pe_tslice1->Add(hsimsignalC1_pe_tslice1_offset200us);
    // time slice 2
    c01_pe_tslice2_offset280us             = (TCanvas*)file3_offset280us->Get("c01_pe_tslice2");
    hinactivebkgC1_pe_tslice2_offset280us  = (TH1F*)c01_pe_tslice2_offset280us->GetPrimitive("petotothC1_t480_1050us"); hinactivebkgC1_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC1_pe_tslice2_offset280us = (TH1F*)c01_pe_tslice2_offset280us->GetPrimitive("petotineC1_t480_1050us"); hinelasticbkgC1_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC1_pe_tslice2_offset280us    = (TH1F*)c01_pe_tslice2_offset280us->GetPrimitive("petotcapC1_t480_1050us"); hsimsignalC1_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    c01_pe_tslice2_offset200us             = (TCanvas*)file3_offset200us->Get("c01_pe_tslice2");
    hinactivebkgC1_pe_tslice2_offset200us  = (TH1F*)c01_pe_tslice2_offset200us->GetPrimitive("petotothC1_t480_1050us"); hinactivebkgC1_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC1_pe_tslice2_offset200us = (TH1F*)c01_pe_tslice2_offset200us->GetPrimitive("petotineC1_t480_1050us"); hinelasticbkgC1_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC1_pe_tslice2_offset200us    = (TH1F*)c01_pe_tslice2_offset200us->GetPrimitive("petotcapC1_t480_1050us"); hsimsignalC1_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC1_pe_tslice2 = (TH1F*)hinactivebkgC1_pe_tslice2_offset280us->Clone("hinactivebkgC1_pe_tslice2");
    hinactivebkgC1_pe_tslice2->Add(hinactivebkgC1_pe_tslice2_offset200us);
    hinelasticbkgC1_pe_tslice2 = (TH1F*)hinelasticbkgC1_pe_tslice2_offset280us->Clone("hinelasticbkgC1_pe_tslice2");
    hinelasticbkgC1_pe_tslice2->Add(hinelasticbkgC1_pe_tslice2_offset200us);
    hsimsignalC1_pe_tslice2 = (TH1F*)hsimsignalC1_pe_tslice2_offset280us->Clone("hsimsignalC1_pe_tslice2");
    hsimsignalC1_pe_tslice2->Add(hsimsignalC1_pe_tslice2_offset200us);
    // timing
    c01_timing_offset280us = (TCanvas*)file3_offset280us->Get("c01_timing");
    hsimsignalC1time_offset280us = (TH1F*)c01_timing_offset280us->GetPrimitive("offlinetimecapC1");   hsimsignalC1time_offset280us->Scale(FlukaMCsfoffset280us);
    hinactivebkgC1time_offset280us = (TH1F*)c01_timing_offset280us->GetPrimitive("offlinetimeothC1"); hinactivebkgC1time_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC1time_offset280us = (TH1F*)c01_timing_offset280us->GetPrimitive("offlinetimeineC1"); hinelasticbkgC1time_offset280us->Scale(FlukaMCsfoffset280us);
    c01_timing_offset200us = (TCanvas*)file3_offset200us->Get("c01_timing");
    hsimsignalC1time_offset200us = (TH1F*)c01_timing_offset200us->GetPrimitive("offlinetimecapC1");   hsimsignalC1time_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC1time_offset200us = (TH1F*)c01_timing_offset200us->GetPrimitive("offlinetimeothC1"); hinactivebkgC1time_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC1time_offset200us = (TH1F*)c01_timing_offset200us->GetPrimitive("offlinetimeineC1"); hinelasticbkgC1time_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC1time = (TH1F*)hsimsignalC1time_offset280us->Clone("hsimsignalC1time");
    hsimsignalC1time->Add(hsimsignalC1time_offset200us);
    hinactivebkgC1time = (TH1F*)hinactivebkgC1time_offset280us->Clone("hinactivebkgC1time");
    hinactivebkgC1time->Add(hinactivebkgC1time_offset200us);
    hinelasticbkgC1time = (TH1F*)hinelasticbkgC1time_offset280us->Clone("hinelasticbkgC1time");
    hinelasticbkgC1time->Add(hinelasticbkgC1time_offset200us);

    // C2
    c02_pe_offset280us = (TCanvas*)file3_offset280us->Get("c02_pe");
    hinactivebkgC2_offset280us  = (TH1F*)c02_pe_offset280us->GetPrimitive("petotothC2"); hinactivebkgC2_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC2_offset280us = (TH1F*)c02_pe_offset280us->GetPrimitive("petotineC2"); hinelasticbkgC2_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC2_offset280us    = (TH1F*)c02_pe_offset280us->GetPrimitive("petotcapC2"); hsimsignalC2_offset280us->Scale(FlukaMCsfoffset280us);
    c02_pe_offset200us = (TCanvas*)file3_offset200us->Get("c02_pe");
    hinactivebkgC2_offset200us  = (TH1F*)c02_pe_offset200us->GetPrimitive("petotothC2"); hinactivebkgC2_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC2_offset200us = (TH1F*)c02_pe_offset200us->GetPrimitive("petotineC2"); hinelasticbkgC2_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC2_offset200us    = (TH1F*)c02_pe_offset200us->GetPrimitive("petotcapC2"); hsimsignalC2_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC2 = (TH1F*)hinactivebkgC2_offset280us->Clone("hinactivebkgC2"); // clone to make sure two hist has same axis binning
    hinactivebkgC2->Add(hinactivebkgC2_offset200us);
    hinelasticbkgC2 = (TH1F*)hinelasticbkgC2_offset280us->Clone("hinelasticbkgC2");
    hinelasticbkgC2->Add(hinelasticbkgC2_offset200us);
    hsimsignalC2 = (TH1F*)hsimsignalC2_offset280us->Clone("hsimsignalC2");
    hsimsignalC2->Add(hsimsignalC2_offset200us);
    // time slice 0
    c02_pe_tslice0_offset280us             = (TCanvas*)file3_offset280us->Get("c02_pe_tslice0");
    hinactivebkgC2_pe_tslice0_offset280us  = (TH1F*)c02_pe_tslice0_offset280us->GetPrimitive("petotothC2_t0_160us"); hinactivebkgC2_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC2_pe_tslice0_offset280us = (TH1F*)c02_pe_tslice0_offset280us->GetPrimitive("petotineC2_t0_160us"); hinelasticbkgC2_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC2_pe_tslice0_offset280us    = (TH1F*)c02_pe_tslice0_offset280us->GetPrimitive("petotcapC2_t0_160us"); hsimsignalC2_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    c02_pe_tslice0_offset200us             = (TCanvas*)file3_offset200us->Get("c02_pe_tslice0");
    hinactivebkgC2_pe_tslice0_offset200us  = (TH1F*)c02_pe_tslice0_offset200us->GetPrimitive("petotothC2_t0_160us"); hinactivebkgC2_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC2_pe_tslice0_offset200us = (TH1F*)c02_pe_tslice0_offset200us->GetPrimitive("petotineC2_t0_160us"); hinelasticbkgC2_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC2_pe_tslice0_offset200us    = (TH1F*)c02_pe_tslice0_offset200us->GetPrimitive("petotcapC2_t0_160us"); hsimsignalC2_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC2_pe_tslice0 = (TH1F*)hinactivebkgC2_pe_tslice0_offset280us->Clone("hinactivebkgC2_pe_tslice0");
    hinactivebkgC2_pe_tslice0->Add(hinactivebkgC2_pe_tslice0_offset200us);
    hinelasticbkgC2_pe_tslice0 = (TH1F*)hinelasticbkgC2_pe_tslice0_offset280us->Clone("hinelasticbkgC2_pe_tslice0");
    hinelasticbkgC2_pe_tslice0->Add(hinelasticbkgC2_pe_tslice0_offset200us);
    hsimsignalC2_pe_tslice0 = (TH1F*)hsimsignalC2_pe_tslice0_offset280us->Clone("hsimsignalC2_pe_tslice0");
    hsimsignalC2_pe_tslice0->Add(hsimsignalC2_pe_tslice0_offset200us);
    // time slice 1
    c02_pe_tslice1_offset280us             = (TCanvas*)file3_offset280us->Get("c02_pe_tslice1");
    hinactivebkgC2_pe_tslice1_offset280us  = (TH1F*)c02_pe_tslice1_offset280us->GetPrimitive("petotothC2_t160_480us"); hinactivebkgC2_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC2_pe_tslice1_offset280us = (TH1F*)c02_pe_tslice1_offset280us->GetPrimitive("petotineC2_t160_480us"); hinelasticbkgC2_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC2_pe_tslice1_offset280us    = (TH1F*)c02_pe_tslice1_offset280us->GetPrimitive("petotcapC2_t160_480us"); hsimsignalC2_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    c02_pe_tslice1_offset200us             = (TCanvas*)file3_offset200us->Get("c02_pe_tslice1");
    hinactivebkgC2_pe_tslice1_offset200us  = (TH1F*)c02_pe_tslice1_offset200us->GetPrimitive("petotothC2_t160_480us"); hinactivebkgC2_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC2_pe_tslice1_offset200us = (TH1F*)c02_pe_tslice1_offset200us->GetPrimitive("petotineC2_t160_480us"); hinelasticbkgC2_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC2_pe_tslice1_offset200us    = (TH1F*)c02_pe_tslice1_offset200us->GetPrimitive("petotcapC2_t160_480us"); hsimsignalC2_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC2_pe_tslice1 = (TH1F*)hinactivebkgC2_pe_tslice1_offset280us->Clone("hinactivebkgC2_pe_tslice1");
    hinactivebkgC2_pe_tslice1->Add(hinactivebkgC2_pe_tslice1_offset200us);
    hinelasticbkgC2_pe_tslice1 = (TH1F*)hinelasticbkgC2_pe_tslice1_offset280us->Clone("hinelasticbkgC2_pe_tslice1");
    hinelasticbkgC2_pe_tslice1->Add(hinelasticbkgC2_pe_tslice1_offset200us);
    hsimsignalC2_pe_tslice1 = (TH1F*)hsimsignalC2_pe_tslice1_offset280us->Clone("hsimsignalC2_pe_tslice1");
    hsimsignalC2_pe_tslice1->Add(hsimsignalC2_pe_tslice1_offset200us);
    // time slice 2
    c02_pe_tslice2_offset280us             = (TCanvas*)file3_offset280us->Get("c02_pe_tslice2");
    hinactivebkgC2_pe_tslice2_offset280us  = (TH1F*)c02_pe_tslice2_offset280us->GetPrimitive("petotothC2_t480_1050us"); hinactivebkgC2_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC2_pe_tslice2_offset280us = (TH1F*)c02_pe_tslice2_offset280us->GetPrimitive("petotineC2_t480_1050us"); hinelasticbkgC2_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC2_pe_tslice2_offset280us    = (TH1F*)c02_pe_tslice2_offset280us->GetPrimitive("petotcapC2_t480_1050us"); hsimsignalC2_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    c02_pe_tslice2_offset200us             = (TCanvas*)file3_offset200us->Get("c02_pe_tslice2");
    hinactivebkgC2_pe_tslice2_offset200us  = (TH1F*)c02_pe_tslice2_offset200us->GetPrimitive("petotothC2_t480_1050us"); hinactivebkgC2_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC2_pe_tslice2_offset200us = (TH1F*)c02_pe_tslice2_offset200us->GetPrimitive("petotineC2_t480_1050us"); hinelasticbkgC2_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC2_pe_tslice2_offset200us    = (TH1F*)c02_pe_tslice2_offset200us->GetPrimitive("petotcapC2_t480_1050us"); hsimsignalC2_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC2_pe_tslice2 = (TH1F*)hinactivebkgC2_pe_tslice2_offset280us->Clone("hinactivebkgC2_pe_tslice2");
    hinactivebkgC2_pe_tslice2->Add(hinactivebkgC2_pe_tslice2_offset200us);
    hinelasticbkgC2_pe_tslice2 = (TH1F*)hinelasticbkgC2_pe_tslice2_offset280us->Clone("hinelasticbkgC2_pe_tslice2");
    hinelasticbkgC2_pe_tslice2->Add(hinelasticbkgC2_pe_tslice2_offset200us);
    hsimsignalC2_pe_tslice2 = (TH1F*)hsimsignalC2_pe_tslice2_offset280us->Clone("hsimsignalC2_pe_tslice2");
    hsimsignalC2_pe_tslice2->Add(hsimsignalC2_pe_tslice2_offset200us);
    // timing
    c02_timing_offset280us = (TCanvas*)file3_offset280us->Get("c02_timing");
    hsimsignalC2time_offset280us = (TH1F*)c02_timing_offset280us->GetPrimitive("offlinetimecapC2");   hsimsignalC2time_offset280us->Scale(FlukaMCsfoffset280us);
    hinactivebkgC2time_offset280us = (TH1F*)c02_timing_offset280us->GetPrimitive("offlinetimeothC2"); hinactivebkgC2time_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC2time_offset280us = (TH1F*)c02_timing_offset280us->GetPrimitive("offlinetimeineC2"); hinelasticbkgC2time_offset280us->Scale(FlukaMCsfoffset280us);
    c02_timing_offset200us = (TCanvas*)file3_offset200us->Get("c02_timing");
    hsimsignalC2time_offset200us = (TH1F*)c02_timing_offset200us->GetPrimitive("offlinetimecapC2");   hsimsignalC2time_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC2time_offset200us = (TH1F*)c02_timing_offset200us->GetPrimitive("offlinetimeothC2"); hinactivebkgC2time_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC2time_offset200us = (TH1F*)c02_timing_offset200us->GetPrimitive("offlinetimeineC2"); hinelasticbkgC2time_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC2time = (TH1F*)hsimsignalC2time_offset280us->Clone("hsimsignalC2time");
    hsimsignalC2time->Add(hsimsignalC2time_offset200us);
    hinactivebkgC2time = (TH1F*)hinactivebkgC2time_offset280us->Clone("hinactivebkgC2time");
    hinactivebkgC2time->Add(hinactivebkgC2time_offset200us);
    hinelasticbkgC2time = (TH1F*)hinelasticbkgC2time_offset280us->Clone("hinelasticbkgC2time");
    hinelasticbkgC2time->Add(hinelasticbkgC2time_offset200us);

    // C3
    c03_pe_offset280us = (TCanvas*)file3_offset280us->Get("c03_pe");
    hinactivebkgC3_offset280us  = (TH1F*)c03_pe_offset280us->GetPrimitive("petotothC3"); hinactivebkgC3_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC3_offset280us = (TH1F*)c03_pe_offset280us->GetPrimitive("petotineC3"); hinelasticbkgC3_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC3_offset280us    = (TH1F*)c03_pe_offset280us->GetPrimitive("petotcapC3"); hsimsignalC3_offset280us->Scale(FlukaMCsfoffset280us);
    c03_pe_offset200us = (TCanvas*)file3_offset200us->Get("c03_pe");
    hinactivebkgC3_offset200us  = (TH1F*)c03_pe_offset200us->GetPrimitive("petotothC3"); hinactivebkgC3_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC3_offset200us = (TH1F*)c03_pe_offset200us->GetPrimitive("petotineC3"); hinelasticbkgC3_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC3_offset200us    = (TH1F*)c03_pe_offset200us->GetPrimitive("petotcapC3"); hsimsignalC3_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC3 = (TH1F*)hinactivebkgC3_offset280us->Clone("hinactivebkgC3"); // clone to make sure two hist has same axis binning
    hinactivebkgC3->Add(hinactivebkgC3_offset200us);
    hinelasticbkgC3 = (TH1F*)hinelasticbkgC3_offset280us->Clone("hinelasticbkgC3");
    hinelasticbkgC3->Add(hinelasticbkgC3_offset200us);
    hsimsignalC3 = (TH1F*)hsimsignalC3_offset280us->Clone("hsimsignalC3");
    hsimsignalC3->Add(hsimsignalC3_offset200us);
    // time slice 0
    c03_pe_tslice0_offset280us             = (TCanvas*)file3_offset280us->Get("c03_pe_tslice0");
    hinactivebkgC3_pe_tslice0_offset280us  = (TH1F*)c03_pe_tslice0_offset280us->GetPrimitive("petotothC3_t0_160us"); hinactivebkgC3_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC3_pe_tslice0_offset280us = (TH1F*)c03_pe_tslice0_offset280us->GetPrimitive("petotineC3_t0_160us"); hinelasticbkgC3_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC3_pe_tslice0_offset280us    = (TH1F*)c03_pe_tslice0_offset280us->GetPrimitive("petotcapC3_t0_160us"); hsimsignalC3_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    c03_pe_tslice0_offset200us             = (TCanvas*)file3_offset200us->Get("c03_pe_tslice0");
    hinactivebkgC3_pe_tslice0_offset200us  = (TH1F*)c03_pe_tslice0_offset200us->GetPrimitive("petotothC3_t0_160us"); hinactivebkgC3_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC3_pe_tslice0_offset200us = (TH1F*)c03_pe_tslice0_offset200us->GetPrimitive("petotineC3_t0_160us"); hinelasticbkgC3_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC3_pe_tslice0_offset200us    = (TH1F*)c03_pe_tslice0_offset200us->GetPrimitive("petotcapC3_t0_160us"); hsimsignalC3_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC3_pe_tslice0 = (TH1F*)hinactivebkgC3_pe_tslice0_offset280us->Clone("hinactivebkgC3_pe_tslice0");
    hinactivebkgC3_pe_tslice0->Add(hinactivebkgC3_pe_tslice0_offset200us);
    hinelasticbkgC3_pe_tslice0 = (TH1F*)hinelasticbkgC3_pe_tslice0_offset280us->Clone("hinelasticbkgC3_pe_tslice0");
    hinelasticbkgC3_pe_tslice0->Add(hinelasticbkgC3_pe_tslice0_offset200us);
    hsimsignalC3_pe_tslice0 = (TH1F*)hsimsignalC3_pe_tslice0_offset280us->Clone("hsimsignalC3_pe_tslice0");
    hsimsignalC3_pe_tslice0->Add(hsimsignalC3_pe_tslice0_offset200us);
    // time slice 1
    c03_pe_tslice1_offset280us             = (TCanvas*)file3_offset280us->Get("c03_pe_tslice1");
    hinactivebkgC3_pe_tslice1_offset280us  = (TH1F*)c03_pe_tslice1_offset280us->GetPrimitive("petotothC3_t160_480us"); hinactivebkgC3_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC3_pe_tslice1_offset280us = (TH1F*)c03_pe_tslice1_offset280us->GetPrimitive("petotineC3_t160_480us"); hinelasticbkgC3_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC3_pe_tslice1_offset280us    = (TH1F*)c03_pe_tslice1_offset280us->GetPrimitive("petotcapC3_t160_480us"); hsimsignalC3_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    c03_pe_tslice1_offset200us             = (TCanvas*)file3_offset200us->Get("c03_pe_tslice1");
    hinactivebkgC3_pe_tslice1_offset200us  = (TH1F*)c03_pe_tslice1_offset200us->GetPrimitive("petotothC3_t160_480us"); hinactivebkgC3_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC3_pe_tslice1_offset200us = (TH1F*)c03_pe_tslice1_offset200us->GetPrimitive("petotineC3_t160_480us"); hinelasticbkgC3_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC3_pe_tslice1_offset200us    = (TH1F*)c03_pe_tslice1_offset200us->GetPrimitive("petotcapC3_t160_480us"); hsimsignalC3_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC3_pe_tslice1 = (TH1F*)hinactivebkgC3_pe_tslice1_offset280us->Clone("hinactivebkgC3_pe_tslice1");
    hinactivebkgC3_pe_tslice1->Add(hinactivebkgC3_pe_tslice1_offset200us);
    hinelasticbkgC3_pe_tslice1 = (TH1F*)hinelasticbkgC3_pe_tslice1_offset280us->Clone("hinelasticbkgC3_pe_tslice1");
    hinelasticbkgC3_pe_tslice1->Add(hinelasticbkgC3_pe_tslice1_offset200us);
    hsimsignalC3_pe_tslice1 = (TH1F*)hsimsignalC3_pe_tslice1_offset280us->Clone("hsimsignalC3_pe_tslice1");
    hsimsignalC3_pe_tslice1->Add(hsimsignalC3_pe_tslice1_offset200us);
    // time slice 2
    c03_pe_tslice2_offset280us             = (TCanvas*)file3_offset280us->Get("c03_pe_tslice2");
    hinactivebkgC3_pe_tslice2_offset280us  = (TH1F*)c03_pe_tslice2_offset280us->GetPrimitive("petotothC3_t480_1050us"); hinactivebkgC3_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC3_pe_tslice2_offset280us = (TH1F*)c03_pe_tslice2_offset280us->GetPrimitive("petotineC3_t480_1050us"); hinelasticbkgC3_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC3_pe_tslice2_offset280us    = (TH1F*)c03_pe_tslice2_offset280us->GetPrimitive("petotcapC3_t480_1050us"); hsimsignalC3_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    c03_pe_tslice2_offset200us             = (TCanvas*)file3_offset200us->Get("c03_pe_tslice2");
    hinactivebkgC3_pe_tslice2_offset200us  = (TH1F*)c03_pe_tslice2_offset200us->GetPrimitive("petotothC3_t480_1050us"); hinactivebkgC3_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC3_pe_tslice2_offset200us = (TH1F*)c03_pe_tslice2_offset200us->GetPrimitive("petotineC3_t480_1050us"); hinelasticbkgC3_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC3_pe_tslice2_offset200us    = (TH1F*)c03_pe_tslice2_offset200us->GetPrimitive("petotcapC3_t480_1050us"); hsimsignalC3_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC3_pe_tslice2 = (TH1F*)hinactivebkgC3_pe_tslice2_offset280us->Clone("hinactivebkgC3_pe_tslice2");
    hinactivebkgC3_pe_tslice2->Add(hinactivebkgC3_pe_tslice2_offset200us);
    hinelasticbkgC3_pe_tslice2 = (TH1F*)hinelasticbkgC3_pe_tslice2_offset280us->Clone("hinelasticbkgC3_pe_tslice2");
    hinelasticbkgC3_pe_tslice2->Add(hinelasticbkgC3_pe_tslice2_offset200us);
    hsimsignalC3_pe_tslice2 = (TH1F*)hsimsignalC3_pe_tslice2_offset280us->Clone("hsimsignalC3_pe_tslice2");
    hsimsignalC3_pe_tslice2->Add(hsimsignalC3_pe_tslice2_offset200us);
    // timing
    c03_timing_offset280us = (TCanvas*)file3_offset280us->Get("c03_timing");
    hsimsignalC3time_offset280us = (TH1F*)c03_timing_offset280us->GetPrimitive("offlinetimecapC3");   hsimsignalC3time_offset280us->Scale(FlukaMCsfoffset280us);
    hinactivebkgC3time_offset280us = (TH1F*)c03_timing_offset280us->GetPrimitive("offlinetimeothC3"); hinactivebkgC3time_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC3time_offset280us = (TH1F*)c03_timing_offset280us->GetPrimitive("offlinetimeineC3"); hinelasticbkgC3time_offset280us->Scale(FlukaMCsfoffset280us);
    c03_timing_offset200us = (TCanvas*)file3_offset200us->Get("c03_timing");
    hsimsignalC3time_offset200us = (TH1F*)c03_timing_offset200us->GetPrimitive("offlinetimecapC3");   hsimsignalC3time_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC3time_offset200us = (TH1F*)c03_timing_offset200us->GetPrimitive("offlinetimeothC3"); hinactivebkgC3time_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC3time_offset200us = (TH1F*)c03_timing_offset200us->GetPrimitive("offlinetimeineC3"); hinelasticbkgC3time_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC3time = (TH1F*)hsimsignalC3time_offset280us->Clone("hsimsignalC3time");
    hsimsignalC3time->Add(hsimsignalC3time_offset200us);
    hinactivebkgC3time = (TH1F*)hinactivebkgC3time_offset280us->Clone("hinactivebkgC3time");
    hinactivebkgC3time->Add(hinactivebkgC3time_offset200us);
    hinelasticbkgC3time = (TH1F*)hinelasticbkgC3time_offset280us->Clone("hinelasticbkgC3time");
    hinelasticbkgC3time->Add(hinelasticbkgC3time_offset200us);

    // C4
    c04_pe_offset280us = (TCanvas*)file3_offset280us->Get("c04_pe");
    hinactivebkgC4_offset280us  = (TH1F*)c04_pe_offset280us->GetPrimitive("petotothC4"); hinactivebkgC4_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC4_offset280us = (TH1F*)c04_pe_offset280us->GetPrimitive("petotineC4"); hinelasticbkgC4_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC4_offset280us    = (TH1F*)c04_pe_offset280us->GetPrimitive("petotcapC4"); hsimsignalC4_offset280us->Scale(FlukaMCsfoffset280us);
    c04_pe_offset200us = (TCanvas*)file3_offset200us->Get("c04_pe");
    hinactivebkgC4_offset200us  = (TH1F*)c04_pe_offset200us->GetPrimitive("petotothC4"); hinactivebkgC4_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC4_offset200us = (TH1F*)c04_pe_offset200us->GetPrimitive("petotineC4"); hinelasticbkgC4_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC4_offset200us    = (TH1F*)c04_pe_offset200us->GetPrimitive("petotcapC4"); hsimsignalC4_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC4 = (TH1F*)hinactivebkgC4_offset280us->Clone("hinactivebkgC4"); // clone to make sure two hist has same axis binning
    hinactivebkgC4->Add(hinactivebkgC4_offset200us);
    hinelasticbkgC4 = (TH1F*)hinelasticbkgC4_offset280us->Clone("hinelasticbkgC4");
    hinelasticbkgC4->Add(hinelasticbkgC4_offset200us);
    hsimsignalC4 = (TH1F*)hsimsignalC4_offset280us->Clone("hsimsignalC4");
    hsimsignalC4->Add(hsimsignalC4_offset200us);
    // time slice 0
    c04_pe_tslice0_offset280us             = (TCanvas*)file3_offset280us->Get("c04_pe_tslice0");
    hinactivebkgC4_pe_tslice0_offset280us  = (TH1F*)c04_pe_tslice0_offset280us->GetPrimitive("petotothC4_t0_160us"); hinactivebkgC4_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC4_pe_tslice0_offset280us = (TH1F*)c04_pe_tslice0_offset280us->GetPrimitive("petotineC4_t0_160us"); hinelasticbkgC4_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC4_pe_tslice0_offset280us    = (TH1F*)c04_pe_tslice0_offset280us->GetPrimitive("petotcapC4_t0_160us"); hsimsignalC4_pe_tslice0_offset280us->Scale(FlukaMCsfoffset280us);
    c04_pe_tslice0_offset200us             = (TCanvas*)file3_offset200us->Get("c04_pe_tslice0");
    hinactivebkgC4_pe_tslice0_offset200us  = (TH1F*)c04_pe_tslice0_offset200us->GetPrimitive("petotothC4_t0_160us"); hinactivebkgC4_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC4_pe_tslice0_offset200us = (TH1F*)c04_pe_tslice0_offset200us->GetPrimitive("petotineC4_t0_160us"); hinelasticbkgC4_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC4_pe_tslice0_offset200us    = (TH1F*)c04_pe_tslice0_offset200us->GetPrimitive("petotcapC4_t0_160us"); hsimsignalC4_pe_tslice0_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC4_pe_tslice0 = (TH1F*)hinactivebkgC4_pe_tslice0_offset280us->Clone("hinactivebkgC4_pe_tslice0");
    hinactivebkgC4_pe_tslice0->Add(hinactivebkgC4_pe_tslice0_offset200us);
    hinelasticbkgC4_pe_tslice0 = (TH1F*)hinelasticbkgC4_pe_tslice0_offset280us->Clone("hinelasticbkgC4_pe_tslice0");
    hinelasticbkgC4_pe_tslice0->Add(hinelasticbkgC4_pe_tslice0_offset200us);
    hsimsignalC4_pe_tslice0 = (TH1F*)hsimsignalC4_pe_tslice0_offset280us->Clone("hsimsignalC4_pe_tslice0");
    hsimsignalC4_pe_tslice0->Add(hsimsignalC4_pe_tslice0_offset200us);
    // time slice 1
    c04_pe_tslice1_offset280us             = (TCanvas*)file3_offset280us->Get("c04_pe_tslice1");
    hinactivebkgC4_pe_tslice1_offset280us  = (TH1F*)c04_pe_tslice1_offset280us->GetPrimitive("petotothC4_t160_480us"); hinactivebkgC4_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC4_pe_tslice1_offset280us = (TH1F*)c04_pe_tslice1_offset280us->GetPrimitive("petotineC4_t160_480us"); hinelasticbkgC4_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC4_pe_tslice1_offset280us    = (TH1F*)c04_pe_tslice1_offset280us->GetPrimitive("petotcapC4_t160_480us"); hsimsignalC4_pe_tslice1_offset280us->Scale(FlukaMCsfoffset280us);
    c04_pe_tslice1_offset200us             = (TCanvas*)file3_offset200us->Get("c04_pe_tslice1");
    hinactivebkgC4_pe_tslice1_offset200us  = (TH1F*)c04_pe_tslice1_offset200us->GetPrimitive("petotothC4_t160_480us"); hinactivebkgC4_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC4_pe_tslice1_offset200us = (TH1F*)c04_pe_tslice1_offset200us->GetPrimitive("petotineC4_t160_480us"); hinelasticbkgC4_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC4_pe_tslice1_offset200us    = (TH1F*)c04_pe_tslice1_offset200us->GetPrimitive("petotcapC4_t160_480us"); hsimsignalC4_pe_tslice1_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC4_pe_tslice1 = (TH1F*)hinactivebkgC4_pe_tslice1_offset280us->Clone("hinactivebkgC4_pe_tslice1");
    hinactivebkgC4_pe_tslice1->Add(hinactivebkgC4_pe_tslice1_offset200us);
    hinelasticbkgC4_pe_tslice1 = (TH1F*)hinelasticbkgC4_pe_tslice1_offset280us->Clone("hinelasticbkgC4_pe_tslice1");
    hinelasticbkgC4_pe_tslice1->Add(hinelasticbkgC4_pe_tslice1_offset200us);
    hsimsignalC4_pe_tslice1 = (TH1F*)hsimsignalC4_pe_tslice1_offset280us->Clone("hsimsignalC4_pe_tslice1");
    hsimsignalC4_pe_tslice1->Add(hsimsignalC4_pe_tslice1_offset200us);
    // time slice 2
    c04_pe_tslice2_offset280us             = (TCanvas*)file3_offset280us->Get("c04_pe_tslice2");
    hinactivebkgC4_pe_tslice2_offset280us  = (TH1F*)c04_pe_tslice2_offset280us->GetPrimitive("petotothC4_t480_1050us"); hinactivebkgC4_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC4_pe_tslice2_offset280us = (TH1F*)c04_pe_tslice2_offset280us->GetPrimitive("petotineC4_t480_1050us"); hinelasticbkgC4_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    hsimsignalC4_pe_tslice2_offset280us    = (TH1F*)c04_pe_tslice2_offset280us->GetPrimitive("petotcapC4_t480_1050us"); hsimsignalC4_pe_tslice2_offset280us->Scale(FlukaMCsfoffset280us);
    c04_pe_tslice2_offset200us             = (TCanvas*)file3_offset200us->Get("c04_pe_tslice2");
    hinactivebkgC4_pe_tslice2_offset200us  = (TH1F*)c04_pe_tslice2_offset200us->GetPrimitive("petotothC4_t480_1050us"); hinactivebkgC4_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC4_pe_tslice2_offset200us = (TH1F*)c04_pe_tslice2_offset200us->GetPrimitive("petotineC4_t480_1050us"); hinelasticbkgC4_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC4_pe_tslice2_offset200us    = (TH1F*)c04_pe_tslice2_offset200us->GetPrimitive("petotcapC4_t480_1050us"); hsimsignalC4_pe_tslice2_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC4_pe_tslice2 = (TH1F*)hinactivebkgC4_pe_tslice2_offset280us->Clone("hinactivebkgC4_pe_tslice2");
    hinactivebkgC4_pe_tslice2->Add(hinactivebkgC4_pe_tslice2_offset200us);
    hinelasticbkgC4_pe_tslice2 = (TH1F*)hinelasticbkgC4_pe_tslice2_offset280us->Clone("hinelasticbkgC4_pe_tslice2");
    hinelasticbkgC4_pe_tslice2->Add(hinelasticbkgC4_pe_tslice2_offset200us);
    hsimsignalC4_pe_tslice2 = (TH1F*)hsimsignalC4_pe_tslice2_offset280us->Clone("hsimsignalC4_pe_tslice2");
    hsimsignalC4_pe_tslice2->Add(hsimsignalC4_pe_tslice2_offset200us);
    // timing
    c04_timing_offset280us = (TCanvas*)file3_offset280us->Get("c04_timing");
    hsimsignalC4time_offset280us = (TH1F*)c04_timing_offset280us->GetPrimitive("offlinetimecapC4");   hsimsignalC4time_offset280us->Scale(FlukaMCsfoffset280us);
    hinactivebkgC4time_offset280us = (TH1F*)c04_timing_offset280us->GetPrimitive("offlinetimeothC4"); hinactivebkgC4time_offset280us->Scale(FlukaMCsfoffset280us);
    hinelasticbkgC4time_offset280us = (TH1F*)c04_timing_offset280us->GetPrimitive("offlinetimeineC4"); hinelasticbkgC4time_offset280us->Scale(FlukaMCsfoffset280us);
    c04_timing_offset200us = (TCanvas*)file3_offset200us->Get("c04_timing");
    hsimsignalC4time_offset200us = (TH1F*)c04_timing_offset200us->GetPrimitive("offlinetimecapC4");   hsimsignalC4time_offset200us->Scale(FlukaMCsfoffset200us);
    hinactivebkgC4time_offset200us = (TH1F*)c04_timing_offset200us->GetPrimitive("offlinetimeothC4"); hinactivebkgC4time_offset200us->Scale(FlukaMCsfoffset200us);
    hinelasticbkgC4time_offset200us = (TH1F*)c04_timing_offset200us->GetPrimitive("offlinetimeineC4"); hinelasticbkgC4time_offset200us->Scale(FlukaMCsfoffset200us);
    hsimsignalC4time = (TH1F*)hsimsignalC4time_offset280us->Clone("hsimsignalC4time");
    hsimsignalC4time->Add(hsimsignalC4time_offset200us);
    hinactivebkgC4time = (TH1F*)hinactivebkgC4time_offset280us->Clone("hinactivebkgC4time");
    hinactivebkgC4time->Add(hinactivebkgC4time_offset200us);
    hinelasticbkgC4time = (TH1F*)hinelasticbkgC4time_offset280us->Clone("hinelasticbkgC4time");
    hinelasticbkgC4time->Add(hinelasticbkgC4time_offset200us);
  }

  ////////////////////////
  // Start overlay plots
  ////////////////////////

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
  std::cout << " C4 data-cosmic PE total entries:   " << hpnsdataC4->Integral(1, 50) - hcosmicdataC4->Integral(1, 50) << std::endl;
  TLegend *legc4 = new TLegend(0.24,0.63,0.75,0.89);
  legc4->SetTextSize(0.05);
  legc4->AddEntry(hcosmicdataC4, "Cosmics (data driven)", "f");
  legc4->AddEntry(hinactivebkgC4, "All inactive materials neutron bkg (Fluka)", "f");
  legc4->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  legc4->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");
  legc4->AddEntry(hpnsdataC4, "Data", "ep");
  legc4->SetBorderSize(0);
  legc4->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_pe_logY.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut_pe.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe.pdf");

  // only plot cosmics
  hcosmicdataC4->SetStats(0);
  hcosmicdataC4->GetXaxis()->SetTitle("PE on C4");
  hcosmicdataC4->SetTitle("");
  hcosmicdataC4->Draw("hist");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe.pdf");

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
  TLegend *legc4nocosmic = new TLegend(0.24,0.63,0.75,0.89);
  legc4nocosmic->SetTextSize(0.05);
  legc4nocosmic->AddEntry(hinactivebkgC4, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmic->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmic->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");
  legc4nocosmic->AddEntry(hpnsdataC4, "Data (cosmics subtracted)", "ep");
  legc4nocosmic->SetBorderSize(0);
  legc4nocosmic->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe.pdf");

  // C4 PE time slice 0
  THStack *stc4petslice0 = new THStack();
  stc4petslice0->SetMaximum(100000.);
  hcosmicdataC4petimeslice0->SetFillColor(2);
  hinactivebkgC4_pe_tslice0->SetFillColor(4);
  hinelasticbkgC4_pe_tslice0->SetFillColor(7);
  hsimsignalC4_pe_tslice0->SetFillColor(6);
  stc4petslice0->Add(hcosmicdataC4petimeslice0);
  stc4petslice0->Add(hinactivebkgC4_pe_tslice0);
  stc4petslice0->Add(hinelasticbkgC4_pe_tslice0);
  stc4petslice0->Add(hsimsignalC4_pe_tslice0);
  stc4petslice0->Draw("hist");
  stc4petslice0->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice0->SetLineColor(1);
  hpnsdataC4petimeslice0->SetMarkerStyle(20);
  hpnsdataC4petimeslice0->SetMarkerSize(0.6);
  hpnsdataC4petimeslice0->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 0 *** " << std::endl;
  std::cout << " hcosmicdataC4:   " << hcosmicdataC4petimeslice0->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC4:  " << hinactivebkgC4_pe_tslice0->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC4: " << hinelasticbkgC4_pe_tslice0->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC4:    " << hsimsignalC4_pe_tslice0->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC4:      " << hpnsdataC4petimeslice0->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 0 *** " << std::endl;

  TLegend *legc4tslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc4tslice0->SetTextSize(0.05);
  legc4tslice0->AddEntry(hcosmicdataC4petimeslice0, "Cosmics (data driven)", "f");
  legc4tslice0->AddEntry(hinactivebkgC4_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc4tslice0->AddEntry(hinelasticbkgC4_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc4tslice0->AddEntry(hsimsignalC4_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc4tslice0->AddEntry(hpnsdataC4petimeslice0, "Data", "ep");
  legc4tslice0->SetBorderSize(0);
  legc4tslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice0_logY.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice0_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice0.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice0.pdf");

  // only plot cosmics
  hcosmicdataC4petimeslice0->SetStats(0);
  hcosmicdataC4petimeslice0->GetXaxis()->SetTitle("PE on C4");
  hcosmicdataC4petimeslice0->SetTitle("");
  hcosmicdataC4petimeslice0->Draw("hist");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice0.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice0.pdf");

  // subtract cosmics from pns data
  THStack *stc4nocosmicpetslice0 = new THStack();
  stc4nocosmicpetslice0->SetMaximum(60000.);
  stc4nocosmicpetslice0->Add(hinactivebkgC4_pe_tslice0);
  stc4nocosmicpetslice0->Add(hinelasticbkgC4_pe_tslice0);
  stc4nocosmicpetslice0->Add(hsimsignalC4_pe_tslice0);
  stc4nocosmicpetslice0->Draw("hist");
  stc4nocosmicpetslice0->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice0->Add(hcosmicdataC4petimeslice0, -1);
  hpnsdataC4petimeslice0->SetLineColor(1);
  hpnsdataC4petimeslice0->SetMarkerStyle(20);
  hpnsdataC4petimeslice0->SetMarkerSize(0.6);
  hpnsdataC4petimeslice0->Draw("E1 X0 SAME");
  TLegend *legc4nocosmictslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc4nocosmictslice0->SetTextSize(0.05);
  legc4nocosmictslice0->AddEntry(hinactivebkgC4_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmictslice0->AddEntry(hinelasticbkgC4_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmictslice0->AddEntry(hsimsignalC4_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc4nocosmictslice0->AddEntry(hpnsdataC4petimeslice0, "Data (cosmics subtracted)", "ep");
  legc4nocosmictslice0->SetBorderSize(0);
  legc4nocosmictslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice0.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice0.pdf");
  // end C4 PE time slice 0

  // begin C4 PE time slice 1
  THStack *stc4petslice1 = new THStack();
  stc4petslice1->SetMaximum(100000.);
  hcosmicdataC4petimeslice1->SetFillColor(2);
  hinactivebkgC4_pe_tslice1->SetFillColor(4);
  hinelasticbkgC4_pe_tslice1->SetFillColor(7);
  hsimsignalC4_pe_tslice1->SetFillColor(6);
  stc4petslice1->Add(hcosmicdataC4petimeslice1);
  stc4petslice1->Add(hinactivebkgC4_pe_tslice1);
  stc4petslice1->Add(hinelasticbkgC4_pe_tslice1);
  stc4petslice1->Add(hsimsignalC4_pe_tslice1);
  stc4petslice1->Draw("hist");
  stc4petslice1->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice1->SetLineColor(1);
  hpnsdataC4petimeslice1->SetMarkerStyle(20);
  hpnsdataC4petimeslice1->SetMarkerSize(0.6);
  hpnsdataC4petimeslice1->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 1 *** " << std::endl;
  std::cout << " hcosmicdataC4:   " << hcosmicdataC4petimeslice1->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC4:  " << hinactivebkgC4_pe_tslice1->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC4: " << hinelasticbkgC4_pe_tslice1->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC4:    " << hsimsignalC4_pe_tslice1->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC4:      " << hpnsdataC4petimeslice1->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 1 *** " << std::endl;

  TLegend *legc4tslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc4tslice1->SetTextSize(0.05);
  legc4tslice1->AddEntry(hcosmicdataC4petimeslice1, "Cosmics (data driven)", "f");
  legc4tslice1->AddEntry(hinactivebkgC4_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc4tslice1->AddEntry(hinelasticbkgC4_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc4tslice1->AddEntry(hsimsignalC4_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc4tslice1->AddEntry(hpnsdataC4petimeslice1, "Data", "ep");
  legc4tslice1->SetBorderSize(0);
  legc4tslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice1_logY.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice1_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice1.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice1.pdf");

  // only plot cosmics
  hcosmicdataC4petimeslice1->SetStats(0);
  hcosmicdataC4petimeslice1->GetXaxis()->SetTitle("PE on C4");
  hcosmicdataC4petimeslice1->SetTitle("");
  hcosmicdataC4petimeslice1->Draw("hist");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice1.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice1.pdf");

  // subtract cosmics from pns data
  THStack *stc4nocosmicpetslice1 = new THStack();
  stc4nocosmicpetslice1->SetMaximum(60000.);
  stc4nocosmicpetslice1->Add(hinactivebkgC4_pe_tslice1);
  stc4nocosmicpetslice1->Add(hinelasticbkgC4_pe_tslice1);
  stc4nocosmicpetslice1->Add(hsimsignalC4_pe_tslice1);
  stc4nocosmicpetslice1->Draw("hist");
  stc4nocosmicpetslice1->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice1->Add(hcosmicdataC4petimeslice1, -1);
  hpnsdataC4petimeslice1->SetLineColor(1);
  hpnsdataC4petimeslice1->SetMarkerStyle(20);
  hpnsdataC4petimeslice1->SetMarkerSize(0.6);
  hpnsdataC4petimeslice1->Draw("E1 X0 SAME");
  TLegend *legc4nocosmictslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc4nocosmictslice1->SetTextSize(0.05);
  legc4nocosmictslice1->AddEntry(hinactivebkgC4_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmictslice1->AddEntry(hinelasticbkgC4_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmictslice1->AddEntry(hsimsignalC4_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc4nocosmictslice1->AddEntry(hpnsdataC4petimeslice1, "Data (cosmics subtracted)", "ep");
  legc4nocosmictslice1->SetBorderSize(0);
  legc4nocosmictslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice1.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice1.pdf");
  // end C4 PE time slice 1

  // begin C4 PE time slice 2
  THStack *stc4petslice2 = new THStack();
  stc4petslice2->SetMaximum(100000.);
  hcosmicdataC4petimeslice2->SetFillColor(2);
  hinactivebkgC4_pe_tslice2->SetFillColor(4);
  hinelasticbkgC4_pe_tslice2->SetFillColor(7);
  hsimsignalC4_pe_tslice2->SetFillColor(6);
  stc4petslice2->Add(hcosmicdataC4petimeslice2);
  stc4petslice2->Add(hinactivebkgC4_pe_tslice2);
  stc4petslice2->Add(hinelasticbkgC4_pe_tslice2);
  stc4petslice2->Add(hsimsignalC4_pe_tslice2);
  stc4petslice2->Draw("hist");
  stc4petslice2->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice2->SetLineColor(1);
  hpnsdataC4petimeslice2->SetMarkerStyle(20);
  hpnsdataC4petimeslice2->SetMarkerSize(0.6);
  hpnsdataC4petimeslice2->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 2 *** " << std::endl;
  std::cout << " hcosmicdataC4:   " << hcosmicdataC4petimeslice2->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC4:  " << hinactivebkgC4_pe_tslice2->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC4: " << hinelasticbkgC4_pe_tslice2->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC4:    " << hsimsignalC4_pe_tslice2->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC4:      " << hpnsdataC4petimeslice2->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 2 *** " << std::endl;

  TLegend *legc4tslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc4tslice2->SetTextSize(0.05);
  legc4tslice2->AddEntry(hcosmicdataC4petimeslice2, "Cosmics (data driven)", "f");
  legc4tslice2->AddEntry(hinactivebkgC4_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc4tslice2->AddEntry(hinelasticbkgC4_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc4tslice2->AddEntry(hsimsignalC4_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc4tslice2->AddEntry(hpnsdataC4petimeslice2, "Data", "ep");
  legc4tslice2->SetBorderSize(0);
  legc4tslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice2_logY.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice2_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice2.root");
  can->SaveAs("PNSAnaC4_minimumcut_pe_timeslice2.pdf");

  // only plot cosmics
  hcosmicdataC4petimeslice2->SetStats(0);
  hcosmicdataC4petimeslice2->GetXaxis()->SetTitle("PE on C4");
  hcosmicdataC4petimeslice2->SetTitle("");
  hcosmicdataC4petimeslice2->Draw("hist");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice2.root");
  can->SaveAs("PNSAnaC4_minimumcut_cosmics_pe_timeslice2.pdf");

  // subtract cosmics from pns data
  THStack *stc4nocosmicpetslice2 = new THStack();
  stc4nocosmicpetslice2->SetMaximum(60000.);
  stc4nocosmicpetslice2->Add(hinactivebkgC4_pe_tslice2);
  stc4nocosmicpetslice2->Add(hinelasticbkgC4_pe_tslice2);
  stc4nocosmicpetslice2->Add(hsimsignalC4_pe_tslice2);
  stc4nocosmicpetslice2->Draw("hist");
  stc4nocosmicpetslice2->GetXaxis()->SetTitle("PE on C4");
  can->Modified();
  hpnsdataC4petimeslice2->Add(hcosmicdataC4petimeslice2, -1);
  hpnsdataC4petimeslice2->SetLineColor(1);
  hpnsdataC4petimeslice2->SetMarkerStyle(20);
  hpnsdataC4petimeslice2->SetMarkerSize(0.6);
  hpnsdataC4petimeslice2->Draw("E1 X0 SAME");
  TLegend *legc4nocosmictslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc4nocosmictslice2->SetTextSize(0.05);
  legc4nocosmictslice2->AddEntry(hinactivebkgC4_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmictslice2->AddEntry(hinelasticbkgC4_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmictslice2->AddEntry(hsimsignalC4_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc4nocosmictslice2->AddEntry(hpnsdataC4petimeslice2, "Data (cosmics subtracted)", "ep");
  legc4nocosmictslice2->SetBorderSize(0);
  legc4nocosmictslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice2.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_pe_timeslice2.pdf");
  // end C4 PE time slice 2

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
  std::cout << " C4 time cosmic only tot:   "             << hcosmicdataC4time->Integral(1, 112)         << std::endl;
  std::cout << " C4 time cosmic only PE<1200 tot:   "     << hcosmicdataC4timelowPE->Integral(1, 112)    << std::endl;
  std::cout << " C4 time cosmic only PE>1200 tot:   "     << hcosmicdataC4timehighPE->Integral(1, 112)   << std::endl;

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
  std::cout << " C4 time pns run tot:   "         << hpnsdataC4time->Integral(1, 112)        << std::endl;
  std::cout << " C4 time pns run PE<1200 tot:   " << hpnsdataC4timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C4 time pns run PE>1200 tot:   " << hpnsdataC4timehighPE->Integral(1, 112)  << std::endl;

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
  std::cout << " C4 time (pns - cosmic) tot:   "         << hpnsdataC4time->Integral(1, 112)        << std::endl;
  std::cout << " C4 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC4timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C4 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC4timehighPE->Integral(1, 112)  << std::endl;

  // JINST review comment: compare timing data - Fluka
  // subtract cosmics from pns data
  THStack *stc4nocosmictiming = new THStack();
  stc4nocosmictiming->SetMaximum(3100.);
  hinactivebkgC4time->SetFillColor(4);
  hinelasticbkgC4time->SetFillColor(7);
  hsimsignalC4time->SetFillColor(6);
  stc4nocosmictiming->Add(hinactivebkgC4time);
  stc4nocosmictiming->Add(hinelasticbkgC4time);
  stc4nocosmictiming->Add(hsimsignalC4time);
  stc4nocosmictiming->Draw("hist");
  stc4nocosmictiming->GetXaxis()->SetTitle("Time (#mus) on C4");
  can->Modified();
  // modify data hist x axis scale to us
  int nbins = hpnsdataC4time->GetXaxis()->GetNbins();
  double xmin = hpnsdataC4time->GetXaxis()->GetXmin() * 0.016; // ticks x16ns/ tick / 1000
  double xmax = hpnsdataC4time->GetXaxis()->GetXmax() * 0.016;
  // Create new histogram with scaled limits
  TH1F *hpnsdataC4time_new = new TH1F("hpnsdataC4time_new", "Rescaled Histogram", nbins, xmin, xmax);
  // Fill new histogram with original values
  for (int i = 1; i <= nbins; ++i) {
    hpnsdataC4time_new->SetBinContent(i, hpnsdataC4time->GetBinContent(i));
    hpnsdataC4time_new->SetBinError(i, hpnsdataC4time->GetBinError(i));
  }
  hpnsdataC4time_new->SetLineColor(1);
  hpnsdataC4time_new->SetMarkerStyle(20);
  hpnsdataC4time_new->SetMarkerSize(0.6);
  hpnsdataC4time_new->Draw("E1 X0 SAME");
  TLegend *legc4nocosmictiming = new TLegend(0.24,0.63,0.75,0.89);
  legc4nocosmictiming->SetTextSize(0.05);
  legc4nocosmictiming->AddEntry(hinactivebkgC4time, "All inactive materials neutron bkg (Fluka)", "f");
  legc4nocosmictiming->AddEntry(hinelasticbkgC4time, "Active LAr n inelastic (Fluka)", "f");
  legc4nocosmictiming->AddEntry(hsimsignalC4time, "Active LAr n capture (Fluka)", "f");
  legc4nocosmictiming->AddEntry(hpnsdataC4time_new, "Data (cosmics subtracted)", "ep");
  legc4nocosmictiming->SetBorderSize(0);
  legc4nocosmictiming->Draw();
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_stackMC.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic_timing_stackMC.pdf");


  // C3
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
  std::cout << " C3 data-cosmic PE total entries:   " << hpnsdataC3->Integral(1, 50) - hcosmicdataC3->Integral(1, 50) << std::endl;
  TLegend *legc3 = new TLegend(0.24,0.63,0.75,0.89);
  legc3->SetTextSize(0.05);
  legc3->AddEntry(hcosmicdataC3, "Cosmics (data driven)", "f");
  legc3->AddEntry(hinactivebkgC3, "All inactive materials neutron bkg (Fluka)", "f");
  legc3->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  legc3->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");
  legc3->AddEntry(hpnsdataC3, "Data", "ep");
  legc3->SetBorderSize(0);
  legc3->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_pe_logY.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut_pe.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe.pdf");

  // only plot cosmics
  hcosmicdataC3->SetStats(0);
  hcosmicdataC3->GetXaxis()->SetTitle("PE on C3");
  hcosmicdataC3->SetTitle("");
  hcosmicdataC3->Draw("hist");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe.pdf");

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
  TLegend *legc3nocosmic = new TLegend(0.24,0.63,0.75,0.89);
  legc3nocosmic->SetTextSize(0.05);
  legc3nocosmic->AddEntry(hinactivebkgC3, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmic->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmic->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");
  legc3nocosmic->AddEntry(hpnsdataC3, "Data (cosmics subtracted)", "ep");
  legc3nocosmic->SetBorderSize(0);
  legc3nocosmic->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe.pdf");

  // C3 PE time slice 0
  THStack *stc3petslice0 = new THStack();
  stc3petslice0->SetMaximum(100000.);
  hcosmicdataC3petimeslice0->SetFillColor(2);
  hinactivebkgC3_pe_tslice0->SetFillColor(4);
  hinelasticbkgC3_pe_tslice0->SetFillColor(7);
  hsimsignalC3_pe_tslice0->SetFillColor(6);
  stc3petslice0->Add(hcosmicdataC3petimeslice0);
  stc3petslice0->Add(hinactivebkgC3_pe_tslice0);
  stc3petslice0->Add(hinelasticbkgC3_pe_tslice0);
  stc3petslice0->Add(hsimsignalC3_pe_tslice0);
  stc3petslice0->Draw("hist");
  stc3petslice0->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice0->SetLineColor(1);
  hpnsdataC3petimeslice0->SetMarkerStyle(20);
  hpnsdataC3petimeslice0->SetMarkerSize(0.6);
  hpnsdataC3petimeslice0->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 0 *** " << std::endl;
  std::cout << " hcosmicdataC3:   " << hcosmicdataC3petimeslice0->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC3:  " << hinactivebkgC3_pe_tslice0->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC3: " << hinelasticbkgC3_pe_tslice0->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC3:    " << hsimsignalC3_pe_tslice0->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC3:      " << hpnsdataC3petimeslice0->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 0 *** " << std::endl;

  TLegend *legc3tslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc3tslice0->SetTextSize(0.05);
  legc3tslice0->AddEntry(hcosmicdataC3petimeslice0, "Cosmics (data driven)", "f");
  legc3tslice0->AddEntry(hinactivebkgC3_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc3tslice0->AddEntry(hinelasticbkgC3_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc3tslice0->AddEntry(hsimsignalC3_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc3tslice0->AddEntry(hpnsdataC3petimeslice0, "Data", "ep");
  legc3tslice0->SetBorderSize(0);
  legc3tslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice0_logY.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice0_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice0.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice0.pdf");

  // only plot cosmics
  hcosmicdataC3petimeslice0->SetStats(0);
  hcosmicdataC3petimeslice0->GetXaxis()->SetTitle("PE on C3");
  hcosmicdataC3petimeslice0->SetTitle("");
  hcosmicdataC3petimeslice0->Draw("hist");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice0.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice0.pdf");

  // subtract cosmics from pns data
  THStack *stc3nocosmicpetslice0 = new THStack();
  stc3nocosmicpetslice0->SetMaximum(60000.);
  stc3nocosmicpetslice0->Add(hinactivebkgC3_pe_tslice0);
  stc3nocosmicpetslice0->Add(hinelasticbkgC3_pe_tslice0);
  stc3nocosmicpetslice0->Add(hsimsignalC3_pe_tslice0);
  stc3nocosmicpetslice0->Draw("hist");
  stc3nocosmicpetslice0->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice0->Add(hcosmicdataC3petimeslice0, -1);
  hpnsdataC3petimeslice0->SetLineColor(1);
  hpnsdataC3petimeslice0->SetMarkerStyle(20);
  hpnsdataC3petimeslice0->SetMarkerSize(0.6);
  hpnsdataC3petimeslice0->Draw("E1 X0 SAME");
  TLegend *legc3nocosmictslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc3nocosmictslice0->SetTextSize(0.05);
  legc3nocosmictslice0->AddEntry(hinactivebkgC3_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmictslice0->AddEntry(hinelasticbkgC3_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmictslice0->AddEntry(hsimsignalC3_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc3nocosmictslice0->AddEntry(hpnsdataC3petimeslice0, "Data (cosmics subtracted)", "ep");
  legc3nocosmictslice0->SetBorderSize(0);
  legc3nocosmictslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice0.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice0.pdf");
  // end C3 PE time slice 0

  // start C3 PE time slice 1
  THStack *stc3petslice1 = new THStack();
  stc3petslice1->SetMaximum(100000.);
  hcosmicdataC3petimeslice1->SetFillColor(2);
  hinactivebkgC3_pe_tslice1->SetFillColor(4);
  hinelasticbkgC3_pe_tslice1->SetFillColor(7);
  hsimsignalC3_pe_tslice1->SetFillColor(6);
  stc3petslice1->Add(hcosmicdataC3petimeslice1);
  stc3petslice1->Add(hinactivebkgC3_pe_tslice1);
  stc3petslice1->Add(hinelasticbkgC3_pe_tslice1);
  stc3petslice1->Add(hsimsignalC3_pe_tslice1);
  stc3petslice1->Draw("hist");
  stc3petslice1->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice1->SetLineColor(1);
  hpnsdataC3petimeslice1->SetMarkerStyle(20);
  hpnsdataC3petimeslice1->SetMarkerSize(0.6);
  hpnsdataC3petimeslice1->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 1 *** " << std::endl;
  std::cout << " hcosmicdataC3:   " << hcosmicdataC3petimeslice1->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC3:  " << hinactivebkgC3_pe_tslice1->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC3: " << hinelasticbkgC3_pe_tslice1->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC3:    " << hsimsignalC3_pe_tslice1->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC3:      " << hpnsdataC3petimeslice1->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 1 *** " << std::endl;
  TLegend *legc3tslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc3tslice1->SetTextSize(0.05);
  legc3tslice1->AddEntry(hcosmicdataC3petimeslice1, "Cosmics (data driven)", "f");
  legc3tslice1->AddEntry(hinactivebkgC3_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc3tslice1->AddEntry(hinelasticbkgC3_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc3tslice1->AddEntry(hsimsignalC3_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc3tslice1->AddEntry(hpnsdataC3petimeslice1, "Data", "ep");
  legc3tslice1->SetBorderSize(0);
  legc3tslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice1_logY.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice1_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice1.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice1.pdf");

  // only plot cosmics
  hcosmicdataC3petimeslice1->SetStats(0);
  hcosmicdataC3petimeslice1->GetXaxis()->SetTitle("PE on C3");
  hcosmicdataC3petimeslice1->SetTitle("");
  hcosmicdataC3petimeslice1->Draw("hist");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice1.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice1.pdf");

  // subtract cosmics from pns data
  THStack *stc3nocosmicpetslice1 = new THStack();
  stc3nocosmicpetslice1->SetMaximum(60000.);
  stc3nocosmicpetslice1->Add(hinactivebkgC3_pe_tslice1);
  stc3nocosmicpetslice1->Add(hinelasticbkgC3_pe_tslice1);
  stc3nocosmicpetslice1->Add(hsimsignalC3_pe_tslice1);
  stc3nocosmicpetslice1->Draw("hist");
  stc3nocosmicpetslice1->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice1->Add(hcosmicdataC3petimeslice1, -1);
  hpnsdataC3petimeslice1->SetLineColor(1);
  hpnsdataC3petimeslice1->SetMarkerStyle(20);
  hpnsdataC3petimeslice1->SetMarkerSize(0.6);
  hpnsdataC3petimeslice1->Draw("E1 X0 SAME");
  TLegend *legc3nocosmictslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc3nocosmictslice1->SetTextSize(0.05);
  legc3nocosmictslice1->AddEntry(hinactivebkgC3_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmictslice1->AddEntry(hinelasticbkgC3_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmictslice1->AddEntry(hsimsignalC3_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc3nocosmictslice1->AddEntry(hpnsdataC3petimeslice1, "Data (cosmics subtracted)", "ep");
  legc3nocosmictslice1->SetBorderSize(0);
  legc3nocosmictslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice1.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice1.pdf");
  // end C3 PE time slice 1

  // start C3 PE time slice 2
  THStack *stc3petslice2 = new THStack();
  stc3petslice2->SetMaximum(100000.);
  hcosmicdataC3petimeslice2->SetFillColor(2);
  hinactivebkgC3_pe_tslice2->SetFillColor(4);
  hinelasticbkgC3_pe_tslice2->SetFillColor(7);
  hsimsignalC3_pe_tslice2->SetFillColor(6);
  stc3petslice2->Add(hcosmicdataC3petimeslice2);
  stc3petslice2->Add(hinactivebkgC3_pe_tslice2);
  stc3petslice2->Add(hinelasticbkgC3_pe_tslice2);
  stc3petslice2->Add(hsimsignalC3_pe_tslice2);
  stc3petslice2->Draw("hist");
  stc3petslice2->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice2->SetLineColor(1);
  hpnsdataC3petimeslice2->SetMarkerStyle(20);
  hpnsdataC3petimeslice2->SetMarkerSize(0.6);
  hpnsdataC3petimeslice2->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 2 *** " << std::endl;
  std::cout << " hcosmicdataC3:   " << hcosmicdataC3petimeslice2->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC3:  " << hinactivebkgC3_pe_tslice2->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC3: " << hinelasticbkgC3_pe_tslice2->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC3:    " << hsimsignalC3_pe_tslice2->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC3:      " << hpnsdataC3petimeslice2->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 2 *** " << std::endl;
  TLegend *legc3tslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc3tslice2->SetTextSize(0.05);
  legc3tslice2->AddEntry(hcosmicdataC3petimeslice2, "Cosmics (data driven)", "f");
  legc3tslice2->AddEntry(hinactivebkgC3_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc3tslice2->AddEntry(hinelasticbkgC3_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc3tslice2->AddEntry(hsimsignalC3_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc3tslice2->AddEntry(hpnsdataC3petimeslice2, "Data", "ep");
  legc3tslice2->SetBorderSize(0);
  legc3tslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice2_logY.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice2_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice2.root");
  can->SaveAs("PNSAnaC3_minimumcut_pe_timeslice2.pdf");

  // only plot cosmics
  hcosmicdataC3petimeslice2->SetStats(0);
  hcosmicdataC3petimeslice2->GetXaxis()->SetTitle("PE on C3");
  hcosmicdataC3petimeslice2->SetTitle("");
  hcosmicdataC3petimeslice2->Draw("hist");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice2.root");
  can->SaveAs("PNSAnaC3_minimumcut_cosmics_pe_timeslice2.pdf");

  // subtract cosmics from pns data
  THStack *stc3nocosmicpetslice2 = new THStack();
  stc3nocosmicpetslice2->SetMaximum(60000.);
  stc3nocosmicpetslice2->Add(hinactivebkgC3_pe_tslice2);
  stc3nocosmicpetslice2->Add(hinelasticbkgC3_pe_tslice2);
  stc3nocosmicpetslice2->Add(hsimsignalC3_pe_tslice2);
  stc3nocosmicpetslice2->Draw("hist");
  stc3nocosmicpetslice2->GetXaxis()->SetTitle("PE on C3");
  can->Modified();
  hpnsdataC3petimeslice2->Add(hcosmicdataC3petimeslice2, -1);
  hpnsdataC3petimeslice2->SetLineColor(1);
  hpnsdataC3petimeslice2->SetMarkerStyle(20);
  hpnsdataC3petimeslice2->SetMarkerSize(0.6);
  hpnsdataC3petimeslice2->Draw("E1 X0 SAME");
  TLegend *legc3nocosmictslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc3nocosmictslice2->SetTextSize(0.05);
  legc3nocosmictslice2->AddEntry(hinactivebkgC3_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmictslice2->AddEntry(hinelasticbkgC3_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmictslice2->AddEntry(hsimsignalC3_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc3nocosmictslice2->AddEntry(hpnsdataC3petimeslice2, "Data (cosmics subtracted)", "ep");
  legc3nocosmictslice2->SetBorderSize(0);
  legc3nocosmictslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice2.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_pe_timeslice2.pdf");
  // end C3 PE time slice 2



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
  std::cout << " C3 time cosmic only tot:   "             << hcosmicdataC3time->Integral(1, 112)         << std::endl;
  std::cout << " C3 time cosmic only PE<1200 tot:   "     << hcosmicdataC3timelowPE->Integral(1, 112)    << std::endl;
  std::cout << " C3 time cosmic only PE>1200 tot:   "     << hcosmicdataC3timehighPE->Integral(1, 112)   << std::endl;

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
  std::cout << " C3 time pns run tot:   "         << hpnsdataC3time->Integral(1, 112)        << std::endl;
  std::cout << " C3 time pns run PE<1200 tot:   " << hpnsdataC3timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C3 time pns run PE>1200 tot:   " << hpnsdataC3timehighPE->Integral(1, 112)  << std::endl;

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
  std::cout << " C3 time (pns - cosmic) tot:   "         << hpnsdataC3time->Integral(1, 112)        << std::endl;
  std::cout << " C3 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC3timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C3 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC3timehighPE->Integral(1, 112)  << std::endl;

  // JINST review comment: compare timing data - Fluka
  // subtract cosmics from pns data
  THStack *stc3nocosmictiming = new THStack();
  stc3nocosmictiming->SetMaximum(300.);
  hinactivebkgC3time->SetFillColor(4);
  hinelasticbkgC3time->SetFillColor(7);
  hsimsignalC3time->SetFillColor(6);
  stc3nocosmictiming->Add(hinactivebkgC3time);
  stc3nocosmictiming->Add(hinelasticbkgC3time);
  stc3nocosmictiming->Add(hsimsignalC3time);
  stc3nocosmictiming->Draw("hist");
  stc3nocosmictiming->GetXaxis()->SetTitle("Time (#mus) on C3");
  can->Modified();
  // modify data hist x axis scale to us
  nbins = hpnsdataC3time->GetXaxis()->GetNbins();
  xmin = hpnsdataC3time->GetXaxis()->GetXmin() * 0.016; // ticks x16ns/ tick / 1000
  xmax = hpnsdataC3time->GetXaxis()->GetXmax() * 0.016;
  // Create new histogram with scaled limits
  TH1F *hpnsdataC3time_new = new TH1F("hpnsdataC3time_new", "Rescaled Histogram", nbins, xmin, xmax);
  // Fill new histogram with original values
  for (int i = 1; i <= nbins; ++i) {
    hpnsdataC3time_new->SetBinContent(i, hpnsdataC3time->GetBinContent(i));
    hpnsdataC3time_new->SetBinError(i, hpnsdataC3time->GetBinError(i));
  }
  hpnsdataC3time_new->SetLineColor(1);
  hpnsdataC3time_new->SetMarkerStyle(20);
  hpnsdataC3time_new->SetMarkerSize(0.6);
  hpnsdataC3time_new->Draw("E1 X0 SAME");
  TLegend *legc3nocosmictiming = new TLegend(0.24,0.63,0.75,0.89);
  legc3nocosmictiming->SetTextSize(0.05);
  legc3nocosmictiming->AddEntry(hinactivebkgC3time, "All inactive materials neutron bkg (Fluka)", "f");
  legc3nocosmictiming->AddEntry(hinelasticbkgC3time, "Active LAr n inelastic (Fluka)", "f");
  legc3nocosmictiming->AddEntry(hsimsignalC3time, "Active LAr n capture (Fluka)", "f");
  legc3nocosmictiming->AddEntry(hpnsdataC3time_new, "Data (cosmics subtracted)", "ep");
  legc3nocosmictiming->SetBorderSize(0);
  legc3nocosmictiming->Draw();
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_stackMC.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic_timing_stackMC.pdf");

  // C2
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
  std::cout << " C2 data-cosmic PE total entries:   " << hpnsdataC2->Integral(1, 50) - hcosmicdataC2->Integral(1, 50) << std::endl;
  TLegend *legc2 = new TLegend(0.24,0.63,0.75,0.89);
  legc2->SetTextSize(0.05);
  legc2->AddEntry(hcosmicdataC2, "Cosmics (data driven)", "f");
  legc2->AddEntry(hinactivebkgC2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  legc2->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");
  legc2->AddEntry(hpnsdataC2, "Data", "ep");
  legc2->SetBorderSize(0);
  legc2->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_pe_logY.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut_pe.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe.pdf");

  // only plot cosmics
  hcosmicdataC2->SetStats(0);
  hcosmicdataC2->GetXaxis()->SetTitle("PE on C2");
  hcosmicdataC2->SetTitle("");
  hcosmicdataC2->Draw("hist");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe.pdf");

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

  TLegend *legc2nocosmic = new TLegend(0.24,0.63,0.75,0.89);
  legc2nocosmic->SetTextSize(0.05);
  legc2nocosmic->AddEntry(hinactivebkgC2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmic->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmic->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");
  legc2nocosmic->AddEntry(hpnsdataC2, "Data (cosmics subtracted)", "ep");
  legc2nocosmic->SetBorderSize(0);
  legc2nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe.pdf");

  // C2 PE time slice 0
  THStack *stc2petslice0 = new THStack();
  stc2petslice0->SetMaximum(100000.);
  hcosmicdataC2petimeslice0->SetFillColor(2);
  hinactivebkgC2_pe_tslice0->SetFillColor(4);
  hinelasticbkgC2_pe_tslice0->SetFillColor(7);
  hsimsignalC2_pe_tslice0->SetFillColor(6);
  stc2petslice0->Add(hcosmicdataC2petimeslice0);
  stc2petslice0->Add(hinactivebkgC2_pe_tslice0);
  stc2petslice0->Add(hinelasticbkgC2_pe_tslice0);
  stc2petslice0->Add(hsimsignalC2_pe_tslice0);
  stc2petslice0->Draw("hist");
  stc2petslice0->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice0->SetLineColor(1);
  hpnsdataC2petimeslice0->SetMarkerStyle(20);
  hpnsdataC2petimeslice0->SetMarkerSize(0.6);
  hpnsdataC2petimeslice0->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 0 *** " << std::endl;
  std::cout << " hcosmicdataC2:   " << hcosmicdataC2petimeslice0->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC2:  " << hinactivebkgC2_pe_tslice0->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC2: " << hinelasticbkgC2_pe_tslice0->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC2:    " << hsimsignalC2_pe_tslice0->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC2:      " << hpnsdataC2petimeslice0->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 0 *** " << std::endl;
  TLegend *legc2tslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc2tslice0->SetTextSize(0.05);
  legc2tslice0->AddEntry(hcosmicdataC2petimeslice0, "Cosmics (data driven)", "f");
  legc2tslice0->AddEntry(hinactivebkgC2_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc2tslice0->AddEntry(hinelasticbkgC2_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc2tslice0->AddEntry(hsimsignalC2_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc2tslice0->AddEntry(hpnsdataC2petimeslice0, "Data", "ep");
  legc2tslice0->SetBorderSize(0);
  legc2tslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice0_logY.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice0_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice0.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice0.pdf");

  // only plot cosmics
  hcosmicdataC2petimeslice0->SetStats(0);
  hcosmicdataC2petimeslice0->GetXaxis()->SetTitle("PE on C2");
  hcosmicdataC2petimeslice0->SetTitle("");
  hcosmicdataC2petimeslice0->Draw("hist");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice0.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice0.pdf");

  // subtract cosmics from pns data
  THStack *stc2nocosmicpetslice0 = new THStack();
  stc2nocosmicpetslice0->SetMaximum(60000.);
  stc2nocosmicpetslice0->Add(hinactivebkgC2_pe_tslice0);
  stc2nocosmicpetslice0->Add(hinelasticbkgC2_pe_tslice0);
  stc2nocosmicpetslice0->Add(hsimsignalC2_pe_tslice0);
  stc2nocosmicpetslice0->Draw("hist");
  stc2nocosmicpetslice0->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice0->Add(hcosmicdataC2petimeslice0, -1);
  hpnsdataC2petimeslice0->SetLineColor(1);
  hpnsdataC2petimeslice0->SetMarkerStyle(20);
  hpnsdataC2petimeslice0->SetMarkerSize(0.6);
  hpnsdataC2petimeslice0->Draw("E1 X0 SAME");
  TLegend *legc2nocosmictslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc2nocosmictslice0->SetTextSize(0.05);
  legc2nocosmictslice0->AddEntry(hinactivebkgC2_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmictslice0->AddEntry(hinelasticbkgC2_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmictslice0->AddEntry(hsimsignalC2_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc2nocosmictslice0->AddEntry(hpnsdataC2petimeslice0, "Data (cosmics subtracted)", "ep");
  legc2nocosmictslice0->SetBorderSize(0);
  legc2nocosmictslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice0.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice0.pdf");
  // end C2 PE time slice 0

  // begin C2 PE time slice 1
  THStack *stc2petslice1 = new THStack();
  stc2petslice1->SetMaximum(100000.);
  hcosmicdataC2petimeslice1->SetFillColor(2);
  hinactivebkgC2_pe_tslice1->SetFillColor(4);
  hinelasticbkgC2_pe_tslice1->SetFillColor(7);
  hsimsignalC2_pe_tslice1->SetFillColor(6);
  stc2petslice1->Add(hcosmicdataC2petimeslice1);
  stc2petslice1->Add(hinactivebkgC2_pe_tslice1);
  stc2petslice1->Add(hinelasticbkgC2_pe_tslice1);
  stc2petslice1->Add(hsimsignalC2_pe_tslice1);
  stc2petslice1->Draw("hist");
  stc2petslice1->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice1->SetLineColor(1);
  hpnsdataC2petimeslice1->SetMarkerStyle(20);
  hpnsdataC2petimeslice1->SetMarkerSize(0.6);
  hpnsdataC2petimeslice1->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 1 *** " << std::endl;
  std::cout << " hcosmicdataC2:   " << hcosmicdataC2petimeslice1->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC2:  " << hinactivebkgC2_pe_tslice1->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC2: " << hinelasticbkgC2_pe_tslice1->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC2:    " << hsimsignalC2_pe_tslice1->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC2:      " << hpnsdataC2petimeslice1->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 1 *** " << std::endl;
  TLegend *legc2tslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc2tslice1->SetTextSize(0.05);
  legc2tslice1->AddEntry(hcosmicdataC2petimeslice1, "Cosmics (data driven)", "f");
  legc2tslice1->AddEntry(hinactivebkgC2_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc2tslice1->AddEntry(hinelasticbkgC2_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc2tslice1->AddEntry(hsimsignalC2_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc2tslice1->AddEntry(hpnsdataC2petimeslice1, "Data", "ep");
  legc2tslice1->SetBorderSize(0);
  legc2tslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice1_logY.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice1_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice1.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice1.pdf");

  // only plot cosmics
  hcosmicdataC2petimeslice1->SetStats(0);
  hcosmicdataC2petimeslice1->GetXaxis()->SetTitle("PE on C2");
  hcosmicdataC2petimeslice1->SetTitle("");
  hcosmicdataC2petimeslice1->Draw("hist");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice1.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice1.pdf");

  // subtract cosmics from pns data
  THStack *stc2nocosmicpetslice1 = new THStack();
  stc2nocosmicpetslice1->SetMaximum(60000.);
  stc2nocosmicpetslice1->Add(hinactivebkgC2_pe_tslice1);
  stc2nocosmicpetslice1->Add(hinelasticbkgC2_pe_tslice1);
  stc2nocosmicpetslice1->Add(hsimsignalC2_pe_tslice1);
  stc2nocosmicpetslice1->Draw("hist");
  stc2nocosmicpetslice1->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice1->Add(hcosmicdataC2petimeslice1, -1);
  hpnsdataC2petimeslice1->SetLineColor(1);
  hpnsdataC2petimeslice1->SetMarkerStyle(20);
  hpnsdataC2petimeslice1->SetMarkerSize(0.6);
  hpnsdataC2petimeslice1->Draw("E1 X0 SAME");
  TLegend *legc2nocosmictslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc2nocosmictslice1->SetTextSize(0.05);
  legc2nocosmictslice1->AddEntry(hinactivebkgC2_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmictslice1->AddEntry(hinelasticbkgC2_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmictslice1->AddEntry(hsimsignalC2_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc2nocosmictslice1->AddEntry(hpnsdataC2petimeslice1, "Data (cosmics subtracted)", "ep");
  legc2nocosmictslice1->SetBorderSize(0);
  legc2nocosmictslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice1.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice1.pdf");
  // end C2 PE time slice 1

  // begin C2 PE time slice 2
  THStack *stc2petslice2 = new THStack();
  stc2petslice2->SetMaximum(100000.);
  hcosmicdataC2petimeslice2->SetFillColor(2);
  hinactivebkgC2_pe_tslice2->SetFillColor(4);
  hinelasticbkgC2_pe_tslice2->SetFillColor(7);
  hsimsignalC2_pe_tslice2->SetFillColor(6);
  stc2petslice2->Add(hcosmicdataC2petimeslice2);
  stc2petslice2->Add(hinactivebkgC2_pe_tslice2);
  stc2petslice2->Add(hinelasticbkgC2_pe_tslice2);
  stc2petslice2->Add(hsimsignalC2_pe_tslice2);
  stc2petslice2->Draw("hist");
  stc2petslice2->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice2->SetLineColor(1);
  hpnsdataC2petimeslice2->SetMarkerStyle(20);
  hpnsdataC2petimeslice2->SetMarkerSize(0.6);
  hpnsdataC2petimeslice2->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 2 *** " << std::endl;
  std::cout << " hcosmicdataC2:   " << hcosmicdataC2petimeslice2->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC2:  " << hinactivebkgC2_pe_tslice2->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC2: " << hinelasticbkgC2_pe_tslice2->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC2:    " << hsimsignalC2_pe_tslice2->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC2:      " << hpnsdataC2petimeslice2->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 2 *** " << std::endl;
  TLegend *legc2tslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc2tslice2->SetTextSize(0.05);
  legc2tslice2->AddEntry(hcosmicdataC2petimeslice2, "Cosmics (data driven)", "f");
  legc2tslice2->AddEntry(hinactivebkgC2_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2tslice2->AddEntry(hinelasticbkgC2_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc2tslice2->AddEntry(hsimsignalC2_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc2tslice2->AddEntry(hpnsdataC2petimeslice2, "Data", "ep");
  legc2tslice2->SetBorderSize(0);
  legc2tslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice2_logY.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice2_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice2.root");
  can->SaveAs("PNSAnaC2_minimumcut_pe_timeslice2.pdf");

  // only plot cosmics
  hcosmicdataC2petimeslice2->SetStats(0);
  hcosmicdataC2petimeslice2->GetXaxis()->SetTitle("PE on C2");
  hcosmicdataC2petimeslice2->SetTitle("");
  hcosmicdataC2petimeslice2->Draw("hist");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice2.root");
  can->SaveAs("PNSAnaC2_minimumcut_cosmics_pe_timeslice2.pdf");

  // subtract cosmics from pns data
  THStack *stc2nocosmicpetslice2 = new THStack();
  stc2nocosmicpetslice2->SetMaximum(60000.);
  stc2nocosmicpetslice2->Add(hinactivebkgC2_pe_tslice2);
  stc2nocosmicpetslice2->Add(hinelasticbkgC2_pe_tslice2);
  stc2nocosmicpetslice2->Add(hsimsignalC2_pe_tslice2);
  stc2nocosmicpetslice2->Draw("hist");
  stc2nocosmicpetslice2->GetXaxis()->SetTitle("PE on C2");
  can->Modified();
  hpnsdataC2petimeslice2->Add(hcosmicdataC2petimeslice2, -1);
  hpnsdataC2petimeslice2->SetLineColor(1);
  hpnsdataC2petimeslice2->SetMarkerStyle(20);
  hpnsdataC2petimeslice2->SetMarkerSize(0.6);
  hpnsdataC2petimeslice2->Draw("E1 X0 SAME");
  TLegend *legc2nocosmictslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc2nocosmictslice2->SetTextSize(0.05);
  legc2nocosmictslice2->AddEntry(hinactivebkgC2_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmictslice2->AddEntry(hinelasticbkgC2_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmictslice2->AddEntry(hsimsignalC2_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc2nocosmictslice2->AddEntry(hpnsdataC2petimeslice2, "Data (cosmics subtracted)", "ep");
  legc2nocosmictslice2->SetBorderSize(0);
  legc2nocosmictslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice2.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_pe_timeslice2.pdf");
  // end C2 PE time slice 2

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
  std::cout << " C2 time cosmic only tot:   "             << hcosmicdataC2time->Integral(1, 112)         << std::endl;
  std::cout << " C2 time cosmic only PE<1200 tot:   "     << hcosmicdataC2timelowPE->Integral(1, 112)    << std::endl;
  std::cout << " C2 time cosmic only PE>1200 tot:   "     << hcosmicdataC2timehighPE->Integral(1, 112)   << std::endl;

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
  std::cout << " C2 time pns run tot:   "         << hpnsdataC2time->Integral(1, 112)        << std::endl;
  std::cout << " C2 time pns run PE<1200 tot:   " << hpnsdataC2timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C2 time pns run PE>1200 tot:   " << hpnsdataC2timehighPE->Integral(1, 112)  << std::endl;

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
  std::cout << " C2 time (pns - cosmic) tot:   "         << hpnsdataC2time->Integral(1, 112)        << std::endl;
  std::cout << " C2 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC2timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C2 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC2timehighPE->Integral(1, 112)  << std::endl;

  // JINST review comment: compare timing data - Fluka
  // subtract cosmics from pns data
  THStack *stc2nocosmictiming = new THStack();
  stc2nocosmictiming->SetMaximum(270.);
  hinactivebkgC2time->SetFillColor(4);
  hinelasticbkgC2time->SetFillColor(7);
  hsimsignalC2time->SetFillColor(6);
  stc2nocosmictiming->Add(hinactivebkgC2time);
  stc2nocosmictiming->Add(hinelasticbkgC2time);
  stc2nocosmictiming->Add(hsimsignalC2time);
  stc2nocosmictiming->Draw("hist");
  stc2nocosmictiming->GetXaxis()->SetTitle("Time (#mus) on C2");
  can->Modified();
  // modify data hist x axis scale to us
  nbins = hpnsdataC2time->GetXaxis()->GetNbins();
  xmin = hpnsdataC2time->GetXaxis()->GetXmin() * 0.016; // ticks x16ns/ tick / 1000
  xmax = hpnsdataC2time->GetXaxis()->GetXmax() * 0.016;
  // Create new histogram with scaled limits
  TH1F *hpnsdataC2time_new = new TH1F("hpnsdataC2time_new", "Rescaled Histogram", nbins, xmin, xmax);
  // Fill new histogram with original values
  for (int i = 1; i <= nbins; ++i) {
    hpnsdataC2time_new->SetBinContent(i, hpnsdataC2time->GetBinContent(i));
    hpnsdataC2time_new->SetBinError(i, hpnsdataC2time->GetBinError(i));
  }
  hpnsdataC2time_new->SetLineColor(1);
  hpnsdataC2time_new->SetMarkerStyle(20);
  hpnsdataC2time_new->SetMarkerSize(0.6);
  hpnsdataC2time_new->Draw("E1 X0 SAME");
  TLegend *legc2nocosmictiming = new TLegend(0.24,0.63,0.75,0.89);
  legc2nocosmictiming->SetTextSize(0.05);
  legc2nocosmictiming->AddEntry(hinactivebkgC2time, "All inactive materials neutron bkg (Fluka)", "f");
  legc2nocosmictiming->AddEntry(hinelasticbkgC2time, "Active LAr n inelastic (Fluka)", "f");
  legc2nocosmictiming->AddEntry(hsimsignalC2time, "Active LAr n capture (Fluka)", "f");
  legc2nocosmictiming->AddEntry(hpnsdataC2time_new, "Data (cosmics subtracted)", "ep");
  legc2nocosmictiming->SetBorderSize(0);
  legc2nocosmictiming->Draw();
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_stackMC.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic_timing_stackMC.pdf");

  // C1
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
  std::cout << " C1 data-cosmic PE total entries:   " << hpnsdataC1->Integral(1, 50) - hcosmicdataC1->Integral(1, 50) << std::endl;

  TLegend *legc1 = new TLegend(0.24,0.63,0.75,0.89);
  legc1->SetTextSize(0.05);
  legc1->AddEntry(hcosmicdataC1, "Cosmics (data driven)", "f");
  legc1->AddEntry(hinactivebkgC1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  legc1->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");
  legc1->AddEntry(hpnsdataC1, "Data", "ep");
  legc1->SetBorderSize(0);
  legc1->Draw();

  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_pe_logY.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut_pe.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe.pdf");

  // only plot cosmics
  hcosmicdataC1->SetStats(0);
  hcosmicdataC1->GetXaxis()->SetTitle("PE on C1");
  hcosmicdataC1->SetTitle("");
  hcosmicdataC1->Draw("hist");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe.pdf");

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

  TLegend *legc1nocosmic = new TLegend(0.24,0.63,0.75,0.89);
  legc1nocosmic->SetTextSize(0.05);
  legc1nocosmic->AddEntry(hinactivebkgC1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmic->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmic->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");
  legc1nocosmic->AddEntry(hpnsdataC1, "Data (cosmics subtracted)", "ep");
  legc1nocosmic->SetBorderSize(0);
  legc1nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe.pdf");

  // C1 PE time slice 0
  THStack *stc1petslice0 = new THStack();
  stc1petslice0->SetMaximum(100000.);
  hcosmicdataC1petimeslice0->SetFillColor(2);
  hinactivebkgC1_pe_tslice0->SetFillColor(4);
  hinelasticbkgC1_pe_tslice0->SetFillColor(7);
  hsimsignalC1_pe_tslice0->SetFillColor(6);
  stc1petslice0->Add(hcosmicdataC1petimeslice0);
  stc1petslice0->Add(hinactivebkgC1_pe_tslice0);
  stc1petslice0->Add(hinelasticbkgC1_pe_tslice0);
  stc1petslice0->Add(hsimsignalC1_pe_tslice0);
  stc1petslice0->Draw("hist");
  stc1petslice0->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice0->SetLineColor(1);
  hpnsdataC1petimeslice0->SetMarkerStyle(20);
  hpnsdataC1petimeslice0->SetMarkerSize(0.6);
  hpnsdataC1petimeslice0->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 0 *** " << std::endl;
  std::cout << " hcosmicdataC1:   " << hcosmicdataC1petimeslice0->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC1:  " << hinactivebkgC1_pe_tslice0->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC1: " << hinelasticbkgC1_pe_tslice0->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC1:    " << hsimsignalC1_pe_tslice0->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC1:      " << hpnsdataC1petimeslice0->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 0 *** " << std::endl;
  TLegend *legc1tslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc1tslice0->SetTextSize(0.05);
  legc1tslice0->AddEntry(hcosmicdataC1petimeslice0, "Cosmics (data driven)", "f");
  legc1tslice0->AddEntry(hinactivebkgC1_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc1tslice0->AddEntry(hinelasticbkgC1_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc1tslice0->AddEntry(hsimsignalC1_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc1tslice0->AddEntry(hpnsdataC1petimeslice0, "Data", "ep");
  legc1tslice0->SetBorderSize(0);
  legc1tslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice0_logY.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice0_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice0.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice0.pdf");

  // only plot cosmics
  hcosmicdataC1petimeslice0->SetStats(0);
  hcosmicdataC1petimeslice0->GetXaxis()->SetTitle("PE on C1");
  hcosmicdataC1petimeslice0->SetTitle("");
  hcosmicdataC1petimeslice0->Draw("hist");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice0.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice0.pdf");

  // subtract cosmics from pns data
  THStack *stc1nocosmicpetslice0 = new THStack();
  stc1nocosmicpetslice0->SetMaximum(60000.);
  stc1nocosmicpetslice0->Add(hinactivebkgC1_pe_tslice0);
  stc1nocosmicpetslice0->Add(hinelasticbkgC1_pe_tslice0);
  stc1nocosmicpetslice0->Add(hsimsignalC1_pe_tslice0);
  stc1nocosmicpetslice0->Draw("hist");
  stc1nocosmicpetslice0->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice0->Add(hcosmicdataC1petimeslice0, -1);
  hpnsdataC1petimeslice0->SetLineColor(1);
  hpnsdataC1petimeslice0->SetMarkerStyle(20);
  hpnsdataC1petimeslice0->SetMarkerSize(0.6);
  hpnsdataC1petimeslice0->Draw("E1 X0 SAME");
  TLegend *legc1nocosmictslice0 = new TLegend(0.24,0.63,0.75,0.89);
  legc1nocosmictslice0->SetTextSize(0.05);
  legc1nocosmictslice0->AddEntry(hinactivebkgC1_pe_tslice0, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmictslice0->AddEntry(hinelasticbkgC1_pe_tslice0, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmictslice0->AddEntry(hsimsignalC1_pe_tslice0, "Active LAr n capture (Fluka)", "f");
  legc1nocosmictslice0->AddEntry(hpnsdataC1petimeslice0, "Data (cosmics subtracted)", "ep");
  legc1nocosmictslice0->SetBorderSize(0);
  legc1nocosmictslice0->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice0.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice0.pdf");
  // end C1 PE time slice 0

  // begin C1 PE time slice 1
  THStack *stc1petslice1 = new THStack();
  stc1petslice1->SetMaximum(100000.);
  hcosmicdataC1petimeslice1->SetFillColor(2);
  hinactivebkgC1_pe_tslice1->SetFillColor(4);
  hinelasticbkgC1_pe_tslice1->SetFillColor(7);
  hsimsignalC1_pe_tslice1->SetFillColor(6);
  stc1petslice1->Add(hcosmicdataC1petimeslice1);
  stc1petslice1->Add(hinactivebkgC1_pe_tslice1);
  stc1petslice1->Add(hinelasticbkgC1_pe_tslice1);
  stc1petslice1->Add(hsimsignalC1_pe_tslice1);
  stc1petslice1->Draw("hist");
  stc1petslice1->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice1->SetLineColor(1);
  hpnsdataC1petimeslice1->SetMarkerStyle(20);
  hpnsdataC1petimeslice1->SetMarkerSize(0.6);
  hpnsdataC1petimeslice1->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 1 *** " << std::endl;
  std::cout << " hcosmicdataC1:   " << hcosmicdataC1petimeslice1->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC1:  " << hinactivebkgC1_pe_tslice1->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC1: " << hinelasticbkgC1_pe_tslice1->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC1:    " << hsimsignalC1_pe_tslice1->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC1:      " << hpnsdataC1petimeslice1->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 1 *** " << std::endl;
  TLegend *legc1tslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc1tslice1->SetTextSize(0.05);
  legc1tslice1->AddEntry(hcosmicdataC1petimeslice1, "Cosmics (data driven)", "f");
  legc1tslice1->AddEntry(hinactivebkgC1_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1tslice1->AddEntry(hinelasticbkgC1_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc1tslice1->AddEntry(hsimsignalC1_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc1tslice1->AddEntry(hpnsdataC1petimeslice1, "Data", "ep");
  legc1tslice1->SetBorderSize(0);
  legc1tslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice1_logY.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice1_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice1.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice1.pdf");

  // only plot cosmics
  hcosmicdataC1petimeslice1->SetStats(0);
  hcosmicdataC1petimeslice1->GetXaxis()->SetTitle("PE on C1");
  hcosmicdataC1petimeslice1->SetTitle("");
  hcosmicdataC1petimeslice1->Draw("hist");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice1.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice1.pdf");

  // subtract cosmics from pns data
  THStack *stc1nocosmicpetslice1 = new THStack();
  stc1nocosmicpetslice1->SetMaximum(60000.);
  stc1nocosmicpetslice1->Add(hinactivebkgC1_pe_tslice1);
  stc1nocosmicpetslice1->Add(hinelasticbkgC1_pe_tslice1);
  stc1nocosmicpetslice1->Add(hsimsignalC1_pe_tslice1);
  stc1nocosmicpetslice1->Draw("hist");
  stc1nocosmicpetslice1->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice1->Add(hcosmicdataC1petimeslice1, -1);
  hpnsdataC1petimeslice1->SetLineColor(1);
  hpnsdataC1petimeslice1->SetMarkerStyle(20);
  hpnsdataC1petimeslice1->SetMarkerSize(0.6);
  hpnsdataC1petimeslice1->Draw("E1 X0 SAME");
  TLegend *legc1nocosmictslice1 = new TLegend(0.24,0.63,0.75,0.89);
  legc1nocosmictslice1->SetTextSize(0.05);
  legc1nocosmictslice1->AddEntry(hinactivebkgC1_pe_tslice1, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmictslice1->AddEntry(hinelasticbkgC1_pe_tslice1, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmictslice1->AddEntry(hsimsignalC1_pe_tslice1, "Active LAr n capture (Fluka)", "f");
  legc1nocosmictslice1->AddEntry(hpnsdataC1petimeslice1, "Data (cosmics subtracted)", "ep");
  legc1nocosmictslice1->SetBorderSize(0);
  legc1nocosmictslice1->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice1.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice1.pdf");
  // end C1 PE time slice 1

  // begin C1 PE time slice 2
  THStack *stc1petslice2 = new THStack();
  stc1petslice2->SetMaximum(100000.);
  hcosmicdataC1petimeslice2->SetFillColor(2);
  hinactivebkgC1_pe_tslice2->SetFillColor(4);
  hinelasticbkgC1_pe_tslice2->SetFillColor(7);
  hsimsignalC1_pe_tslice2->SetFillColor(6);
  stc1petslice2->Add(hcosmicdataC1petimeslice2);
  stc1petslice2->Add(hinactivebkgC1_pe_tslice2);
  stc1petslice2->Add(hinelasticbkgC1_pe_tslice2);
  stc1petslice2->Add(hsimsignalC1_pe_tslice2);
  stc1petslice2->Draw("hist");
  stc1petslice2->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice2->SetLineColor(1);
  hpnsdataC1petimeslice2->SetMarkerStyle(20);
  hpnsdataC1petimeslice2->SetMarkerSize(0.6);
  hpnsdataC1petimeslice2->Draw("E1 X0 SAME");
  std::cout << "*** Time Slice 2 *** " << std::endl;
  std::cout << " hcosmicdataC1:   " << hcosmicdataC1petimeslice2->Integral(1, 50)   << std::endl;
  std::cout << " hinactivebkgC1:  " << hinactivebkgC1_pe_tslice2->Integral(1, 50)  << std::endl;
  std::cout << " hinelasticbkgC1: " << hinelasticbkgC1_pe_tslice2->Integral(1, 50) << std::endl;
  std::cout << " hsimsignalC1:    " << hsimsignalC1_pe_tslice2->Integral(1, 50)    << std::endl;
  std::cout << " hpnsdataC1:      " << hpnsdataC1petimeslice2->Integral(1, 50)     << std::endl;
  std::cout << "*** Time Slice 2 *** " << std::endl;
  TLegend *legc1tslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc1tslice2->SetTextSize(0.05);
  legc1tslice2->AddEntry(hcosmicdataC1petimeslice2, "Cosmics (data driven)", "f");
  legc1tslice2->AddEntry(hinactivebkgC1_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc1tslice2->AddEntry(hinelasticbkgC1_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc1tslice2->AddEntry(hsimsignalC1_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc1tslice2->AddEntry(hpnsdataC1petimeslice2, "Data", "ep");
  legc1tslice2->SetBorderSize(0);
  legc1tslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice2_logY.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice2_logY.pdf");
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice2.root");
  can->SaveAs("PNSAnaC1_minimumcut_pe_timeslice2.pdf");

  // only plot cosmics
  hcosmicdataC1petimeslice2->SetStats(0);
  hcosmicdataC1petimeslice2->GetXaxis()->SetTitle("PE on C1");
  hcosmicdataC1petimeslice2->SetTitle("");
  hcosmicdataC1petimeslice2->Draw("hist");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice2.root");
  can->SaveAs("PNSAnaC1_minimumcut_cosmics_pe_timeslice2.pdf");

  // subtract cosmics from pns data
  THStack *stc1nocosmicpetslice2 = new THStack();
  stc1nocosmicpetslice2->SetMaximum(60000.);
  stc1nocosmicpetslice2->Add(hinactivebkgC1_pe_tslice2);
  stc1nocosmicpetslice2->Add(hinelasticbkgC1_pe_tslice2);
  stc1nocosmicpetslice2->Add(hsimsignalC1_pe_tslice2);
  stc1nocosmicpetslice2->Draw("hist");
  stc1nocosmicpetslice2->GetXaxis()->SetTitle("PE on C1");
  can->Modified();
  hpnsdataC1petimeslice2->Add(hcosmicdataC1petimeslice2, -1);
  hpnsdataC1petimeslice2->SetLineColor(1);
  hpnsdataC1petimeslice2->SetMarkerStyle(20);
  hpnsdataC1petimeslice2->SetMarkerSize(0.6);
  hpnsdataC1petimeslice2->Draw("E1 X0 SAME");
  TLegend *legc1nocosmictslice2 = new TLegend(0.24,0.63,0.75,0.89);
  legc1nocosmictslice2->SetTextSize(0.05);
  legc1nocosmictslice2->AddEntry(hinactivebkgC1_pe_tslice2, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmictslice2->AddEntry(hinelasticbkgC1_pe_tslice2, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmictslice2->AddEntry(hsimsignalC1_pe_tslice2, "Active LAr n capture (Fluka)", "f");
  legc1nocosmictslice2->AddEntry(hpnsdataC1petimeslice2, "Data (cosmics subtracted)", "ep");
  legc1nocosmictslice2->SetBorderSize(0);
  legc1nocosmictslice2->Draw();
  gPad->SetLogy();
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice2.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_pe_timeslice2.pdf");
  // end C1 PE time slice 2

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
  std::cout << " C1 time cosmic only tot:   "             << hcosmicdataC1time->Integral(1, 112)         << std::endl;
  std::cout << " C1 time cosmic only PE<1200 tot:   "     << hcosmicdataC1timelowPE->Integral(1, 112)    << std::endl;
  std::cout << " C1 time cosmic only PE>1200 tot:   "     << hcosmicdataC1timehighPE->Integral(1, 112)   << std::endl;

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
  std::cout << " C1 time pns run tot:   "         << hpnsdataC1time->Integral(1, 112)        << std::endl;
  std::cout << " C1 time pns run PE<1200 tot:   " << hpnsdataC1timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C1 time pns run PE>1200 tot:   " << hpnsdataC1timehighPE->Integral(1, 112)  << std::endl;

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
  std::cout << " C1 time (pns - cosmic) tot:   "         << hpnsdataC1time->Integral(1, 112)        << std::endl;
  std::cout << " C1 time (pns - cosmic) PE<1200 tot:   " << hpnsdataC1timelowPE->Integral(1, 112)   << std::endl;
  std::cout << " C1 time (pns - cosmic) PE>1200 tot:   " << hpnsdataC1timehighPE->Integral(1, 112)  << std::endl;

  // JINST review comment: compare timing data - Fluka
  // subtract cosmics from pns data
  THStack *stc1nocosmictiming = new THStack();
  stc1nocosmictiming->SetMaximum(1100.);
  hinactivebkgC1time->SetFillColor(4);
  hinelasticbkgC1time->SetFillColor(7);
  hsimsignalC1time->SetFillColor(6);
  stc1nocosmictiming->Add(hinactivebkgC1time);
  stc1nocosmictiming->Add(hinelasticbkgC1time);
  stc1nocosmictiming->Add(hsimsignalC1time);
  stc1nocosmictiming->Draw("hist");
  stc1nocosmictiming->GetXaxis()->SetTitle("Time (#mus) on C1");
  can->Modified();
  // modify data hist x axis scale to us
  nbins = hpnsdataC1time->GetXaxis()->GetNbins();
  xmin = hpnsdataC1time->GetXaxis()->GetXmin() * 0.016; // ticks x16ns/ tick / 1000
  xmax = hpnsdataC1time->GetXaxis()->GetXmax() * 0.016;
  // Create new histogram with scaled limits
  TH1F *hpnsdataC1time_new = new TH1F("hpnsdataC1time_new", "Rescaled Histogram", nbins, xmin, xmax);
  // Fill new histogram with original values
  for (int i = 1; i <= nbins; ++i) {
    hpnsdataC1time_new->SetBinContent(i, hpnsdataC1time->GetBinContent(i));
    hpnsdataC1time_new->SetBinError(i, hpnsdataC1time->GetBinError(i));
  }
  hpnsdataC1time_new->SetLineColor(1);
  hpnsdataC1time_new->SetMarkerStyle(20);
  hpnsdataC1time_new->SetMarkerSize(0.6);
  hpnsdataC1time_new->Draw("E1 X0 SAME");
  TLegend *legc1nocosmictiming = new TLegend(0.24,0.63,0.75,0.89);
  legc1nocosmictiming->SetTextSize(0.05);
  legc1nocosmictiming->AddEntry(hinactivebkgC1time, "All inactive materials neutron bkg (Fluka)", "f");
  legc1nocosmictiming->AddEntry(hinelasticbkgC1time, "Active LAr n inelastic (Fluka)", "f");
  legc1nocosmictiming->AddEntry(hsimsignalC1time, "Active LAr n capture (Fluka)", "f");
  legc1nocosmictiming->AddEntry(hpnsdataC1time_new, "Data (cosmics subtracted)", "ep");
  legc1nocosmictiming->SetBorderSize(0);
  legc1nocosmictiming->Draw();
  gPad->SetLogy(0);
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_stackMC.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic_timing_stackMC.pdf");

}
