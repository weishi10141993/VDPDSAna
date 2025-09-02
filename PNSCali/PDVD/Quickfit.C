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

  int nKEe = 8; // electron
  int nKEpi = 2; // pion
  //int nKEk = 4; // kaon
  double KEe[8] = {0.4995, 0.9995, 1.4995, 1.9995, 2.4995, 2.9995, 3.9995, 4.9995};
  double PEe[8] = {1167.5, 3167.8, 5381.3, 7160.6, 8429.4, 9847.0, 12157.6, 13862.0};
  double KEerre[8] = {0, 0, 0, 0};
  double PEerre[8] = {2050.9, 2849.9, 3288.5, 3286.7, 3365.0, 2816.3, 2600.6, 2591.5};
  double KEpi[2] = {2.8637, 5.8621};
  double PEpi[2] = {9765.6, 12850.1};
  double KEerrpi[2] = {0, 0};
  double PEerrpi[2] = {5405.1, 5800.8};
  //double KEk[4] = {0.6215, 1.0855, 2.5467, 3.5367};
  //double PEk[4] = {3738.2, 5513.3, 9646.8, 11267.7};
  //double KEerrk[4] = {0, 0, 0, 0};
  //double PEerrk[4] = {3900.9, 4241.3, 5487.7, 5850.4};

  auto egraph = new TGraphErrors(nKEe, KEe, PEe, KEerre, PEerre);
  auto pigraph = new TGraphErrors(nKEpi, KEpi, PEpi, KEerrpi, PEerrpi);
  //auto kgraph = new TGraphErrors(nKEk, KEk, PEk, KEerrk, PEerrk);

  TFile *f = new TFile("PE_KE_allparticle_3rdbeamperiod_PDSonly.root", "RECREATE");

  egraph->SetMarkerColor(3);
  egraph->SetMarkerStyle(20);
  pigraph->SetMarkerColor(4);
  pigraph->SetMarkerStyle(21);
  //kgraph->SetMarkerColor(2);
  //kgraph->SetMarkerStyle(22);
  // Create a TMultiGraph
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("3rd Beam Period PDS only: run 38930 -  39108;KE (GeV);Cathode PDS Tot PE mean");

  // Add the TGraphErrors to the TMultiGraph
  mg->Add(egraph);
  mg->Add(pigraph);
  //mg->Add(kgraph);

  // Create a canvas and draw the TMultiGraph
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  mg->Draw("AP"); // "A" draws axes, "P" draws markers with errors
  mg->GetXaxis()->SetLimits(0, 7);

  TF1 *lineare = new TF1("lineare","[0]+[1]*x", 0, 4.5);
  TF1 *linearpi = new TF1("linearpi","[0]+[1]*x", 0, 4.5);
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
  cout << "p+ -  chi2: "<< fitpi_chi2 << ", ndf: " << fitpi_ndf << ", pvalue: "<< fitpi_pValue << ", p0: "<< fitpi_p0 << ", p1: " << fitpi_p1 << endl;

  /*kgraph->Fit("lineark");
  TF1 *fitk = kgraph->GetFunction("lineark");
  fitk->SetLineColor(2);
  Double_t fitk_chi2 = fitk->GetChisquare();
  // value of the first parameter
  Double_t fitk_p0 = fitk->GetParameter(0);
  Double_t fitk_p1 = fitk->GetParameter(1);
  cout << "proton -  chi2: "<< fitk_chi2 << ", p0: "<< fitk_p0 << ", p1: " << fitk_p1 << endl;*/


  // Optional: Add a legend
  TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
  legend->AddEntry(egraph, "e^{+}", "P");
  legend->AddEntry(pigraph, "#pi^{+}", "P");
  //legend->AddEntry(kgraph, "kaon", "P");
  legend->AddEntry(fite, Form("#splitline{Fit f = %.1f + %.1f * KE,}{#chi^{2} = %.2f, ndf = %i, p = %.2f}",  fite_p0,  fite_p1,  fite_chi2,  fite_ndf,  fite_pValue), "l");
  legend->AddEntry(fitpi, Form("#splitline{Fit f = %.1f + %.1f * KE,}{#chi^{2} = %.2f, ndf = %i, p = %.2f}",  fitpi_p0, fitpi_p1, fitpi_chi2, fitpi_ndf, fitpi_pValue), "l");
  //legend->AddEntry(fitk, Form("Fit chi2 = %.2f: %.1f + %.1f * x", fitk_chi2, fitk_p0, fitk_p1), "l");
  legend->Draw();
  c1->Write();

} // End function: void
