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


  int nKEe = 4;
  int nKEp = 4;
  int nKEk = 4;
  double KEe[4] = {0.9995, 1.4995, 2.9995, 3.9995};
  double PEe[4] = {3222.4, 5334.2, 10282.8, 11883.0};
  double KEerre[4] = {0, 0, 0, 0};
  double PEerre[4] = {2763.5, 3012.7, 2733.5, 3506.3};
  double KEp[4] = {0.4331, 0.8311, 2.2052, 3.1705};
  double PEp[4] = {1747.6, 2361.0, 6660.6, 8382.3};
  double KEerrp[4] = {0, 0, 0, 0};
  double PEerrp[4] = {2894.9, 3195.6, 5741.8, 6400.8};
  double KEk[4] = {0.6215, 1.0855, 2.5467, 3.5367};
  double PEk[4] = {3738.2, 5513.3, 9646.8, 11267.7};
  double KEerrk[4] = {0, 0, 0, 0};
  double PEerrk[4] = {3900.9, 4241.3, 5487.7, 5850.4};

  auto egraph = new TGraphErrors(nKEe, KEe, PEe, KEerre, PEerre);
  auto pgraph = new TGraphErrors(nKEp, KEp, PEp, KEerrp, PEerrp);
  auto kgraph = new TGraphErrors(nKEk, KEk, PEk, KEerrk, PEerrk);

  TFile *f = new TFile("PE_KE_allparticle_beam.root", "RECREATE");

  egraph->SetMarkerColor(3);
  egraph->SetMarkerStyle(20);
  pgraph->SetMarkerColor(4);
  pgraph->SetMarkerStyle(21);
  kgraph->SetMarkerColor(2);
  kgraph->SetMarkerStyle(22);
  // Create a TMultiGraph
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("August Beam runs;KE (GeV);Tot cathode PE");

  // Add the TGraphErrors to the TMultiGraph
  mg->Add(egraph); // "P" option draws markers
  mg->Add(pgraph);
  mg->Add(kgraph);

  // Create a canvas and draw the TMultiGraph
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  mg->Draw("AP"); // "A" draws axes, "P" draws markers with errors
  mg->GetXaxis()->SetLimits(0, 4.5);

  TF1 *lineare = new TF1("lineare","[0]+[1]*x", 0, 4.5);
  TF1 *linearp = new TF1("linearp","[0]+[1]*x", 0, 4.5);
  TF1 *lineark = new TF1("lineark","[0]+[1]*x", 0, 4.5);
  egraph->Fit("lineare");
  TF1 *fite = egraph->GetFunction("lineare");
  fite->SetLineColor(3);
  Double_t fite_chi2 = fite->GetChisquare();
  // value of the first parameter
  Double_t fite_p0 = fite->GetParameter(0);
  Double_t fite_p1 = fite->GetParameter(1);
  cout << "e/mu/pi - chi2: "<< fite_chi2 << ", p0: "<< fite_p0 << ", p1: " << fite_p1 << endl;

  pgraph->Fit("linearp");
  TF1 *fitp = pgraph->GetFunction("linearp");
  fitp->SetLineColor(4);
  Double_t fitp_chi2 = fitp->GetChisquare();
  // value of the first parameter
  Double_t fitp_p0 = fitp->GetParameter(0);
  Double_t fitp_p1 = fitp->GetParameter(1);
  cout << "proton -  chi2: "<< fitp_chi2 << ", p0: "<< fitp_p0 << ", p1: " << fitp_p1 << endl;

  kgraph->Fit("lineark");
  TF1 *fitk = kgraph->GetFunction("lineark");
  fitk->SetLineColor(2);
  Double_t fitk_chi2 = fitk->GetChisquare();
  // value of the first parameter
  Double_t fitk_p0 = fitk->GetParameter(0);
  Double_t fitk_p1 = fitk->GetParameter(1);
  cout << "proton -  chi2: "<< fitk_chi2 << ", p0: "<< fitk_p0 << ", p1: " << fitk_p1 << endl;


  // Optional: Add a legend
  TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
  legend->AddEntry(egraph, "e^{+}/mu^{+}/#pi^{+}", "P");
  legend->AddEntry(pgraph, "proton", "P");
  legend->AddEntry(kgraph, "kaon", "P");
  legend->AddEntry(fite, Form("Fit chi2 = %.2f: %.1f + %.1f * x", fite_chi2, fite_p0, fite_p1), "l");
  legend->AddEntry(fitp, Form("Fit chi2 = %.2f: %.1f + %.1f * x", fitp_chi2, fitp_p0, fitp_p1), "l");
  legend->AddEntry(fitk, Form("Fit chi2 = %.2f: %.1f + %.1f * x", fitk_chi2, fitk_p0, fitk_p1), "l");
  legend->Draw();
  c1->Write();

} // End function: void
