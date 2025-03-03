// root -l -b -q  Ncapture_overlay.C

void Ncapture_overlay()
{
  bool useG4 = true;
  double FlukaMCsf = 0.023; // only tune this
  int totFlukaevts = 3628574;
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

  // pns run data after selection
  TFile *file0 = TFile::Open("PNS_runs_25036_25068_25071.root");

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
  TFile *file1 = TFile::Open("Cosmic_runs_25004_25066_25078_25084_25086.root");

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
  TFile *file3 = TFile::Open("vbox_sep24_pe.root");
  TCanvas *c04 = (TCanvas*)file3->Get("c04");
  TH1F *hinactivebkgC4 = (TH1F*)c04->GetPrimitive("petotothC4");
  hinactivebkgC4->Scale(FlukaMCsf);
  TCanvas *c03 = (TCanvas*)file3->Get("c03");
  TH1F *hinactivebkgC3 = (TH1F*)c03->GetPrimitive("petotothC3");
  hinactivebkgC3->Scale(FlukaMCsf);
  TCanvas *c02 = (TCanvas*)file3->Get("c02");
  TH1F *hinactivebkgC2 = (TH1F*)c02->GetPrimitive("petotothC2");
  hinactivebkgC2->Scale(FlukaMCsf);
  TCanvas *c01 = (TCanvas*)file3->Get("c01");
  TH1F *hinactivebkgC1 = (TH1F*)c01->GetPrimitive("petotothC1");
  hinactivebkgC1->Scale(FlukaMCsf);

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
    hsimsignalC4 = (TH1F*)c04->GetPrimitive("petotcapC4");
    hsimsignalC4->Scale(FlukaMCsf);
    hsimsignalC3 = (TH1F*)c03->GetPrimitive("petotcapC3");
    hsimsignalC3->Scale(FlukaMCsf);
    hsimsignalC2 = (TH1F*)c02->GetPrimitive("petotcapC2");
    hsimsignalC2->Scale(FlukaMCsf);
    hsimsignalC1 = (TH1F*)c01->GetPrimitive("petotcapC1");
    hsimsignalC1->Scale(FlukaMCsf);
  }


  TCanvas *can = new TCanvas();
  can->cd();

  THStack *stc4 = new THStack();
  stc4->SetMaximum(400.);
  //hcosmicdataC4->SetLineColorAlpha(2, 0);
  hcosmicdataC4->SetFillColor(2);
  //hinactivebkgC4->SetLineColorAlpha(4, 0);
  hinactivebkgC4->SetFillColor(4);
  //hsimsignalC4->SetLineColorAlpha(6, 0);
  hsimsignalC4->SetFillColor(6);
  stc4->Add(hcosmicdataC4);
  stc4->Add(hinactivebkgC4);
  stc4->Add(hsimsignalC4);
  stc4->Draw("hist");

  hpnsdataC4->SetLineColor(1);
  hpnsdataC4->SetMarkerStyle(20);
  hpnsdataC4->SetMarkerSize(0.6);
  hpnsdataC4->Draw("E1 X0 SAME");

  TLegend *legc4 = new TLegend(0.34,0.58,0.78,0.82);
  legc4->SetTextSize(0.05);
  legc4->AddEntry(hcosmicdataC4, "bkg in cosmic run (data driven)", "f");
  legc4->AddEntry(hinactivebkgC4, "neutron bkg outside active LAr (Fluka)", "f");
  if (useG4) {legc4->AddEntry(hsimsignalC4, "capture signal (G4)", "f");}
  else {legc4->AddEntry(hsimsignalC4, "capture signal (Fluka)", "f");}
  legc4->AddEntry(hpnsdataC4, "pns run data", "ep");
  legc4->Draw();

  can->SaveAs("PNSAnaC4.root");
  can->SaveAs("PNSAnaC4.pdf");

  THStack *stc3 = new THStack();
  stc3->SetMaximum(50.);
  hcosmicdataC3->SetFillColor(2);
  hinactivebkgC3->SetFillColor(4);
  hsimsignalC3->SetFillColor(6);
  stc3->Add(hcosmicdataC3);
  stc3->Add(hinactivebkgC3);
  stc3->Add(hsimsignalC3);
  stc3->Draw("hist");

  hpnsdataC3->SetLineColor(1);
  hpnsdataC3->SetMarkerStyle(20);
  hpnsdataC3->SetMarkerSize(0.6);
  hpnsdataC3->Draw("E1 X0 SAME");

  TLegend *legc3 = new TLegend(0.34,0.58,0.78,0.82);
  legc3->SetTextSize(0.05);
  legc3->AddEntry(hcosmicdataC3, "bkg in cosmic run (data driven)", "f");
  legc3->AddEntry(hinactivebkgC3, "neutron bkg outside active LAr (Fluka)", "f");
  if (useG4) {legc3->AddEntry(hsimsignalC3, "capture signal (G4)", "f");}
  else {legc3->AddEntry(hsimsignalC3, "capture signal (Fluka)", "f");}

  legc3->AddEntry(hpnsdataC3, "pns run data", "ep");
  legc3->Draw();

  can->SaveAs("PNSAnaC3.root");
  can->SaveAs("PNSAnaC3.pdf");

  THStack *stc2 = new THStack();
  stc2->SetMaximum(50.);
  hcosmicdataC2->SetFillColor(2);
  hinactivebkgC2->SetFillColor(4);
  hsimsignalC2->SetFillColor(6);
  stc2->Add(hcosmicdataC2);
  stc2->Add(hinactivebkgC2);
  stc2->Add(hsimsignalC2);
  stc2->Draw("hist");

  hpnsdataC2->SetLineColor(1);
  hpnsdataC2->SetMarkerStyle(20);
  hpnsdataC2->SetMarkerSize(0.6);
  hpnsdataC2->Draw("E1 X0 SAME");

  TLegend *legc2 = new TLegend(0.34,0.58,0.78,0.82);
  legc2->SetTextSize(0.05);
  legc2->AddEntry(hcosmicdataC2, "bkg in cosmic run (data driven)", "f");
  legc2->AddEntry(hinactivebkgC2, "neutron bkg outside active LAr (Fluka)", "f");
  if (useG4) {legc2->AddEntry(hsimsignalC2, "capture signal (G4)", "f");}
  else{legc2->AddEntry(hsimsignalC2, "capture signal (Fluka)", "f");}
  legc2->AddEntry(hpnsdataC2, "pns run data", "ep");
  legc2->Draw();

  can->SaveAs("PNSAnaC2.root");
  can->SaveAs("PNSAnaC2.pdf");

  THStack *stc1 = new THStack();
  stc1->SetMaximum(300.);
  hcosmicdataC1->SetFillColor(2);
  hinactivebkgC1->SetFillColor(4);
  hsimsignalC1->SetFillColor(6);
  stc1->Add(hcosmicdataC1);
  stc1->Add(hinactivebkgC1);
  stc1->Add(hsimsignalC1);
  stc1->Draw("hist");

  hpnsdataC1->SetLineColor(1);
  hpnsdataC1->SetMarkerStyle(20);
  hpnsdataC1->SetMarkerSize(0.6);
  hpnsdataC1->Draw("E1 X0 SAME");

  TLegend *legc1 = new TLegend(0.34,0.58,0.78,0.82);
  legc1->SetTextSize(0.05);
  legc1->AddEntry(hcosmicdataC1, "bkg in cosmic run (data driven)", "f");
  legc1->AddEntry(hinactivebkgC1, "neutron bkg outside active LAr (Fluka)", "f");
  if (useG4) {legc1->AddEntry(hsimsignalC1, "capture signal (G4)", "f");}
  else{legc1->AddEntry(hsimsignalC1, "capture signal (Fluka)", "f");}
  legc1->AddEntry(hpnsdataC1, "pns run data", "ep");
  legc1->Draw();

  can->SaveAs("PNSAnaC1.root");
  can->SaveAs("PNSAnaC1.pdf");

}
