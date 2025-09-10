//
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q Quickfit.C
//

#include "TFile.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TH1.h"
#include "TH3.h"
#include "TCanvas.h"

void Quickfit() {

  int nKEe  = 12; // electron
  int nKEpi = 4; // pion
  int nKEp  = 3; // proton
  //int nKEk = 4; // kaon
  double KEe[12] = {0.1995, 0.2995, 0.4995, 0.6995, 0.9995, 1.4995, 1.9995, 2.4995, 2.9995, 3.9995, 4.9995, 7.9995};
  // histogram mean and std
  //double PEe[12] = {11156.2, 11541.7, 15850.8, 24511.1, 37191.5, 68592.5, 100927.6, 126610.0, 153666.8, 204283.3, 246124.7, 300488.4};
  double KEerre[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //double PEerre[12] = {27900.5, 27416.9, 29705.5, 31926.8, 39357.3, 52381.3, 57187.3, 62560.4, 56642.1, 55903.4, 62083.4, 133110.6};
  // Gaussian fitted mean and std at high KE >=2.5GeV/c
  double PEe[12] =    {11156.2, 11541.7, 15850.8, 24511.1, 37191.5, 68592.5, 100927.6, 146714.4, 167583.1, 211412.3, 253392.6, 355893.6};
  double PEerre[12] = {27900.5, 27416.9, 29705.5, 31926.8, 39357.3, 52381.3, 57187.3, 36266.6, 29406.1, 32608.9, 33036.5, 47306.1};


  double KEpi[4] = {1.3669, 1.8653, 2.8637, 5.8621};
  // histogram mean and std
  //double PEpi[4] = {78107.6, 101917.7, 155856.7, 256333.3};
  double KEerrpi[4] = {0, 0, 0, 0};
  //double PEerrpi[4] = {66631.8, 76289.1, 97969.5, 143325.1};
  double PEpi[4] =    {78107.6, 101917.7, 155856.7, 358062.5};
  double PEerrpi[4] = {66631.8, 76289.1, 97969.5, 59422.7};

  double KEp[3] = {0.8311, 1.2710, 2.2052};
  double PEp[3] = {28075.5, 49252.2, 107986.7};
  double KEerrp[3] = {0, 0, 0};
  double PEerrp[3] = {41299.9, 61208.0, 90149.2};
  //double KEk[4] = {0.6215, 1.0855, 2.5467, 3.5367};
  //double PEk[4] = {3738.2, 5513.3, 9646.8, 11267.7};
  //double KEerrk[4] = {0, 0, 0, 0};
  //double PEerrk[4] = {3900.9, 4241.3, 5487.7, 5850.4};

  auto egraph = new TGraphErrors(nKEe, KEe, PEe, KEerre, PEerre);
  auto pigraph = new TGraphErrors(nKEpi, KEpi, PEpi, KEerrpi, PEerrpi);
  auto pgraph = new TGraphErrors(nKEp, KEp, PEp, KEerrp, PEerrp);
  //auto kgraph = new TGraphErrors(nKEk, KEk, PEk, KEerrk, PEerrk);

  TFile *f = new TFile("Qbased_PE_KE_allparticle_3rdbeamperiod_PDSonly.root", "RECREATE");

  egraph->SetMarkerColor(3);
  egraph->SetMarkerStyle(20);
  egraph->SetLineColor(3);  // err bar
  pigraph->SetMarkerColor(4);
  pigraph->SetMarkerStyle(21);
  pigraph->SetLineColor(4);  // err bar
  pgraph->SetMarkerColor(6);
  pgraph->SetMarkerStyle(23);
  pgraph->SetLineColor(6);  // err bar
  //kgraph->SetMarkerColor(2);
  //kgraph->SetMarkerStyle(22);
  // Create a TMultiGraph
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("3rd Beam Period PDS only;KE (GeV);Cathode PDS Tot PE mean");

  // Add the TGraphErrors to the TMultiGraph
  mg->Add(egraph);
  mg->Add(pigraph);
  mg->Add(pgraph);
  //mg->Add(kgraph);

  // Create a canvas and draw the TMultiGraph
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  mg->Draw("AP"); // "A" draws axes, "P" draws markers with errors
  mg->GetXaxis()->SetLimits(0, 9);
  mg->GetYaxis()->SetLimits(-40000, 450000);

  TF1 *lineare = new TF1("lineare",   "[0]+[1]*x", 0, 8.5);
  TF1 *linearpi = new TF1("linearpi", "[0]+[1]*x", 0, 6.5);
  TF1 *linearp = new TF1("linearp",   "[0]+[1]*x", 0, 3.0);
  //TF1 *lineark = new TF1("lineark","[0]+[1]*x", 0, 4.5);
  egraph->Fit("lineare");
  TF1 *fite = egraph->GetFunction("lineare");
  fite->SetLineColor(3);
  Double_t fite_chi2 = fite->GetChisquare();
  int fite_ndf = fite->GetNDF();
  // To get the p-value, you need the TMath library
  Double_t fite_pValue = TMath::Prob(fite_chi2, fite_ndf);
  // value of the first parameter
  Double_t fite_p0 = fite->GetParameter(0);
  Double_t fite_p1 = fite->GetParameter(1);

  cout << "e+ - chi2: "<< fite_chi2 << ", ndf: " << fite_ndf << ", pvalue: "<< fite_pValue << ", p0: "<< fite_p0 << ", p1: " << fite_p1 << endl;

  pigraph->Fit("linearpi");
  TF1 *fitpi = pigraph->GetFunction("linearpi");
  fitpi->SetLineColor(4);
  Double_t fitpi_chi2 = fitpi->GetChisquare();
  int fitpi_ndf = fitpi->GetNDF();
  // To get the p-value, you need the TMath library
  Double_t fitpi_pValue = TMath::Prob(fitpi_chi2, fitpi_ndf);
  // value of the first parameter
  Double_t fitpi_p0 = fitpi->GetParameter(0);
  Double_t fitpi_p1 = fitpi->GetParameter(1);
  cout << "pi+ -  chi2: "<< fitpi_chi2 << ", ndf: " << fitpi_ndf << ", pvalue: "<< fitpi_pValue << ", p0: "<< fitpi_p0 << ", p1: " << fitpi_p1 << endl;

  pgraph->Fit("linearp");
  TF1 *fitp = pgraph->GetFunction("linearp");
  fitp->SetLineColor(6);
  Double_t fitp_chi2 = fitp->GetChisquare();
  int fitp_ndf = fitp->GetNDF();
  // To get the p-value, you need the TMath library
  Double_t fitp_pValue = TMath::Prob(fitp_chi2, fitp_ndf);
  // value of the first parameter
  Double_t fitp_p0 = fitp->GetParameter(0);
  Double_t fitp_p1 = fitp->GetParameter(1);
  cout << "proton -  chi2: "<< fitp_chi2 << ", ndf: " << fitp_ndf << ", pvalue: "<< fitp_pValue << ", p0: "<< fitp_p0 << ", p1: " << fitp_p1 << endl;

  /*kgraph->Fit("lineark");
  TF1 *fitk = kgraph->GetFunction("lineark");
  fitk->SetLineColor(2);
  Double_t fitk_chi2 = fitk->GetChisquare();
  // value of the first parameter
  Double_t fitk_p0 = fitk->GetParameter(0);
  Double_t fitk_p1 = fitk->GetParameter(1);
  cout << "proton -  chi2: "<< fitk_chi2 << ", p0: "<< fitk_p0 << ", p1: " << fitk_p1 << endl;*/


  // Optional: Add a legend
  TLegend *legend = new TLegend(0.1, 0.6, 0.3, 0.9);
  legend->AddEntry(egraph,  "e^{+}",   "P");
  legend->AddEntry(pigraph, "#pi^{+}", "P");
  legend->AddEntry(pgraph,  "proton",  "P");
  //legend->AddEntry(kgraph, "kaon", "P");
  legend->AddEntry(fite,  Form("#splitline{Fit f = %.1f + %.1f * KE,}{#chi^{2} = %.2f, ndf = %i, p = %.2f}",  fite_p0,  fite_p1,  fite_chi2,  fite_ndf,  fite_pValue), "l");
  legend->AddEntry(fitpi, Form("#splitline{Fit f = %.1f + %.1f * KE,}{#chi^{2} = %.2f, ndf = %i, p = %.2f}",  fitpi_p0, fitpi_p1, fitpi_chi2, fitpi_ndf, fitpi_pValue), "l");
  legend->AddEntry(fitp,  Form("#splitline{Fit f = %.1f + %.1f * KE,}{#chi^{2} = %.2f, ndf = %i, p = %.2f}",  fitp_p0, fitp_p1, fitp_chi2, fitp_ndf, fitp_pValue), "l");
  //legend->AddEntry(fitk, Form("Fit chi2 = %.2f: %.1f + %.1f * x", fitk_chi2, fitk_p0, fitk_p1), "l");
  legend->Draw();
  c1->Write();

} // End function: void
