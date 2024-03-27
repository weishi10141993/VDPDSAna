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
#include "TH3.h"
#include "TCanvas.h"

void LightYieldAna() {

  TFile *file = new TFile("TextFileGen_hist_module1.root");
  //TFile *file = new TFile("SinglesGen_hist_module1.root");
  TTree* OpDetEventsTree = (TTree*) file->Get("XAresponse/OpDetEvents");
  TTree* DetectedPhotonsTree = (TTree*) file->Get("XAresponse/DetectedPhotons");

  double XAplaneX = -15.387; //cm
  double XA0y = -38;
  double XA0z = 257.901;
  double XA1y = 118;
  double XA1z = 186.701;
  double XA2y = -118;
  double XA2z = 111.701;
  double XA3y = 37.2;
  double XA3z = 40.9009;
  double driftlength = 20; // small CB
  double capturerange = 28; // 2x14cm radiation length

  int CountDetected_OpDetEvents = 0;
  vector<double> *gammaposX = 0;
  vector<double> *gammaposY = 0;
  vector<double> *gammaposZ = 0;
  vector<double> *gammaenergy = 0;
  bool positronprod = false;
  int EventID_OpDetEvents = 0;
  int EventID_DetectedPhotons = 0;

  int OpChannel_DetectedPhotons = 0;

  OpDetEventsTree->SetBranchAddress("CountDetected", &CountDetected_OpDetEvents);
  OpDetEventsTree->SetBranchAddress("posXMC", &gammaposX);
  OpDetEventsTree->SetBranchAddress("posYMC", &gammaposY);
  OpDetEventsTree->SetBranchAddress("posZMC", &gammaposZ);
  OpDetEventsTree->SetBranchAddress("energyMC", &gammaenergy);
  OpDetEventsTree->SetBranchAddress("pairprod", &positronprod);
  OpDetEventsTree->SetBranchAddress("EventID", &EventID_OpDetEvents);

  DetectedPhotonsTree->SetBranchAddress("OpChannel", &OpChannel_DetectedPhotons);
  DetectedPhotonsTree->SetBranchAddress("EventID", &EventID_DetectedPhotons);

  // Define plots
  TH1D *h_X = new TH1D("h_X", "", 50, -25, 25);
  TH1D *h_Y = new TH1D("h_Y", "", 100, -200, 200);
  TH1D *h_Z = new TH1D("h_Z", "", 100, 0, 300);
  TH3D *h_YZX = new TH3D("h_YZX", "",  400, -200, 200, 300, 0, 300, 50, -25, 25);
  TH1D *h_totE = new TH1D("h_totE", "", 30000, 0, 6.1);
  //TH1D *h_totE = new TH1D("h_totE", "", 3000, 6.097, 6.1);
  //TH2D *h_totE_detected                   = new TH2D("h_totE_detected", "",                   30, 6.097, 6.1, 70, 0, 700);
  //TH2D* h_totE_detected_column_normalized = new TH2D("h_totE_detected_column_normalized", "", 30, 6.097, 6.1, 70, 0, 700);
  TH2D *h_totE_detected                   = new TH2D("h_totE_detected", "",                   300, 0, 6.1, 70, 0, 700);
  TH2D* h_totE_detected_column_normalized = new TH2D("h_totE_detected_column_normalized", "", 300, 0, 6.1, 70, 0, 700);
  TH1D *h_LY = new TH1D("h_LY", "h_LY", 60, 0, 60.0);
  TH1D *h_XA0_detected = new TH1D("h_XA0_detected", "h_XA0_detected", 1000, 0, 2000.0);
  TH1D *h_XA1_detected = new TH1D("h_XA1_detected", "h_XA1_detected", 1000, 0, 2000.0);
  TH1D *h_XA2_detected = new TH1D("h_XA2_detected", "h_XA2_detected", 1000, 0, 2000.0);
  TH1D *h_XA3_detected = new TH1D("h_XA3_detected", "h_XA3_detected", 1000, 0, 2000.0);
  TH1D *h_SourceXAsTop_XAsDet = new TH1D("h_SourceXAsTop_XAsDet", "h_SourceXAsTop_XAsDet", 1000, 0, 2000.0);
  TH1D *h_SourceXA0Top_XA0Det = new TH1D("h_SourceXA0Top_XA0Det", "h_SourceXA0Top_XA0Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA0Top_XA0Det_positronprod = new TH1D("h_SourceXA0Top_XA0Det_positronprod", "h_SourceXA0Top_XA0Det_positronprod", 1000, 0, 2000.0);
  TH1D *h_SourceXA0Top_XA1Det = new TH1D("h_SourceXA0Top_XA1Det", "h_SourceXA0Top_XA1Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA0Top_XA2Det = new TH1D("h_SourceXA0Top_XA2Det", "h_SourceXA0Top_XA2Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA0Top_XA3Det = new TH1D("h_SourceXA0Top_XA3Det", "h_SourceXA0Top_XA3Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA1Top_XA0Det = new TH1D("h_SourceXA1Top_XA0Det", "h_SourceXA1Top_XA0Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA1Top_XA1Det = new TH1D("h_SourceXA1Top_XA1Det", "h_SourceXA1Top_XA1Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA1Top_XA2Det = new TH1D("h_SourceXA1Top_XA2Det", "h_SourceXA1Top_XA2Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA1Top_XA3Det = new TH1D("h_SourceXA1Top_XA3Det", "h_SourceXA1Top_XA3Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA2Top_XA0Det = new TH1D("h_SourceXA2Top_XA0Det", "h_SourceXA2Top_XA0Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA2Top_XA1Det = new TH1D("h_SourceXA2Top_XA1Det", "h_SourceXA2Top_XA1Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA2Top_XA2Det = new TH1D("h_SourceXA2Top_XA2Det", "h_SourceXA2Top_XA2Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA2Top_XA3Det = new TH1D("h_SourceXA2Top_XA3Det", "h_SourceXA2Top_XA3Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA3Top_XA0Det = new TH1D("h_SourceXA3Top_XA0Det", "h_SourceXA3Top_XA0Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA3Top_XA1Det = new TH1D("h_SourceXA3Top_XA1Det", "h_SourceXA3Top_XA1Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA3Top_XA2Det = new TH1D("h_SourceXA3Top_XA2Det", "h_SourceXA3Top_XA2Det", 1000, 0, 2000.0);
  TH1D *h_SourceXA3Top_XA3Det = new TH1D("h_SourceXA3Top_XA3Det", "h_SourceXA3Top_XA3Det", 1000, 0, 2000.0);
  TH2D *h_X_LY                   = new TH2D("h_X_LY",                   "", 50, -25, 25, 30, 0, 60.0);
  TH2D *h_X_LY_column_normalized = new TH2D("h_X_LY_column_normalized", "", 50, -25, 25, 30, 0, 60.0);
  TH2D *h_Y_LY                   = new TH2D("h_Y_LY",                   "", 40, -200, 200, 30, 0, 60.0);
  TH2D *h_Y_LY_column_normalized = new TH2D("h_Y_LY_column_normalized", "", 40, -200, 200, 30, 0, 60.0);
  TH2D *h_Z_LY                   = new TH2D("h_Z_LY",                   "", 50, 0, 300, 30, 0, 60.0);
  TH2D *h_Z_LY_column_normalized = new TH2D("h_Z_LY_column_normalized", "", 50, 0, 300, 30, 0, 60.0);
  int nxbins;
  int nybins;
  int hist2dentries;

  int totOpDetEvents = 0; // Total number of totOpDetEvents of this exposure
  totOpDetEvents = OpDetEventsTree->GetEntries();
  int totDetectedPhotons = 0; // Total number of DetectedPhotons of this exposure
  totDetectedPhotons = DetectedPhotonsTree->GetEntries();
  std::cout << "Total evts in OpDetEventsTree: " << totOpDetEvents << std::endl;
  std::cout << "Total evts in DetectedPhotonsTree: " << totDetectedPhotons << std::endl;

  // Loop over totOpDetEvents
  for ( int ievt = 0; ievt < totOpDetEvents; ievt++ ) {

    OpDetEventsTree->GetEntry(ievt);

    int numgamma = 0; // number of gamma cascades in each evt
    double totE = 0; // total gamma energy, unit: MeV

    // Get number of cascades in each evt
    numgamma = gammaenergy->size();

    for ( int igamma = 0; igamma < numgamma; igamma++ ) totE = totE + gammaenergy->at(igamma)*1000; // add up energy from all gamma cascades

    //std::cout << "number of cascades: " << numgamma << ", totE (MeV): " << totE << std::endl;

    // Plot capture position
    // Since all cascade gammas are primary particles in G4 and they are at the same pos,
    // use the first particle's (i.e., one of the gammas) position as capture position
    h_X->Fill(gammaposX->at(0));
    h_Y->Fill(gammaposY->at(0));
    h_Z->Fill(gammaposZ->at(0));
    h_YZX->Fill(gammaposY->at(0), gammaposZ->at(0), gammaposX->at(0));

    // Plot tot E distribution of all events
    h_totE->Fill(totE);

    // Plot tot det photons vs tot E (ideally should have linear correlation)
    h_totE_detected->Fill(totE, CountDetected_OpDetEvents);

    // Plot light yield distribution of all event
    h_LY->Fill(CountDetected_OpDetEvents/totE);

    // Plot light yield as a function of X, Y, Z
    h_X_LY->Fill(gammaposX->at(0), CountDetected_OpDetEvents/totE); // since all gammas in each evt are at same pos
    h_Y_LY->Fill(gammaposY->at(0), CountDetected_OpDetEvents/totE);
    h_Z_LY->Fill(gammaposZ->at(0), CountDetected_OpDetEvents/totE);

    if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength && gammaposY->at(0) > XA0y - capturerange && gammaposY->at(0) < XA0y + capturerange && gammaposZ->at(0) > XA0z - capturerange && gammaposZ->at(0) < XA0z + capturerange ) h_SourceXAsTop_XAsDet->Fill(CountDetected_OpDetEvents);
    if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength && gammaposY->at(0) > XA1y - capturerange && gammaposY->at(0) < XA1y + capturerange && gammaposZ->at(0) > XA1z - capturerange && gammaposZ->at(0) < XA1z + capturerange ) h_SourceXAsTop_XAsDet->Fill(CountDetected_OpDetEvents);
    if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) h_SourceXAsTop_XAsDet->Fill(CountDetected_OpDetEvents);
    if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) h_SourceXAsTop_XAsDet->Fill(CountDetected_OpDetEvents);

    // Fill tot detected at each XA PD module
    // ievt: 0 cooresponds to EventID: 1, max EventID is tot number of evts
    //std::cout << "ievt: " << ievt << ", EventID_OpDetEvents: " << EventID_OpDetEvents << ", EventID_DetectedPhotons: " << EventID_DetectedPhotons << ", OpChannel_DetectedPhotons: " << OpChannel_DetectedPhotons << std::endl;
    // If EventID_DetectedPhotons == ievt + 1, count OpChannel_DetectedPhotons 0, 1, 2, 3
    int XA0Detected = 0;
    int XA1Detected = 0;
    int XA2Detected = 0;
    int XA3Detected = 0;

    // Read detected on each XA when capture is right on top of each XA
    int XA0Detected_captureonXA0top=0;
    int XA0Detected_captureonXA0top_positronprod=0; // this is pair prod
    int XA1Detected_captureonXA0top=0;
    int XA2Detected_captureonXA0top=0;
    int XA3Detected_captureonXA0top=0;

    int XA0Detected_captureonXA1top=0;
    int XA1Detected_captureonXA1top=0;
    int XA2Detected_captureonXA1top=0;
    int XA3Detected_captureonXA1top=0;

    int XA0Detected_captureonXA2top=0;
    int XA1Detected_captureonXA2top=0;
    int XA2Detected_captureonXA2top=0;
    int XA3Detected_captureonXA2top=0;

    int XA0Detected_captureonXA3top=0;
    int XA1Detected_captureonXA3top=0;
    int XA2Detected_captureonXA3top=0;
    int XA3Detected_captureonXA3top=0;

    for ( int idetph = 0; idetph < totDetectedPhotons; idetph++ ) {
      DetectedPhotonsTree->GetEntry(idetph);
      // Count detected phs in this event on each XA

      // XA0
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 0) {
        XA0Detected++;
        //
        // If capture happens right on top of each XA (above the PD, around the center)
        //
        // Right on top of XA0
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA0y - capturerange && gammaposY->at(0) < XA0y + capturerange && gammaposZ->at(0) > XA0z - capturerange && gammaposZ->at(0) < XA0z + capturerange ) {
          XA0Detected_captureonXA0top++;
          if (positronprod == 1) {
            XA0Detected_captureonXA0top_positronprod++;
          }
        }
        // Right on top of XA1
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA1y - capturerange && gammaposY->at(0) < XA1y + capturerange && gammaposZ->at(0) > XA1z - capturerange && gammaposZ->at(0) < XA1z + capturerange ) XA0Detected_captureonXA1top++;
        // Right on top of XA2
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) XA0Detected_captureonXA2top++;
        // Right on top of XA3
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) XA0Detected_captureonXA3top++;
      }

      // XA1
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 1) {
        XA1Detected++;
        // Right on top of XA0
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA0y - capturerange && gammaposY->at(0) < XA0y + capturerange && gammaposZ->at(0) > XA0z - capturerange && gammaposZ->at(0) < XA0z + capturerange ) XA1Detected_captureonXA0top++;
        // Right on top of XA1
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA1y - capturerange && gammaposY->at(0) < XA1y + capturerange && gammaposZ->at(0) > XA1z - capturerange && gammaposZ->at(0) < XA1z + capturerange ) XA1Detected_captureonXA1top++;
        // Right on top of XA2
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) XA1Detected_captureonXA2top++;
        // Right on top of XA3
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) XA1Detected_captureonXA3top++;
      }

      // XA2
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 2) {
        XA2Detected++;
        // Right on top of XA0
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA0y - capturerange && gammaposY->at(0) < XA0y + capturerange && gammaposZ->at(0) > XA0z - capturerange && gammaposZ->at(0) < XA0z + capturerange ) XA2Detected_captureonXA0top++;
        // Right on top of XA1
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA1y - capturerange && gammaposY->at(0) < XA1y + capturerange && gammaposZ->at(0) > XA1z - capturerange && gammaposZ->at(0) < XA1z + capturerange ) XA2Detected_captureonXA1top++;
        // Right on top of XA2
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) XA2Detected_captureonXA2top++;
        // Right on top of XA3
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) XA2Detected_captureonXA3top++;
      }

      // XA3
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 3) {
        XA3Detected++;
        // Right on top of XA0
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA0y - capturerange && gammaposY->at(0) < XA0y + capturerange && gammaposZ->at(0) > XA0z - capturerange && gammaposZ->at(0) < XA0z + capturerange ) XA3Detected_captureonXA0top++;
        // Right on top of XA1
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA1y - capturerange && gammaposY->at(0) < XA1y + capturerange && gammaposZ->at(0) > XA1z - capturerange && gammaposZ->at(0) < XA1z + capturerange ) XA3Detected_captureonXA1top++;
        // Right on top of XA2
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) XA3Detected_captureonXA2top++;
        // Right on top of XA3
        if ( gammaposX->at(0) > XAplaneX && gammaposX->at(0) < XAplaneX + driftlength  && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) XA3Detected_captureonXA3top++;
      }

    } // end loop over totDetectedPhotons

    h_XA0_detected->Fill(XA0Detected);
    h_XA1_detected->Fill(XA1Detected);
    h_XA2_detected->Fill(XA2Detected);
    h_XA3_detected->Fill(XA3Detected);

    h_SourceXA0Top_XA0Det->Fill(XA0Detected_captureonXA0top);
    if (XA0Detected_captureonXA0top_positronprod > 0) h_SourceXA0Top_XA0Det_positronprod->Fill(XA0Detected_captureonXA0top_positronprod);
    h_SourceXA0Top_XA1Det->Fill(XA1Detected_captureonXA0top);
    h_SourceXA0Top_XA2Det->Fill(XA2Detected_captureonXA0top);
    h_SourceXA0Top_XA3Det->Fill(XA3Detected_captureonXA0top);

    h_SourceXA1Top_XA0Det->Fill(XA0Detected_captureonXA1top);
    h_SourceXA1Top_XA1Det->Fill(XA1Detected_captureonXA1top);
    h_SourceXA1Top_XA2Det->Fill(XA2Detected_captureonXA1top);
    h_SourceXA1Top_XA3Det->Fill(XA3Detected_captureonXA1top);

    h_SourceXA2Top_XA0Det->Fill(XA0Detected_captureonXA2top);
    h_SourceXA2Top_XA1Det->Fill(XA1Detected_captureonXA2top);
    h_SourceXA2Top_XA2Det->Fill(XA2Detected_captureonXA2top);
    h_SourceXA2Top_XA3Det->Fill(XA3Detected_captureonXA2top);

    h_SourceXA3Top_XA0Det->Fill(XA0Detected_captureonXA3top);
    h_SourceXA3Top_XA1Det->Fill(XA1Detected_captureonXA3top);
    h_SourceXA3Top_XA2Det->Fill(XA2Detected_captureonXA3top);
    h_SourceXA3Top_XA3Det->Fill(XA3Detected_captureonXA3top);

  } // end loop over totOpDetEvents

  // Output file
  TFile myPlot("PDS_PNS_Calib_ColdBox.root", "RECREATE");

  h_X->GetXaxis()->SetTitle("Capture position X (cm)");
  h_X->GetYaxis()->SetTitle("Events/cm");
  h_X->Write();

  h_Y->GetXaxis()->SetTitle("Capture position Y (cm)");
  h_Y->GetYaxis()->SetTitle("Events/2cm");
  h_Y->Write();

  h_Z->GetXaxis()->SetTitle("Capture position Z (cm)");
  h_Z->GetYaxis()->SetTitle("Events/3cm");
  h_Z->Write();

  h_YZX->GetXaxis()->SetTitle("Y (cm)");
  h_YZX->GetYaxis()->SetTitle("Z (cm)");
  h_YZX->GetZaxis()->SetTitle("X (cm)");
  h_YZX->Write();

  h_totE->GetXaxis()->SetTitle("Total gamma cascade E (MeV)");
  h_totE->GetYaxis()->SetTitle("Events/eV");
  h_totE->Write();

  // Column normalize
  TH1D *h_totE_detected_projectX = h_totE_detected->ProjectionX();
  TH1D *h_totE_detected_projectY = h_totE_detected->ProjectionY();
  nxbins = h_totE_detected_projectX->GetNbinsX();
  nybins = h_totE_detected_projectY->GetNbinsX();
  //std::cout << "number of xbins: " << nxbins << ", ybins: " << nybins << std::endl;
  hist2dentries = 0;
  for (int ixbin=1; ixbin <= nxbins; ixbin++){
    int column_tot_tmp = 0;
    column_tot_tmp = h_totE_detected_projectX->GetBinContent(ixbin);
    //std::cout << "ixbin: " << ixbin << ", tot: " << column_tot_tmp << std::endl;
    hist2dentries = hist2dentries + column_tot_tmp;

    for (int iybin=1; iybin <= nybins; iybin++){
      if (column_tot_tmp > 0) h_totE_detected_column_normalized->SetBinContent(ixbin, iybin, h_totE_detected->GetBinContent(ixbin, iybin)*1.0/column_tot_tmp);
    }
  }
  //std::cout << "hist2dentries: " << hist2dentries << std::endl;

  h_totE_detected_projectY->GetXaxis()->SetTitle("Total detected PE");
  h_totE_detected_projectY->Write();

  h_totE_detected_column_normalized->SetTitle("Column Normalized");
  h_totE_detected_column_normalized->GetXaxis()->SetTitle("Total gamma cascade E (MeV)");
  h_totE_detected_column_normalized->GetYaxis()->SetTitle("Total detected photons");
  h_totE_detected_column_normalized->Write();

  h_LY->GetXaxis()->SetTitle("Light yield (PE/MeV)");
  h_LY->Write();

  h_XA0_detected->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_XA0_detected->Write();
  h_XA1_detected->GetXaxis()->SetTitle("XA1 Detected PEs");
  h_XA1_detected->Write();
  h_XA2_detected->GetXaxis()->SetTitle("XA2 Detected PEs");
  h_XA2_detected->Write();
  h_XA3_detected->GetXaxis()->SetTitle("XA3 Detected PEs");
  h_XA3_detected->Write();
  h_SourceXAsTop_XAsDet->GetXaxis()->SetTitle("All XAs Detected PEs");
  h_SourceXAsTop_XAsDet->Write();
  h_SourceXA0Top_XA0Det->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_SourceXA0Top_XA0Det->Write();
  h_SourceXA0Top_XA0Det_positronprod->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_SourceXA0Top_XA0Det_positronprod->Write();
  h_SourceXA0Top_XA1Det->GetXaxis()->SetTitle("XA1 Detected PEs");
  h_SourceXA0Top_XA1Det->Write();
  h_SourceXA0Top_XA2Det->GetXaxis()->SetTitle("XA2 Detected PEs");
  h_SourceXA0Top_XA2Det->Write();
  h_SourceXA0Top_XA3Det->GetXaxis()->SetTitle("XA3 Detected PEs");
  h_SourceXA0Top_XA3Det->Write();
  h_SourceXA1Top_XA0Det->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_SourceXA1Top_XA0Det->Write();
  h_SourceXA1Top_XA1Det->GetXaxis()->SetTitle("XA1 Detected PEs");
  h_SourceXA1Top_XA1Det->Write();
  h_SourceXA1Top_XA2Det->GetXaxis()->SetTitle("XA2 Detected PEs");
  h_SourceXA1Top_XA2Det->Write();
  h_SourceXA1Top_XA3Det->GetXaxis()->SetTitle("XA3 Detected PEs");
  h_SourceXA1Top_XA3Det->Write();
  h_SourceXA2Top_XA0Det->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_SourceXA2Top_XA0Det->Write();
  h_SourceXA2Top_XA1Det->GetXaxis()->SetTitle("XA1 Detected PEs");
  h_SourceXA2Top_XA1Det->Write();
  h_SourceXA2Top_XA2Det->GetXaxis()->SetTitle("XA2 Detected PEs");
  h_SourceXA2Top_XA2Det->Write();
  h_SourceXA2Top_XA3Det->GetXaxis()->SetTitle("XA3 Detected PEs");
  h_SourceXA2Top_XA3Det->Write();
  h_SourceXA3Top_XA0Det->GetXaxis()->SetTitle("XA0 Detected PEs");
  h_SourceXA3Top_XA0Det->Write();
  h_SourceXA3Top_XA1Det->GetXaxis()->SetTitle("XA1 Detected PEs");
  h_SourceXA3Top_XA1Det->Write();
  h_SourceXA3Top_XA2Det->GetXaxis()->SetTitle("XA2 Detected PEs");
  h_SourceXA3Top_XA2Det->Write();
  h_SourceXA3Top_XA3Det->GetXaxis()->SetTitle("XA3 Detected PEs");
  h_SourceXA3Top_XA3Det->Write();

  // Column normalize
  TH1D *h_X_LY_projectX = h_X_LY->ProjectionX();
  TH1D *h_X_LY_projectY = h_X_LY->ProjectionY();
  nxbins = h_X_LY_projectX->GetNbinsX();
  nybins = h_X_LY_projectY->GetNbinsX();
  hist2dentries = 0;
  for (int ixbin=1; ixbin <= nxbins; ixbin++){
    int column_tot_tmp = 0;
    column_tot_tmp = h_X_LY_projectX->GetBinContent(ixbin);
    hist2dentries = hist2dentries + column_tot_tmp;

    for (int iybin=1; iybin <= nybins; iybin++){
      if (column_tot_tmp > 0) h_X_LY_column_normalized->SetBinContent(ixbin, iybin, h_X_LY->GetBinContent(ixbin, iybin)*1.0/column_tot_tmp);
    }
  }
  h_X_LY_column_normalized->SetTitle("Column Normalized");
  h_X_LY_column_normalized->GetXaxis()->SetTitle("Capture position X (cm)");
  h_X_LY_column_normalized->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_X_LY_column_normalized->Write();

  TH1D *h_Y_LY_projectX = h_Y_LY->ProjectionX();
  TH1D *h_Y_LY_projectY = h_Y_LY->ProjectionY();
  nxbins = h_Y_LY_projectX->GetNbinsX();
  nybins = h_Y_LY_projectY->GetNbinsX();
  hist2dentries = 0;
  for (int ixbin=1; ixbin <= nxbins; ixbin++){
    int column_tot_tmp = 0;
    column_tot_tmp = h_Y_LY_projectX->GetBinContent(ixbin);
    hist2dentries = hist2dentries + column_tot_tmp;

    for (int iybin=1; iybin <= nybins; iybin++){
      if (column_tot_tmp > 0) h_Y_LY_column_normalized->SetBinContent(ixbin, iybin, h_Y_LY->GetBinContent(ixbin, iybin)*1.0/column_tot_tmp);
    }
  }
  h_Y_LY_column_normalized->SetTitle("Column Normalized");
  h_Y_LY_column_normalized->GetXaxis()->SetTitle("Capture position Y (cm)");
  h_Y_LY_column_normalized->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_Y_LY_column_normalized->Write();

  TH1D *h_Z_LY_projectX = h_Z_LY->ProjectionX();
  TH1D *h_Z_LY_projectY = h_Z_LY->ProjectionY();
  nxbins = h_Z_LY_projectX->GetNbinsX();
  nybins = h_Z_LY_projectY->GetNbinsX();
  hist2dentries = 0;
  for (int ixbin=1; ixbin <= nxbins; ixbin++){
    int column_tot_tmp = 0;
    column_tot_tmp = h_Z_LY_projectX->GetBinContent(ixbin);
    hist2dentries = hist2dentries + column_tot_tmp;

    for (int iybin=1; iybin <= nybins; iybin++){
      if (column_tot_tmp > 0) h_Z_LY_column_normalized->SetBinContent(ixbin, iybin, h_Z_LY->GetBinContent(ixbin, iybin)*1.0/column_tot_tmp);
    }
  }
  h_Z_LY_column_normalized->SetTitle("Column Normalized");
  h_Z_LY_column_normalized->GetXaxis()->SetTitle("Capture position Z (cm)");
  h_Z_LY_column_normalized->GetYaxis()->SetTitle("Light yield (PE/MeV)");
  h_Z_LY_column_normalized->Write();

  myPlot.Close();

  std::cout << "Program finished" << std::endl;

} // End function: void
