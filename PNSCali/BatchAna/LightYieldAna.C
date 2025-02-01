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

  // Defind voxel size
  double driftstart0 = 0; // distance from XA plane
  double driftstart5 = 5;
  double driftstop10 = 0;
  double driftstop20 = 20;
  double capturerange10 = 10; // +/-10cm from the center
  double capturerange30 = 30; // +/-30cm from the center

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

  // Read detected on each XA when capture is on top of each XA
  int XAsDet_CaponXAstop_drift0_20_yz_abs_30;
  int XA2Det_CaponXA2top_drift0_20_yz_abs_30;
  int XA2Det_CaponXA2top_drift0_20_yz_abs_30_positronprod; // this is pair prod
  int XA3Det_CaponXA3top_drift0_20_yz_abs_30;
  int XA4Det_CaponXA4top_drift0_20_yz_abs_30;
  int XA5Det_CaponXA5top_drift0_20_yz_abs_30;

  int XAsDet_CaponXAstop_drift0_20_yz_abs_10;
  int XA2Det_CaponXA2top_drift0_20_yz_abs_10;
  int XA3Det_CaponXA3top_drift0_20_yz_abs_10;
  int XA4Det_CaponXA4top_drift0_20_yz_abs_10;
  int XA5Det_CaponXA5top_drift0_20_yz_abs_10;

  int XAsDet_CaponXAstop_drift0_10_yz_abs_10;
  int XA2Det_CaponXA2top_drift0_10_yz_abs_10;
  int XA3Det_CaponXA3top_drift0_10_yz_abs_10;
  int XA4Det_CaponXA4top_drift0_10_yz_abs_10;
  int XA5Det_CaponXA5top_drift0_10_yz_abs_10;

  int XAsDet_CaponXAstop_drift5_20_yz_abs_10;
  int XA2Det_CaponXA2top_drift5_20_yz_abs_10;
  int XA3Det_CaponXA3top_drift5_20_yz_abs_10;
  int XA4Det_CaponXA4top_drift5_20_yz_abs_10;
  int XA5Det_CaponXA5top_drift5_20_yz_abs_10;

  int XAsDet_CaponXAstop_drift5_20_yz_abs_30;
  int XA2Det_CaponXA2top_drift5_20_yz_abs_30;
  int XA3Det_CaponXA3top_drift5_20_yz_abs_30;
  int XA4Det_CaponXA4top_drift5_20_yz_abs_30;
  int XA5Det_CaponXA5top_drift5_20_yz_abs_30;

  TFile myPlot("Plots_10k_grid.root", "RECREATE");
  TTree tree("myTree", "myTree");
  tree.Branch("posX", &posX);
  tree.Branch("posY", &posY);
  tree.Branch("posZ", &posZ);
  tree.Branch("totE", &totE);
  tree.Branch("lightyield", &lightyield);

  tree.Branch("XAsDet_CaponXAstop_drift0_20_yz_abs_30", &XAsDet_CaponXAstop_drift0_20_yz_abs_30);
  tree.Branch("XA2Det_CaponXA2top_drift0_20_yz_abs_30", &XA2Det_CaponXA2top_drift0_20_yz_abs_30);
  tree.Branch("XA2Det_CaponXA2top_drift0_20_yz_abs_30_positronprod", &XA2Det_CaponXA2top_drift0_20_yz_abs_30_positronprod);
  tree.Branch("XA3Det_CaponXA3top_drift0_20_yz_abs_30", &XA3Det_CaponXA3top_drift0_20_yz_abs_30);
  tree.Branch("XA4Det_CaponXA4top_drift0_20_yz_abs_30", &XA4Det_CaponXA4top_drift0_20_yz_abs_30);
  tree.Branch("XA5Det_CaponXA5top_drift0_20_yz_abs_30", &XA5Det_CaponXA5top_drift0_20_yz_abs_30);

  tree.Branch("XAsDet_CaponXAstop_drift0_20_yz_abs_10", &XAsDet_CaponXAstop_drift0_20_yz_abs_10);
  tree.Branch("XA2Det_CaponXA2top_drift0_20_yz_abs_10", &XA2Det_CaponXA2top_drift0_20_yz_abs_10);
  tree.Branch("XA3Det_CaponXA3top_drift0_20_yz_abs_10", &XA3Det_CaponXA3top_drift0_20_yz_abs_10);
  tree.Branch("XA4Det_CaponXA4top_drift0_20_yz_abs_10", &XA4Det_CaponXA4top_drift0_20_yz_abs_10);
  tree.Branch("XA5Det_CaponXA5top_drift0_20_yz_abs_10", &XA5Det_CaponXA5top_drift0_20_yz_abs_10);

  tree.Branch("XAsDet_CaponXAstop_drift0_10_yz_abs_10", &XAsDet_CaponXAstop_drift0_10_yz_abs_10);
  tree.Branch("XA2Det_CaponXA2top_drift0_10_yz_abs_10", &XA2Det_CaponXA2top_drift0_10_yz_abs_10);
  tree.Branch("XA3Det_CaponXA3top_drift0_10_yz_abs_10", &XA3Det_CaponXA3top_drift0_10_yz_abs_10);
  tree.Branch("XA4Det_CaponXA4top_drift0_10_yz_abs_10", &XA4Det_CaponXA4top_drift0_10_yz_abs_10);
  tree.Branch("XA5Det_CaponXA5top_drift0_10_yz_abs_10", &XA5Det_CaponXA5top_drift0_10_yz_abs_10);

  tree.Branch("XAsDet_CaponXAstop_drift5_20_yz_abs_10", &XAsDet_CaponXAstop_drift5_20_yz_abs_10);
  tree.Branch("XA2Det_CaponXA2top_drift5_20_yz_abs_10", &XA2Det_CaponXA2top_drift5_20_yz_abs_10);
  tree.Branch("XA3Det_CaponXA3top_drift5_20_yz_abs_10", &XA3Det_CaponXA3top_drift5_20_yz_abs_10);
  tree.Branch("XA4Det_CaponXA4top_drift5_20_yz_abs_10", &XA4Det_CaponXA4top_drift5_20_yz_abs_10);
  tree.Branch("XA5Det_CaponXA5top_drift5_20_yz_abs_10", &XA5Det_CaponXA5top_drift5_20_yz_abs_10);

  tree.Branch("XAsDet_CaponXAstop_drift5_20_yz_abs_30", &XAsDet_CaponXAstop_drift5_20_yz_abs_30);
  tree.Branch("XA2Det_CaponXA2top_drift5_20_yz_abs_30", &XA2Det_CaponXA2top_drift5_20_yz_abs_30);
  tree.Branch("XA3Det_CaponXA3top_drift5_20_yz_abs_30", &XA3Det_CaponXA3top_drift5_20_yz_abs_30);
  tree.Branch("XA4Det_CaponXA4top_drift5_20_yz_abs_30", &XA4Det_CaponXA4top_drift5_20_yz_abs_30);
  tree.Branch("XA5Det_CaponXA5top_drift5_20_yz_abs_30", &XA5Det_CaponXA5top_drift5_20_yz_abs_30);

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
    XAsDet_CaponXAstop_drift0_20_yz_abs_30=0;
    XAsDet_CaponXAstop_drift0_20_yz_abs_10=0;
    XAsDet_CaponXAstop_drift0_10_yz_abs_10=0;
    XAsDet_CaponXAstop_drift5_20_yz_abs_10=0;
    XAsDet_CaponXAstop_drift5_20_yz_abs_30=0;

    if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 &&
       ( ( gammaposY->at(0) > XA2y - capturerange30 && gammaposY->at(0) < XA2y + capturerange30 && gammaposZ->at(0) > XA2z - capturerange30 && gammaposZ->at(0) < XA2z + capturerange30 ) ||
         ( gammaposY->at(0) > XA3y - capturerange30 && gammaposY->at(0) < XA3y + capturerange30 && gammaposZ->at(0) > XA3z - capturerange30 && gammaposZ->at(0) < XA3z + capturerange30 ) ||
         ( gammaposY->at(0) > XA4y - capturerange30 && gammaposY->at(0) < XA4y + capturerange30 && gammaposZ->at(0) > XA4z - capturerange30 && gammaposZ->at(0) < XA4z + capturerange30 ) ||
         ( gammaposY->at(0) > XA5y - capturerange30 && gammaposY->at(0) < XA5y + capturerange30 && gammaposZ->at(0) > XA5z - capturerange30 && gammaposZ->at(0) < XA5z + capturerange30 ) )
       ) {
         // 0-20cm drift x 60 cm x 60cm
         XAsDet_CaponXAstop_drift0_20_yz_abs_30 = CountDetected_OpDetEvents;
       }

    if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 &&
       ( ( gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) ||
         ( gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) ||
         ( gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) ||
         ( gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) )
       ) {
         // smaller voxel : 0-20cm drift x 20 cm x 20cm
         XAsDet_CaponXAstop_drift0_20_yz_abs_10 = CountDetected_OpDetEvents;
       }

    if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop10 &&
       ( ( gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) ||
         ( gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) ||
         ( gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) ||
         ( gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) )
       ) {
         // smaller voxel: 0-10cm drift x 20 cm x 20cm
         XAsDet_CaponXAstop_drift0_10_yz_abs_10 = CountDetected_OpDetEvents;
       }

    if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 &&
       ( ( gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) ||
         ( gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) ||
         ( gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) ||
         ( gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) )
       ) {
         // smaller voxel: 5-20cm drift x 20 cm x 20cm
         XAsDet_CaponXAstop_drift5_20_yz_abs_10 = CountDetected_OpDetEvents;
       }

    if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 &&
       ( ( gammaposY->at(0) > XA2y - capturerange30 && gammaposY->at(0) < XA2y + capturerange30 && gammaposZ->at(0) > XA2z - capturerange30 && gammaposZ->at(0) < XA2z + capturerange30 ) ||
         ( gammaposY->at(0) > XA3y - capturerange30 && gammaposY->at(0) < XA3y + capturerange30 && gammaposZ->at(0) > XA3z - capturerange30 && gammaposZ->at(0) < XA3z + capturerange30 ) ||
         ( gammaposY->at(0) > XA4y - capturerange30 && gammaposY->at(0) < XA4y + capturerange30 && gammaposZ->at(0) > XA4z - capturerange30 && gammaposZ->at(0) < XA4z + capturerange30 ) ||
         ( gammaposY->at(0) > XA5y - capturerange30 && gammaposY->at(0) < XA5y + capturerange30 && gammaposZ->at(0) > XA5z - capturerange30 && gammaposZ->at(0) < XA5z + capturerange30 ) )
       ) {
         // voxel: 5-20cm drift x 60 cm x 60cm
         XAsDet_CaponXAstop_drift5_20_yz_abs_30 = CountDetected_OpDetEvents;
       }

    // Fill tot detected at each XA PD module
    // ievt: 0 cooresponds to EventID: 1, max EventID is tot number of evts
    //std::cout << "ievt: " << ievt << ", EventID_OpDetEvents: " << EventID_OpDetEvents << ", EventID_DetectedPhotons: " << EventID_DetectedPhotons << ", OpChannel_DetectedPhotons: " << OpChannel_DetectedPhotons << std::endl;
    // If EventID_DetectedPhotons == ievt + 1, count OpChannel_DetectedPhotons 0, 1, 2, 3

    // Read detected on each XA when capture is right on top of each XA
    XA2Det_CaponXA2top_drift0_20_yz_abs_30=0;
    XA2Det_CaponXA2top_drift0_20_yz_abs_30_positronprod=0; // this is pair prod
    XA3Det_CaponXA3top_drift0_20_yz_abs_30=0;
    XA4Det_CaponXA4top_drift0_20_yz_abs_30=0;
    XA5Det_CaponXA5top_drift0_20_yz_abs_30=0;

    XA2Det_CaponXA2top_drift0_20_yz_abs_10=0;
    XA3Det_CaponXA3top_drift0_20_yz_abs_10=0;
    XA4Det_CaponXA4top_drift0_20_yz_abs_10=0;
    XA5Det_CaponXA5top_drift0_20_yz_abs_10=0;

    XA2Det_CaponXA2top_drift0_10_yz_abs_10=0;
    XA3Det_CaponXA3top_drift0_10_yz_abs_10=0;
    XA4Det_CaponXA4top_drift0_10_yz_abs_10=0;
    XA5Det_CaponXA5top_drift0_10_yz_abs_10=0;

    XA2Det_CaponXA2top_drift5_20_yz_abs_10=0;
    XA3Det_CaponXA3top_drift5_20_yz_abs_10=0;
    XA4Det_CaponXA4top_drift5_20_yz_abs_10=0;
    XA5Det_CaponXA5top_drift5_20_yz_abs_10=0;

    XA2Det_CaponXA2top_drift5_20_yz_abs_30=0;
    XA3Det_CaponXA3top_drift5_20_yz_abs_30=0;
    XA4Det_CaponXA4top_drift5_20_yz_abs_30=0;
    XA5Det_CaponXA5top_drift5_20_yz_abs_30=0;

    for ( int idetph = 0; idetph < totDetectedPhotons; idetph++ ) {
      DetectedPhotonsTree->GetEntry(idetph);
      // Count detected phs in this event on each XA
      // XA2
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 2) {
        //
        // If capture happens right on top of each XA (above the PD, around the center)
        //
        // Right on top of XA2
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA2y - capturerange30 && gammaposY->at(0) < XA2y + capturerange30 && gammaposZ->at(0) > XA2z - capturerange30 && gammaposZ->at(0) < XA2z + capturerange30 ) {
          XA2Det_CaponXA2top_drift0_20_yz_abs_30++;
          if (positronprod == 1) {
            XA2Det_CaponXA2top_drift0_20_yz_abs_30_positronprod++;
          }
        }
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) XA2Det_CaponXA2top_drift0_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop10 && gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) XA2Det_CaponXA2top_drift0_10_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA2y - capturerange10 && gammaposY->at(0) < XA2y + capturerange10 && gammaposZ->at(0) > XA2z - capturerange10 && gammaposZ->at(0) < XA2z + capturerange10 ) XA2Det_CaponXA2top_drift5_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA2y - capturerange30 && gammaposY->at(0) < XA2y + capturerange30 && gammaposZ->at(0) > XA2z - capturerange30 && gammaposZ->at(0) < XA2z + capturerange30 ) XA2Det_CaponXA2top_drift5_20_yz_abs_30++;
      }

      // XA3
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 3) {
        // Right on top of XA3
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA3y - capturerange30 && gammaposY->at(0) < XA3y + capturerange30 && gammaposZ->at(0) > XA3z - capturerange30 && gammaposZ->at(0) < XA3z + capturerange30 ) XA3Det_CaponXA3top_drift0_20_yz_abs_30++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) XA3Det_CaponXA3top_drift0_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop10 && gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) XA3Det_CaponXA3top_drift0_10_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA3y - capturerange10 && gammaposY->at(0) < XA3y + capturerange10 && gammaposZ->at(0) > XA3z - capturerange10 && gammaposZ->at(0) < XA3z + capturerange10 ) XA3Det_CaponXA3top_drift5_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA3y - capturerange30 && gammaposY->at(0) < XA3y + capturerange30 && gammaposZ->at(0) > XA3z - capturerange30 && gammaposZ->at(0) < XA3z + capturerange30 ) XA3Det_CaponXA3top_drift5_20_yz_abs_30++;
      }

      // XA4
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 4) {
        // Right on top of XA4
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA4y - capturerange30 && gammaposY->at(0) < XA4y + capturerange30 && gammaposZ->at(0) > XA4z - capturerange30 && gammaposZ->at(0) < XA4z + capturerange30 ) XA4Det_CaponXA4top_drift0_20_yz_abs_30++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) XA4Det_CaponXA4top_drift0_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop10 && gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) XA4Det_CaponXA4top_drift0_10_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA4y - capturerange10 && gammaposY->at(0) < XA4y + capturerange10 && gammaposZ->at(0) > XA4z - capturerange10 && gammaposZ->at(0) < XA4z + capturerange10 ) XA4Det_CaponXA4top_drift5_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA4y - capturerange30 && gammaposY->at(0) < XA4y + capturerange30 && gammaposZ->at(0) > XA4z - capturerange30 && gammaposZ->at(0) < XA4z + capturerange30 ) XA4Det_CaponXA4top_drift5_20_yz_abs_30++;
      }

      // XA5
      if ( EventID_DetectedPhotons == ievt + 1 && OpChannel_DetectedPhotons == 5) {
        // Right on top of XA5
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA5y - capturerange30 && gammaposY->at(0) < XA5y + capturerange30 && gammaposZ->at(0) > XA5z - capturerange30 && gammaposZ->at(0) < XA5z + capturerange30 ) XA5Det_CaponXA5top_drift0_20_yz_abs_30++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) XA5Det_CaponXA5top_drift0_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart0 && gammaposX->at(0) < CathodeXAplaneX + driftstop10 && gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) XA5Det_CaponXA5top_drift0_10_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA5y - capturerange10 && gammaposY->at(0) < XA5y + capturerange10 && gammaposZ->at(0) > XA5z - capturerange10 && gammaposZ->at(0) < XA5z + capturerange10 ) XA5Det_CaponXA5top_drift5_20_yz_abs_10++;
        if ( gammaposX->at(0) > CathodeXAplaneX + driftstart5 && gammaposX->at(0) < CathodeXAplaneX + driftstop20 && gammaposY->at(0) > XA5y - capturerange30 && gammaposY->at(0) < XA5y + capturerange30 && gammaposZ->at(0) > XA5z - capturerange30 && gammaposZ->at(0) < XA5z + capturerange30 ) XA5Det_CaponXA5top_drift5_20_yz_abs_30++;
      }

    } // end loop over totDetectedPhotons

    tree.Fill();
  } // end loop over totOpDetEvents

  tree.Write();

  myPlot.Close();

  std::cout << "Program finished" << std::endl;

} // End function: void
