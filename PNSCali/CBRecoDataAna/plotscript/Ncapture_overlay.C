// root -l -b -q  Ncapture_overlay.C

void Ncapture_overlay()
{
  bool useG4 = false;
  //double FlukaMCsf = 1.3382; // normalize C1
  //double FlukaMCsf = 0.366; // normalize C2
  //double FlukaMCsf = 0.2672; // normalize C3
  //double FlukaMCsf = 0.6687; // normalize C3
  double FlukaMCsf = 2.3564988; // normalize C3
  //double FlukaMCsf = 0.544; // normalize C4
  int C1Flukacapevts = 3624;
  int C2Flukacapevts = 825;
  int C3Flukacapevts = 976;
  int C4Flukacapevts = 8300;
  double cosmicDatasf = 0.642;

  TH1 *hpnsdataC4;
  TH1 *hpnsdataC3;
  TH1 *hpnsdataC2;
  TH1 *hpnsdataC1;
  TH1 *hcosmicdataC4;
  TH1 *hcosmicdataC3;
  TH1 *hcosmicdataC2;
  TH1 *hcosmicdataC1;

  TCanvas *c1 = new TCanvas();
  c1->cd();

  // Data after selection
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut.root");
  //TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib.root");
  TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071_minimumcut_adc2pepluspde_cali_ajib_min50PE.root");

  TTree *myMatchedC4Peaks_pns = (TTree*)file0->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_pns->Draw("C4_matched_PDPeak_PE>>hpnsdataC4(50, 100, 2100)");
  hpnsdataC4 = (TH1*)gPad->GetPrimitive("hpnsdataC4");
  TTree *myMatchedC3Peaks_pns = (TTree*)file0->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_pns->Draw("C3_matched_PDPeak_PE>>hpnsdataC3(50, 100, 2100)");
  hpnsdataC3 = (TH1*)gPad->GetPrimitive("hpnsdataC3");
  TTree *myMatchedC2Peaks_pns = (TTree*)file0->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_pns->Draw("C2_matched_PDPeak_PE>>hpnsdataC2(50, 100, 2100)");
  hpnsdataC2 = (TH1*)gPad->GetPrimitive("hpnsdataC2");
  TTree *myMatchedC1Peaks_pns = (TTree*)file0->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_pns->Draw("C1_matched_PDPeak_PE>>hpnsdataC1(50, 100, 2100)");
  hpnsdataC1 = (TH1*)gPad->GetPrimitive("hpnsdataC1");

  // cosmic bkg: data driven
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut.root");
  //TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib.root");
  TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086_minimumcut_adc2pepluspde_cali_ajib_min50PE.root");

  TTree *myMatchedC4Peaks_cosmic = (TTree*)file1->Get("myMatchedC4Peaks");
  myMatchedC4Peaks_cosmic->Draw("C4_matched_PDPeak_PE>>hcosmicdataC4(50, 100, 2100)");
  hcosmicdataC4 = (TH1*)gPad->GetPrimitive("hcosmicdataC4");
  hcosmicdataC4->Scale(cosmicDatasf);
  TTree *myMatchedC3Peaks_cosmic = (TTree*)file1->Get("myMatchedC3Peaks");
  myMatchedC3Peaks_cosmic->Draw("C3_matched_PDPeak_PE>>hcosmicdataC3(50, 100, 2100)");
  hcosmicdataC3 = (TH1*)gPad->GetPrimitive("hcosmicdataC3");
  hcosmicdataC3->Scale(cosmicDatasf);
  TTree *myMatchedC2Peaks_cosmic = (TTree*)file1->Get("myMatchedC2Peaks");
  myMatchedC2Peaks_cosmic->Draw("C2_matched_PDPeak_PE>>hcosmicdataC2(50, 100, 2100)");
  hcosmicdataC2 = (TH1*)gPad->GetPrimitive("hcosmicdataC2");
  hcosmicdataC2->Scale(cosmicDatasf);
  TTree *myMatchedC1Peaks_cosmic = (TTree*)file1->Get("myMatchedC1Peaks");
  myMatchedC1Peaks_cosmic->Draw("C1_matched_PDPeak_PE>>hcosmicdataC1(50, 100, 2100)");
  hcosmicdataC1 = (TH1*)gPad->GetPrimitive("hcosmicdataC1");
  hcosmicdataC1->Scale(cosmicDatasf);


  // Fluka simulated inactive region bkg
  // 20 million sim neutrons/events
  // Recorded events correspond to about 1.5% of generated neutrons.
  // closest to PNS is 2
  // furthest to PNS is 1
  //TFile *file3 = TFile::Open("vbox_sep24_pe.root");
  TFile *file3 = TFile::Open("vbox_sep24_pe_minimumcut.root");
  TCanvas *c04 = (TCanvas*)file3->Get("c04");
  TH1F *hinactivebkgC4 = (TH1F*)c04->GetPrimitive("petotothC4");
  hinactivebkgC4->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC4 = (TH1F*)c04->GetPrimitive("petotineC4");
  hinelasticbkgC4->Scale(FlukaMCsf);

  TCanvas *c03 = (TCanvas*)file3->Get("c03");
  TH1F *hinactivebkgC3 = (TH1F*)c03->GetPrimitive("petotothC3");
  hinactivebkgC3->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC3 = (TH1F*)c03->GetPrimitive("petotineC3");
  hinelasticbkgC3->Scale(FlukaMCsf);

  TCanvas *c02 = (TCanvas*)file3->Get("c02");
  TH1F *hinactivebkgC2 = (TH1F*)c02->GetPrimitive("petotothC2");
  hinactivebkgC2->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC2 = (TH1F*)c02->GetPrimitive("petotineC2");
  hinelasticbkgC2->Scale(FlukaMCsf);

  TCanvas *c01 = (TCanvas*)file3->Get("c01");
  TH1F *hinactivebkgC1 = (TH1F*)c01->GetPrimitive("petotothC1");
  hinactivebkgC1->Scale(FlukaMCsf);
  TH1F *hinelasticbkgC1 = (TH1F*)c01->GetPrimitive("petotineC1");
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
  if (useG4) {
    TFile *file2 = TFile::Open("Plots_1M_PNSside_job84035104.root");
    TTree *myTree = (TTree*)file2->Get("myTree");
    int C4G4capevts = myTree->GetEntries("XA2Det_CaponXA2top_drift5_20_yz_abs_30 > 0");
    // tot cap on each module need to scale relative to Fluka’s sim
    double G4MCcapsfC4 = FlukaMCsf*C4Flukacapevts/C4G4capevts;
    std::cout << " C4G4cap: " << C4G4capevts << std::endl;
    myTree->Draw("XA2Det_CaponXA2top_drift5_20_yz_abs_30>>hsimsignalC4(50, 100, 2100)"); // Closest to PNS is ch5, correspond to C4 in CB
    hsimsignalC4 = (TH1*)gPad->GetPrimitive("hsimsignalC4");
    hsimsignalC4->Scale(G4MCcapsfC4);

    int C3G4capevts = myTree->GetEntries("XA4Det_CaponXA4top_drift5_20_yz_abs_30 > 0");
    double G4MCcapsfC3 = FlukaMCsf*C3Flukacapevts/C3G4capevts;
    std::cout << " C3G4cap: " << C3G4capevts << std::endl;
    myTree->Draw("XA4Det_CaponXA4top_drift5_20_yz_abs_30>>hsimsignalC3(50, 100, 2100)");
    hsimsignalC3 = (TH1*)gPad->GetPrimitive("hsimsignalC3");
    hsimsignalC3->Scale(G4MCcapsfC3);

    int C2G4capevts = myTree->GetEntries("XA5Det_CaponXA5top_drift5_20_yz_abs_30 > 0");
    double G4MCcapsfC2 = FlukaMCsf*C2Flukacapevts/C2G4capevts;
    std::cout << " C2G4cap: " << C2G4capevts << std::endl;
    myTree->Draw("XA5Det_CaponXA5top_drift5_20_yz_abs_30>>hsimsignalC2(50, 100, 2100)"); // Furthest to PNS is ch5, correspond to C2 in CB
    hsimsignalC2 = (TH1*)gPad->GetPrimitive("hsimsignalC2");
    hsimsignalC2->Scale(G4MCcapsfC2);

    int C1G4capevts = myTree->GetEntries("XA3Det_CaponXA3top_drift5_20_yz_abs_30 > 0");
    double G4MCcapsfC1 = FlukaMCsf*C1Flukacapevts/C1G4capevts;
    std::cout << " C1G4cap: " << C1G4capevts << std::endl;
    myTree->Draw("XA3Det_CaponXA3top_drift5_20_yz_abs_30>>hsimsignalC1(50, 100, 2100)");
    hsimsignalC1 = (TH1*)gPad->GetPrimitive("hsimsignalC1");
    hsimsignalC1->Scale(G4MCcapsfC1);
  }
  else{
    // Fluka sim PNS signal
    // same fluka scale factor
    //hsimsignalC4 = (TH1F*)c04->GetPrimitive("petotcapontileC4");
    hsimsignalC4 = (TH1F*)c04->GetPrimitive("petotcapC4");
    hsimsignalC4->Scale(FlukaMCsf);
    //hsimsignalC3 = (TH1F*)c03->GetPrimitive("petotcapontileC3");
    hsimsignalC3 = (TH1F*)c03->GetPrimitive("petotcapC3");
    hsimsignalC3->Scale(FlukaMCsf);
    //hsimsignalC2 = (TH1F*)c02->GetPrimitive("petotcapontileC2");
    hsimsignalC2 = (TH1F*)c02->GetPrimitive("petotcapC2");
    hsimsignalC2->Scale(FlukaMCsf);
    //hsimsignalC1 = (TH1F*)c01->GetPrimitive("petotcapontileC1");
    hsimsignalC1 = (TH1F*)c01->GetPrimitive("petotcapC1");
    hsimsignalC1->Scale(FlukaMCsf);
  }


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
  legc4->AddEntry(hinactivebkgC4, "Inactive LAr neutron bkg (Fluka)", "f");
  legc4->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc4->AddEntry(hsimsignalC4, "capture signal (G4)", "f");}
  else {legc4->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");}
  legc4->AddEntry(hpnsdataC4, "Data", "ep");
  legc4->SetBorderSize(0);
  legc4->Draw();

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

  TLegend *legc4nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc4nocosmic->SetTextSize(0.05);
  legc4nocosmic->AddEntry(hinactivebkgC4, "Inactive LAr neutron bkg (Fluka)", "f");
  legc4nocosmic->AddEntry(hinelasticbkgC4, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc4nocosmic->AddEntry(hsimsignalC4, "capture signal (G4)", "f");}
  else {legc4nocosmic->AddEntry(hsimsignalC4, "Active LAr n capture (Fluka)", "f");}
  legc4nocosmic->AddEntry(hpnsdataC4, "Data (cosmics subtracted)", "ep");
  legc4nocosmic->SetBorderSize(0);
  legc4nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC4_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC4_minimumcut_nocosmic.pdf");




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
  legc3->AddEntry(hinactivebkgC3, "Inactive LAr neutron bkg (Fluka)", "f");
  legc3->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc3->AddEntry(hsimsignalC3, "n capture signal active LAr (G4)", "f");}
  else {legc3->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");}
  legc3->AddEntry(hpnsdataC3, "Data", "ep");
  legc3->SetBorderSize(0);
  legc3->Draw();

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

  TLegend *legc3nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc3nocosmic->SetTextSize(0.05);
  legc3nocosmic->AddEntry(hinactivebkgC3, "Inactive LAr neutron bkg (Fluka)", "f");
  legc3nocosmic->AddEntry(hinelasticbkgC3, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc3nocosmic->AddEntry(hsimsignalC3, "capture signal (G4)", "f");}
  else {legc3nocosmic->AddEntry(hsimsignalC3, "Active LAr n capture (Fluka)", "f");}
  legc3nocosmic->AddEntry(hpnsdataC3, "Data (cosmics subtracted)", "ep");
  legc3nocosmic->SetBorderSize(0);
  legc3nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC3_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC3_minimumcut_nocosmic.pdf");



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
  legc2->AddEntry(hinactivebkgC2, "Inactive LAr neutron bkg (Fluka)", "f");
  legc2->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc2->AddEntry(hsimsignalC2, "capture signal (G4)", "f");}
  else{legc2->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");}
  legc2->AddEntry(hpnsdataC2, "Data", "ep");
  legc2->SetBorderSize(0);
  legc2->Draw();

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

  TLegend *legc2nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc2nocosmic->SetTextSize(0.05);
  legc2nocosmic->AddEntry(hinactivebkgC2, "Inactive LAr neutron bkg (Fluka)", "f");
  legc2nocosmic->AddEntry(hinelasticbkgC2, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc2nocosmic->AddEntry(hsimsignalC2, "capture signal (G4)", "f");}
  else {legc2nocosmic->AddEntry(hsimsignalC2, "Active LAr n capture (Fluka)", "f");}
  legc2nocosmic->AddEntry(hpnsdataC2, "Data (cosmics subtracted)", "ep");
  legc2nocosmic->SetBorderSize(0);
  legc2nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC2_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC2_minimumcut_nocosmic.pdf");



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
  legc1->AddEntry(hinactivebkgC1, "Inactive LAr neutron bkg (Fluka)", "f");
  legc1->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc1->AddEntry(hsimsignalC1, "capture signal (G4)", "f");}
  else{legc1->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");}
  legc1->AddEntry(hpnsdataC1, "Data", "ep");
  legc1->SetBorderSize(0);
  legc1->Draw();

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

  TLegend *legc1nocosmic = new TLegend(0.24,0.63,0.89,0.89);
  legc1nocosmic->SetTextSize(0.05);
  legc1nocosmic->AddEntry(hinactivebkgC1, "Inactive LAr neutron bkg (Fluka)", "f");
  legc1nocosmic->AddEntry(hinelasticbkgC1, "Active LAr n inelastic (Fluka)", "f");
  if (useG4) {legc1nocosmic->AddEntry(hsimsignalC1, "capture signal (G4)", "f");}
  else {legc1nocosmic->AddEntry(hsimsignalC1, "Active LAr n capture (Fluka)", "f");}
  legc1nocosmic->AddEntry(hpnsdataC1, "Data (cosmics subtracted)", "ep");
  legc1nocosmic->SetBorderSize(0);
  legc1nocosmic->Draw();

  gPad->SetLogy();

  can->SaveAs("PNSAnaC1_minimumcut_nocosmic.root");
  can->SaveAs("PNSAnaC1_minimumcut_nocosmic.pdf");

}
