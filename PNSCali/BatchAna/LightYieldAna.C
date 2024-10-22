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

  ProcInfo_t procinfo;
  gSystem->GetProcInfo(&procinfo);
  std::cerr << "[MEM]: Resident = " << procinfo.fMemResident << std::endl;

  TFile *file = new TFile("PDS_PNS_Calib_ColdBox_10k.root");
  //TFile *file = new TFile("SinglesGen_hist_module1.root");
  TTree* OpDetEventsTree = (TTree*) file->Get("XAresponse/OpDetEvents");
  TTree* DetectedPhotonsTree = (TTree*) file->Get("XAresponse/DetectedPhotons");

  double CathodeXAplaneX = -15.387; //cm
  double XA2y = -38;
  double XA2z = 257.901;
  double XA3y = 118;
  double XA3z = 186.701;
  double XA4y = -118;
  double XA4z = 111.701;
  double XA5y = 37.2;
  double XA5z = 40.9009;
  double driftlength = 20; // default 20cm drift x 60 cm x 60cm
  double capturerange = 30;
  double driftlength2 = 20; // smaller voxel : 20cm drift x 20 cm x 20cm
  double capturerange2 = 10;
  double driftlength3 = 10; // smaller voxel: 10cm drift x 20 cm x 20cm
  double capturerange3 = 10;

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

  int totOpDetEvents = 0; // Total number of totOpDetEvents of this exposure
  totOpDetEvents = OpDetEventsTree->GetEntries();
  int totDetectedPhotons = 0; // Total number of DetectedPhotons of this exposure
  totDetectedPhotons = DetectedPhotonsTree->GetEntries();
  std::cout << "Total evts in OpDetEventsTree: " << totOpDetEvents << std::endl;
  std::cout << "Total evts in DetectedPhotonsTree: " << totDetectedPhotons << std::endl;

  ProcInfo_t procinfo2;
  gSystem->GetProcInfo(&procinfo2);
  std::cerr << "[MEM]: Resident2 = " << procinfo2.fMemResident << std::endl;

  // Output file
  double posX;
  double posY;
  double posZ;
  double totE;
  double lightyield;

  // Read detected on each XA when capture is right on top of each XA
  int XA2Detected_captureonXA2top;
  int XA2Detected_captureonXA2top_positronprod; // this is pair prod
  int XA3Detected_captureonXA3top;
  int XA4Detected_captureonXA4top;
  int XA5Detected_captureonXA5top;
  int XAsDetected_captureonXAstop;
  int XA2Detected_captureonXA2top2;
  int XA3Detected_captureonXA3top2;
  int XA4Detected_captureonXA4top2;
  int XA5Detected_captureonXA5top2;
  int XAsDetected_captureonXAstop2;
  int XA2Detected_captureonXA2top3;
  int XA3Detected_captureonXA3top3;
  int XA4Detected_captureonXA4top3;
  int XA5Detected_captureonXA5top3;
  int XAsDetected_captureonXAstop3;

  TFile myPlot("Plots_10k_grid.root", "RECREATE");
  TTree tree("myTree", "myTree");
  tree.Branch("posX", &posX);
  tree.Branch("posY", &posY);
  tree.Branch("posZ", &posZ);
  tree.Branch("totE", &totE);
  tree.Branch("lightyield", &lightyield);
  tree.Branch("XA2Detected_captureonXA2top", &XA2Detected_captureonXA2top);
  tree.Branch("XA2Detected_captureonXA2top_positronprod", &XA2Detected_captureonXA2top_positronprod);
  tree.Branch("XA3Detected_captureonXA3top", &XA3Detected_captureonXA3top);
  tree.Branch("XA4Detected_captureonXA4top", &XA4Detected_captureonXA4top);
  tree.Branch("XA5Detected_captureonXA5top", &XA5Detected_captureonXA5top);
  tree.Branch("XAsDetected_captureonXAstop", &XAsDetected_captureonXAstop);
  tree.Branch("XA2Detected_captureonXA2top2", &XA2Detected_captureonXA2top2);
  tree.Branch("XA3Detected_captureonXA3top2", &XA3Detected_captureonXA3top2);
  tree.Branch("XA4Detected_captureonXA4top2", &XA4Detected_captureonXA4top2);
  tree.Branch("XA5Detected_captureonXA5top2", &XA5Detected_captureonXA5top2);
  tree.Branch("XAsDetected_captureonXAstop2", &XAsDetected_captureonXAstop2);
  tree.Branch("XA2Detected_captureonXA2top3", &XA2Detected_captureonXA2top3);
  tree.Branch("XA3Detected_captureonXA3top3", &XA3Detected_captureonXA3top3);
  tree.Branch("XA4Detected_captureonXA4top3", &XA4Detected_captureonXA4top3);
  tree.Branch("XA5Detected_captureonXA5top3", &XA5Detected_captureonXA5top3);
  tree.Branch("XAsDetected_captureonXAstop3", &XAsDetected_captureonXAstop3);

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
    posX = gammaposX->at(0);
    posY = gammaposY->at(0);
    posZ = gammaposZ->at(0);

    // Plot light yield distribution of all event
    lightyield = CountDetected_OpDetEvents/totE;
    XAsDetected_captureonXAstop=0;
    XAsDetected_captureonXAstop2=0;
    XAsDetected_captureonXAstop3=0;

    if ( ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA4y - capturerange && gammaposY->at(0) < XA4y + capturerange && gammaposZ->at(0) > XA4z - capturerange && gammaposZ->at(0) < XA4z + capturerange )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA5y - capturerange && gammaposY->at(0) < XA5y + capturerange && gammaposZ->at(0) > XA5z - capturerange && gammaposZ->at(0) < XA5z + capturerange ) ) {
        XAsDetected_captureonXAstop = CountDetected_OpDetEvents;
      }

    if ( ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA2y - capturerange2 && gammaposY->at(0) < XA2y + capturerange2 && gammaposZ->at(0) > XA2z - capturerange2 && gammaposZ->at(0) < XA2z + capturerange2 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA3y - capturerange2 && gammaposY->at(0) < XA3y + capturerange2 && gammaposZ->at(0) > XA3z - capturerange2 && gammaposZ->at(0) < XA3z + capturerange2 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA4y - capturerange2 && gammaposY->at(0) < XA4y + capturerange2 && gammaposZ->at(0) > XA4z - capturerange2 && gammaposZ->at(0) < XA4z + capturerange2 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA5y - capturerange2 && gammaposY->at(0) < XA5y + capturerange2 && gammaposZ->at(0) > XA5z - capturerange2 && gammaposZ->at(0) < XA5z + capturerange2 ) ) {
        XAsDetected_captureonXAstop2 = CountDetected_OpDetEvents;
      }

    if ( ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA2y - capturerange3 && gammaposY->at(0) < XA2y + capturerange3 && gammaposZ->at(0) > XA2z - capturerange3 && gammaposZ->at(0) < XA2z + capturerange3 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA3y - capturerange3 && gammaposY->at(0) < XA3y + capturerange3 && gammaposZ->at(0) > XA3z - capturerange3 && gammaposZ->at(0) < XA3z + capturerange3 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA4y - capturerange3 && gammaposY->at(0) < XA4y + capturerange3 && gammaposZ->at(0) > XA4z - capturerange3 && gammaposZ->at(0) < XA4z + capturerange3 )
      || ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA5y - capturerange3 && gammaposY->at(0) < XA5y + capturerange3 && gammaposZ->at(0) > XA5z - capturerange3 && gammaposZ->at(0) < XA5z + capturerange3 ) ) {
        XAsDetected_captureonXAstop3 = CountDetected_OpDetEvents;
      }
    // Fill tot detected at each XA PD module
    // ievt: 0 cooresponds to EventID: 1, max EventID is tot number of evts
    //std::cout << "ievt: " << ievt << ", EventID_OpDetEvents: " << EventID_OpDetEvents << ", EventID_DetectedPhotons: " << EventID_DetectedPhotons << ", OpChannel_DetectedPhotons: " << OpChannel_DetectedPhotons << std::endl;
    // If EventID_DetectedPhotons == ievt + 1, count OpChannel_DetectedPhotons 0, 1, 2, 3

    // Read detected on each XA when capture is right on top of each XA
    XA2Detected_captureonXA2top=0;
    XA2Detected_captureonXA2top_positronprod=0; // this is pair prod
    XA3Detected_captureonXA3top=0;
    XA4Detected_captureonXA4top=0;
    XA5Detected_captureonXA5top=0;
    XA2Detected_captureonXA2top2=0;
    XA3Detected_captureonXA3top2=0;
    XA4Detected_captureonXA4top2=0;
    XA5Detected_captureonXA5top2=0;
    XA2Detected_captureonXA2top3=0;
    XA3Detected_captureonXA3top3=0;
    XA4Detected_captureonXA4top3=0;
    XA5Detected_captureonXA5top3=0;

    for ( int idetph = 0; idetph < totDetectedPhotons; idetph++ ) {
      DetectedPhotonsTree->GetEntry(idetph);
      // Count detected phs in this event on each XA
      // XA2
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 2) {
        //
        // If capture happens right on top of each XA (above the PD, around the center)
        //
        // Right on top of XA2
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA2y - capturerange && gammaposY->at(0) < XA2y + capturerange && gammaposZ->at(0) > XA2z - capturerange && gammaposZ->at(0) < XA2z + capturerange ) {
          XA2Detected_captureonXA2top++;
          if (positronprod == 1) {
            XA2Detected_captureonXA2top_positronprod++;
          }
        }
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA2y - capturerange2 && gammaposY->at(0) < XA2y + capturerange2 && gammaposZ->at(0) > XA2z - capturerange2 && gammaposZ->at(0) < XA2z + capturerange2 ) XA2Detected_captureonXA2top2++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA2y - capturerange3 && gammaposY->at(0) < XA2y + capturerange3 && gammaposZ->at(0) > XA2z - capturerange3 && gammaposZ->at(0) < XA2z + capturerange3 ) XA2Detected_captureonXA2top3++;
      }

      // XA3
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 3) {
        // Right on top of XA3
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA3y - capturerange && gammaposY->at(0) < XA3y + capturerange && gammaposZ->at(0) > XA3z - capturerange && gammaposZ->at(0) < XA3z + capturerange ) XA3Detected_captureonXA3top++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA3y - capturerange2 && gammaposY->at(0) < XA3y + capturerange2 && gammaposZ->at(0) > XA3z - capturerange2 && gammaposZ->at(0) < XA3z + capturerange2 ) XA3Detected_captureonXA3top2++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA3y - capturerange3 && gammaposY->at(0) < XA3y + capturerange3 && gammaposZ->at(0) > XA3z - capturerange3 && gammaposZ->at(0) < XA3z + capturerange3 ) XA3Detected_captureonXA3top3++;
      }

      // XA4
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 4) {
        // Right on top of XA4
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA4y - capturerange && gammaposY->at(0) < XA4y + capturerange && gammaposZ->at(0) > XA4z - capturerange && gammaposZ->at(0) < XA4z + capturerange ) XA4Detected_captureonXA4top++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA4y - capturerange2 && gammaposY->at(0) < XA4y + capturerange2 && gammaposZ->at(0) > XA4z - capturerange2 && gammaposZ->at(0) < XA4z + capturerange2 ) XA4Detected_captureonXA4top2++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA4y - capturerange3 && gammaposY->at(0) < XA4y + capturerange3 && gammaposZ->at(0) > XA4z - capturerange3 && gammaposZ->at(0) < XA4z + capturerange3 ) XA4Detected_captureonXA4top3++;

      }

      // XA5
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 5) {
        // Right on top of XA5
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength && gammaposY->at(0) > XA5y - capturerange && gammaposY->at(0) < XA5y + capturerange && gammaposZ->at(0) > XA5z - capturerange && gammaposZ->at(0) < XA5z + capturerange ) XA5Detected_captureonXA5top++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength2 && gammaposY->at(0) > XA5y - capturerange2 && gammaposY->at(0) < XA5y + capturerange2 && gammaposZ->at(0) > XA5z - capturerange2 && gammaposZ->at(0) < XA5z + capturerange2 ) XA5Detected_captureonXA5top2++;
        if ( gammaposX->at(0) > CathodeXAplaneX && gammaposX->at(0) < CathodeXAplaneX + driftlength3 && gammaposY->at(0) > XA5y - capturerange3 && gammaposY->at(0) < XA5y + capturerange3 && gammaposZ->at(0) > XA5z - capturerange3 && gammaposZ->at(0) < XA5z + capturerange3 ) XA5Detected_captureonXA5top3++;
      }

    } // end loop over totDetectedPhotons

    tree.Fill();
  } // end loop over totOpDetEvents

  tree.Write();

  myPlot.Close();

  std::cout << "Program finished" << std::endl;

} // End function: void
