//
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q LightYieldAna.C
//

#include "TFile.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"

void LightYieldAna() {

  TFile *file = new TFile("TextFileGen_hist_module1.root");
  TTree* tree = (TTree*) file->Get("XAresponse/OpDetEvents");

  int CountDetected = 0;
  vector<double> *gammaposX = 0;
  vector<double> *gammaposY = 0;
  vector<double> *gammaposZ = 0;
  vector<double> *gammaenergy = 0;

  tree->SetBranchAddress("CountDetected", &CountDetected);
  tree->SetBranchAddress("posXMC", &gammaposX);
  tree->SetBranchAddress("posYMC", &gammaposY);
  tree->SetBranchAddress("posZMC", &gammaposZ);
  tree->SetBranchAddress("energyMC", &gammaenergy);

  // Define plots
  TH1D *h_totE = new TH1D("h_totE", "h_totE", 300, 6.097, 6.1);
  TH2D *h_totE_detected = new TH2D("h_totE_detected", "h_totE_detected", 30, 6.097, 6.1, 70, 0, 700);
  TH1D *h_LY = new TH1D("h_LY", "h_LY", 60, 0, 60.0);
  TH2D *h_X_LY = new TH2D("h_X_LY", "h_X_LY", 25, -25, 0, 20, 0, 60.0);
  TH2D *h_Y_LY = new TH2D("h_Y_LY", "h_Y_LY", 30, -90, 90, 20, 0, 60.0);
  TH2D *h_Z_LY = new TH2D("h_Z_LY", "h_Z_LY", 20, 70, 230, 20, 0, 60.0);

  // Plot light yield as a function of XY, YZ, XZ
  TH2D *h_XY_LY = new TH2D("h_XY_LY", "h_XY_LY", 25, -25, 0, 30, -90, 90);
  TH2D *h_YZ_LY = new TH2D("h_YZ_LY", "h_YZ_LY", 30, -90, 90, 20, 70, 230);
  TH2D *h_ZX_LY = new TH2D("h_ZX_LY", "h_ZX_LY", 20, 70, 230, 25, -25, 0);

  int totevts = 0; // Total number of totevts of this exposure
  totevts = tree->GetEntries();
  std::cout << "Total totevts: " << totevts << std::endl;

  // Loop over totevts
  for ( int ievt = 0; ievt < totevts; ievt++ ) {

    tree->GetEntry(ievt);

    int numgamma = 0; // number of gamma cascades in each evt
    double totE = 0; // total gamma energy, unit: MeV

    // Get number of cascades in each evt
    numgamma = gammaenergy->size();

    for ( int igamma = 0; igamma < numgamma; igamma++ ) {
      totE = totE + gammaenergy->at(igamma)*1000; // 2-3 gamma cascades, add E up
    }

    std::cout << "number of cascades: " << numgamma << ", totE (MeV): " << totE << std::endl;

    // Plot tot E distribution of all events
    h_totE->Fill(totE);

    // Plot tot det photons vs tot E (ideally should have linear correlation)
    h_totE_detected->Fill(totE, CountDetected);

    // Plot light yield distribution of all event
    h_LY->Fill(CountDetected/totE);

    // Plot light yield as a function of X, Y, Z
    h_X_LY->Fill(gammaposX->at(0), CountDetected/totE); // since all gammas in each evt are at same pos
    h_Y_LY->Fill(gammaposY->at(0), CountDetected/totE);
    h_Z_LY->Fill(gammaposZ->at(0), CountDetected/totE);

    // Plot light yield as a function of XY, YZ, XZ
    h_XY_LY->Fill(gammaposX->at(0), gammaposY->at(0), CountDetected/totE); // since all gammas in each evt are at same pos
    h_YZ_LY->Fill(gammaposY->at(0), gammaposZ->at(0), CountDetected/totE);
    h_ZX_LY->Fill(gammaposZ->at(0), gammaposX->at(0), CountDetected/totE);

  } // end loop over totevts


  // Output file
  TFile myPlot("PDS_PNS_Calib_ColdBox.root", "RECREATE");

  TCanvas *c_1 = new TCanvas("c_1", "c_1", 800, 600);
  c_1->cd();

  h_totE->GetXaxis()->SetTitle("Total gamma cascade E (MeV)");
  h_totE->Draw();
  c_1->Write("totE");

  h_totE_detected->GetXaxis()->SetTitle("Total gamma cascade E (MeV)");
  h_totE_detected->GetYaxis()->SetTitle("Total detected photons");
  h_totE_detected->Draw("COLZ");
  c_1->Write("totE_detectedph");

  h_LY->GetXaxis()->SetTitle("Light yield (PE/MeV)");
  h_LY->Draw();
  c_1->Write("LY");

  h_X_LY->GetXaxis()->SetTitle("Cascade position X (cm)");
  h_X_LY->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_X_LY->Draw("COLZ");
  c_1->Write("X_LY");

  h_Y_LY->GetXaxis()->SetTitle("Cascade position Y (cm)");
  h_Y_LY->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_Y_LY->Draw("COLZ");
  c_1->Write("Y_LY");

  h_Z_LY->GetXaxis()->SetTitle("Cascade position Z (cm)");
  h_Z_LY->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_Z_LY->Draw("COLZ");
  c_1->Write("Z_LY");

  h_XY_LY->GetXaxis()->SetTitle("Cascade position X (cm)");
  h_XY_LY->GetYaxis()->SetTitle("Cascade position Y (cm)");
  h_XY_LY->Draw("COLZ");
  c_1->Write("XY_LY");

  h_YZ_LY->GetXaxis()->SetTitle("Cascade position Y (cm)");
  h_YZ_LY->GetYaxis()->SetTitle("Cascade position Z (cm)");
  h_YZ_LY->Draw("COLZ");
  c_1->Write("YZ_LY");

  h_ZX_LY->GetXaxis()->SetTitle("Cascade position Z (cm)");
  h_ZX_LY->GetYaxis()->SetTitle("Cascade position X (cm)");
  h_ZX_LY->Draw("COLZ");
  c_1->Write("ZX_LY");

  myPlot.Close();

} // End function: void
