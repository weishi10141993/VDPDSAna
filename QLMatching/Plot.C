// source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
// spack load root@6.28.12
// root -l -b -q  Plot.C

// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TCut.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <THStack.h>
#include <TFitResultPtr.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TEfficiency.h>
#include <TMath.h>
#include "TLorentzVector.h"
#include <TRandom3.h>
#include "TSystem.h"
#include "TROOT.h"
#include <TGraph2D.h>
#include <TRandom.h>
#include <TF2.h>

// C++ includes
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iterator>

void Plot()
{
  // 1/6 of 14kt, 9767 events, each -4.2 to +4.2 ms, bin 0.25 MeV
  Double_t Exposurescale = (1/(14.0/6)) * (1/(8.4*9767/1000/3600/24/365)) * (1/0.25);
  int flashminophits = 2;
  Double_t timesliceT0 = 1000; // ns this looks at flash within T0 +/- 1000ns
  Double_t signalT0 = 0; // this number shouldn't change for all VD sample production

  // signal + bkg
  TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalandBkg/cluster100ns3m_min1hits_1PE_nomemb_100kevts/Marleyandbkg_fullstream_opcluster100ns3m_min1hits_1PE_nomemb_newbtmaxEfrac_2026-1-14_QLMatch_reco22ana_001.root");
  // signal only
  //TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalOnly/100ns3mcluster_min1hits_minophit1PE/MarleyOnly_fullstream_opcluster100ns3m_min1hit_1PE_nomemb_newbtmaxEfrac_2026-1-14_QLMatch_reco22ana.root");

  gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");
  TH1D *h_signalflashes_avg_dyz = new TH1D("h_signalflashes_avg_dyz",  "h_signalflashes_avg_dyz; mean dyz [cm]", 500, 0, 1000);
  TH1D *h_signalflashes_min_dyz = new TH1D("h_signalflashes_min_dyz",  "h_signalflashes_min_dyz; min  dyz [cm]", 500, 0, 1000);
  TH1D *h_signalflashes_max_dyz = new TH1D("h_signalflashes_max_dyz",  "h_signalflashes_max_dyz; max  dyz [cm]", 500, 500, 1500);

  TH1D *h_signalflashes_avg_dt  = new TH1D("h_signalflashes_avg_dt",   "h_signalflashes_avg_dt; mean dt [us]", 50, 0, 50);
  TH1D *h_signalflashes_min_dt  = new TH1D("h_signalflashes_min_dt",   "h_signalflashes_min_dt; min dt [us]", 50, 0, 50);
  TH1D *h_signalflashes_max_dt  = new TH1D("h_signalflashes_max_dt",   "h_signalflashes_max_dt; max dt [us]", 50, 0, 50);

  TH1D *h_signalflashes_avg_purity  = new TH1D("h_signalflashes_avg_purity",   "h_signalflashes_avg_purity; mean flash purity", 20, 0, 2);
  TH1D *h_signalflashes_min_purity  = new TH1D("h_signalflashes_min_purity",   "h_signalflashes_min_purity; min flash purity",  20, 0, 2);
  TH1D *h_signalflashes_max_purity  = new TH1D("h_signalflashes_max_purity",   "h_signalflashes_max_purity; max flash purity",  20, 0, 2);

  TH1D *h_signalflashes_avg_pe             = new TH1D("h_signalflashes_avg_pe",             "h_signalflashes_avg_pe;          flashes mean PE",        100, 0, 100);
  TH1D *h_signalflashes_min_pe             = new TH1D("h_signalflashes_min_pe",             "h_signalflashes_min_pe;          flashes min PE",         100, 0, 100);
  TH1D *h_signalflashes_max_pe             = new TH1D("h_signalflashes_max_pe",             "h_signalflashes_max_pe;          flashes max PE",         100, 0, 1000);
  TH1D *h_signalflashes_mindyz_flash_pe    = new TH1D("h_signalflashes_mindyz_flash_pe",    "h_signalflashes_mindyz_flash_pe; min dyz flash PE",       100, 0, 1000);
  TH1D *h_signalflashes_mindt_flash_pe     = new TH1D("h_signalflashes_mindt_flash_pe",     "h_signalflashes_mindt_flash_pe;  min dt flash PE",        100, 0, 1000);
  TH1D *h_signalflashes_maxpurity_flash_pe = new TH1D("h_signalflashes_maxpurity_flash_pe", "h_signalflashes_maxpurity_flash_pe;  max purity flash PE",100, 0, 1000);

  TH1D *h_signalflashes_avg_ophits             = new TH1D("h_signalflashes_avg_ophits",             "h_signalflashes_avg_ophits;             mean ophits",             10, 0, 10);
  TH1D *h_signalflashes_min_ophits             = new TH1D("h_signalflashes_min_ophits",             "h_signalflashes_min_ophits;             min ophits",              10, 0, 10);
  TH1D *h_signalflashes_max_ophits             = new TH1D("h_signalflashes_max_ophits",             "h_signalflashes_max_ophits;             max ophits",              10, 0, 10);
  TH1D *h_signalflashes_mindyz_flash_ophits    = new TH1D("h_signalflashes_mindyz_flash_ophits",    "h_signalflashes_mindyz_flash_ophits;    min dyz flash ophits",    10, 0, 10);
  TH1D *h_signalflashes_mindt_flash_ophits     = new TH1D("h_signalflashes_mindt_flash_ophits",     "h_signalflashes_mindt_flash_ophits;     min dt flash ophits",     10, 0, 10);
  TH1D *h_signalflashes_maxpurity_flash_ophits = new TH1D("h_signalflashes_maxpurity_flash_ophits", "h_signalflashes_maxpurity_flash_ophits; max purity flash ophits", 10, 0, 10);

  // var comp
  TH1D *h_flash_totophits_marley = new TH1D("h_flash_totophits_marley",      "h_flash_totophits_marley;   flash tot ophits",  25, 0, 25);
  TH1D *h_flash_totophits_bkg    = new TH1D("h_flash_totophits_bkg",         "h_flash_totophits_bkg;      flash tot ophits",  25, 0, 25);

  TH1D *h_flash_pe_marley        = new TH1D("h_flash_pe_marley",             "h_flash_pe_marley;          flash tot PE",      100, 0, 2000);
  TH1D *h_flash_pe_bkg           = new TH1D("h_flash_pe_bkg",                "h_flash_pe_bkg;             flash tot PE",      100, 0, 2000);

  TH1D *h_flash_y_marley         = new TH1D("h_flash_y_marley",             "h_flash_y_marley;          flash y [cm]",      160, -800, 800);
  TH1D *h_flash_y_bkg            = new TH1D("h_flash_y_bkg",                "h_flash_y_bkg;             flash y [cm]",      160, -800, 800);

  TH1D *h_flash_z_marley         = new TH1D("h_flash_z_marley",             "h_flash_z_marley;          flash z [cm]",      250, -200, 2300);
  TH1D *h_flash_z_bkg            = new TH1D("h_flash_z_bkg",                "h_flash_z_bkg;             flash z [cm]",      250, -200, 2300);

  TH1D *h_flash_stdpe_marley     = new TH1D("h_flash_stdpe_marley",          "h_flash_stdpe_marley;       flash std PE",      100, 0, 2000);
  TH1D *h_flash_stdpe_bkg        = new TH1D("h_flash_stdpe_bkg",             "h_flash_stdpe_bkg;          flash std PE",      100, 0, 2000);

  TH1D *h_flash_avgpe_marley     = new TH1D("h_flash_avgpe_marley",          "h_flash_avgpe_marley;       flash avg PE",      100, 0, 2000);
  TH1D *h_flash_avgpe_bkg        = new TH1D("h_flash_avgpe_bkg",             "h_flash_avgpe_bkg;          flash avg PE",      100, 0, 2000);

  TH1D *h_flash_avgtimediff_hits_bkg = new TH1D("h_flash_avgtimediff_hits_bkg",      "h_flash_avgtimediff_hits_bkg;   avg dt ophits ",  20, 0, 0.2);
  TH1D *h_flash_avgtimediff_hits_marley = new TH1D("h_flash_avgtimediff_hits_marley",      "h_flash_avgtimediff_hits_marley;   avg dt ophits",  20, 0, 0.2);

  TH1D *h_flash_avgyzdiff_hits_bkg = new TH1D("h_flash_avgyzdiff_hits_bkg",      "h_flash_avgyzdiff_hits_bkg;   avg dyz ophits",  40, 0, 400);
  TH1D *h_flash_avgyzdiff_hits_marley = new TH1D("h_flash_avgyzdiff_hits_marley",      "h_flash_avgyzdiff_hits_marley;   avg dyz ophits",  40, 0, 400);

  TH1D *h_flash_yzStd_hits_bkg = new TH1D("h_flash_yzStd_hits_bkg",      "h_flash_yzStd_hits_bkg;   Std dyz ophits",  40, 0, 400);
  TH1D *h_flash_yzStd_hits_marley = new TH1D("h_flash_yzStd_hits_marley",      "h_flash_yzStd_hits_marley;   Std dyz ophits",  40, 0, 400);

  TH1D *h_flash_n_neighbors_bkg     = new TH1D("h_flash_n_neighbors_bkg",       "h_flash_n_neighbors_bkg;      N neighbor flashes",  50, 0, 50);
  TH1D *h_flash_n_neighbors_marley  = new TH1D("h_flash_n_neighbors_marley",    "h_flash_n_neighbors_marley;   N neighbor flashes",  50, 0, 50);

  TH1D *h_flash_neighbors_totPE_bkg    = new TH1D("h_flash_neighbors_totPE_bkg",      "h_flash_neighbors_totPE_bkg;     neighbor flashes tot PE",      100, 0, 2000);
  TH1D *h_flash_neighbors_totPE_marley = new TH1D("h_flash_neighbors_totPE_marley",   "h_flash_neighbors_totPE_marley;  neighbor flashes tot PE",      100, 0, 2000);

  TH1D *h_flash_neighbors_avgophits_bkg = new TH1D("h_flash_neighbors_avgophits_bkg",      "h_flash_neighbors_avgophits_bkg;   neighbor flashes avg ophits",  25, 0, 25);
  TH1D *h_flash_neighbors_avgophits_marley = new TH1D("h_flash_neighbors_avgophits_marley",      "h_flash_neighbors_avgophits_marley;   neighbor flashes avg ophits",  25, 0, 25);

  TH1D *h_flash_avgtdiff_neighbors_bkg = new TH1D("h_flash_avgtdiff_neighbors_bkg",      "h_flash_avgtdiff_neighbors_bkg;   avg time diff to neighbors",  60, 0, 6);
  TH1D *h_flash_avgtdiff_neighbors_marley = new TH1D("h_flash_avgtdiff_neighbors_marley",      "h_flash_avgtdiff_neighbors_marley;   avg time diff to neighbors",  60, 0, 6);

  TH1D *h_flash_avgyzdiff_neighbors_bkg = new TH1D("h_flash_avgyzdiff_neighbors_bkg",      "h_flash_avgyzdiff_neighbors_bkg;   avg yz diff to neighbors",  60, 0, 600);
  TH1D *h_flash_avgyzdiff_neighbors_marley = new TH1D("h_flash_avgyzdiff_neighbors_marley",      "h_flash_avgyzdiff_neighbors_marley;   avg yz diff to neighbors",  60, 0, 600);

  TH1D *h_flash_stdyzdiff_neighbors_bkg = new TH1D("h_flash_stdyzdiff_neighbors_bkg",      "h_flash_stdyzdiff_neighbors_bkg;   std yz diff to neighbors",  60, 0, 600);
  TH1D *h_flash_stdyzdiff_neighbors_marley = new TH1D("h_flash_stdyzdiff_neighbors_marley",      "h_flash_stdyzdiff_neighbors_marley;   std yz diff to neighbors",  60, 0, 600);

  TH1D *h_mindt_neighbor_flash_dt_marley = new TH1D("h_mindt_neighbor_flash_dt_marley",      "h_mindt_neighbor_flash_dt_marley;   mindt neighbor flash dt ",  100, 0, 5);
  TH1D *h_mindt_neighbor_flash_dt_bkg = new TH1D("h_mindt_neighbor_flash_dt_bkg",      "h_mindt_neighbor_flash_dt_bkg;   mindt neighbor flash dt ",  100, 0, 5);

  TH1D *h_mindt_neighbor_flash_dyz_marley = new TH1D("h_mindt_neighbor_flash_dyz_marley",      "h_mindt_neighbor_flash_dyz_marley;   mindt neighbor flash dyz ",  40, 0, 400);
  TH1D *h_mindt_neighbor_flash_dyz_bkg = new TH1D("h_mindt_neighbor_flash_dyz_bkg",      "h_mindt_neighbor_flash_dyz_bkg;   mindt neighbor flash dyz ",  40, 0, 400);

  TH1D *h_mindt_neighbor_flash_totPE_marley = new TH1D("h_mindt_neighbor_flash_totPE_marley",      "h_mindt_neighbor_flash_totPE_marley;   mindt neighbor flash PE ",  100, 0, 2000);
  TH1D *h_mindt_neighbor_flash_totPE_bkg = new TH1D("h_mindt_neighbor_flash_totPE_bkg",      "h_mindt_neighbor_flash_totPE_bkg;   mindt neighbor flash PE ",  100, 0, 2000);

  TH1D *h_mindt_neighbor_flash_totophits_marley = new TH1D("h_mindt_neighbor_flash_totophits_marley",      "h_mindt_neighbor_flash_totophits_marley;   mindt neighbor flash ophits ",  25, 0, 25);
  TH1D *h_mindt_neighbor_flash_totophits_bkg = new TH1D("h_mindt_neighbor_flash_totophits_bkg",      "h_mindt_neighbor_flash_totophits_bkg;   mindt neighbor flash ophits ",  25, 0, 25);

  TString title1=""; title1 = title1 + "max PE flash: T0+/-"+Form("%.0f", timesliceT0)+"ns; d (flash - true nue) [cm]";
  TString title2=""; title2 = title2 + "max PE flash: T0+/-"+Form("%.0f", timesliceT0)+"ns; t (flash - true nue) [ns]";
  TH1F *hmaxpe_T0slice_flash_d_res = new TH1F("hmaxpe_T0slice_flash_d_res",  title1, 310, -100, 3000);
  TH1F *hmaxpe_T0slice_flash_t_res = new TH1F("hmaxpe_T0slice_flash_t_res",  title2, 100, -10000, 10000);

  TString title3=""; title3 = title3 + "max PE flash: min"+Form("%i", flashminophits)+"ophits; d (flash - true nue) [cm]";
  TString title4=""; title4 = title4 + "max PE flash: min"+Form("%i", flashminophits)+"ophits; t (flash - true nue) [ns]";
  TH1F *hmaxpe_minophit_flash_d_res = new TH1F("hmaxpe_minophit_flash_d_res",  title3, 310, -100, 3000);
  TH1F *hmaxpe_minophit_flash_t_res = new TH1F("hmaxpe_minophit_flash_t_res",  title4, 900, -4500000, 4500000);

  TString title5=""; title5 = title5 + "max PE flash: T0+/-"+Form("%.0f", timesliceT0)+"ns, min"+Form("%i", flashminophits)+"ophits; d (flash - true nue) [cm]";
  TString title6=""; title6 = title6 + "max PE flash: T0+/-"+Form("%.0f", timesliceT0)+"ns, min"+Form("%i", flashminophits)+"ophits; t (flash - true nue) [ns]";
  TH1F *hmaxpe_T0slice_minophit_flash_d_res = new TH1F("hmaxpe_T0slice_minophit_flash_d_res",  title5, 310, -100, 3000);
  TH1F *hmaxpe_T0slice_minophit_flash_t_res = new TH1F("hmaxpe_T0slice_minophit_flash_t_res",  title6, 100, -10000, 10000);

  TH1F *hdebug_flash_min_d = new TH1F("hdebug_flash_min_d",  "flash reco debug; min d_{| flash - true nue |} [cm]", 220, -100, 1000);
  TH1F *hdebug_flash_min_t = new TH1F("hdebug_flash_min_t",  "flash reco debug; min t_{| flash - true nue |} [ns]", 500, -10000, 10000);

  // Need TH1D, not TH1F, as the bin content is > 2^24, a standard 32-bit float uses a 23-bit mantissa to store the significant digits of a number.
  TH1D *h_KE_bkg0  = new TH1D("h_KE_bkg0",  "Ar39GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg1  = new TH1D("h_KE_bkg1",  "Kr85GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg2  = new TH1D("h_KE_bkg2",  "Ar42GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg3  = new TH1D("h_KE_bkg3",  "K42From42ArGenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg4  = new TH1D("h_KE_bkg4",  "Rn222ChainRn222GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg5  = new TH1D("h_KE_bkg5",  "Rn222ChainPo218GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg6  = new TH1D("h_KE_bkg6",  "Rn222ChainPb214GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg7  = new TH1D("h_KE_bkg7",  "Rn222ChainBi214GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg8  = new TH1D("h_KE_bkg8",  "Rn222ChainPb210GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg9  = new TH1D("h_KE_bkg9",  "Rn220ChainPb212GenInLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg10 = new TH1D("h_KE_bkg10", "K40GenInCathode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg11 = new TH1D("h_KE_bkg11", "U238ChainGenInCathode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg12 = new TH1D("h_KE_bkg12", "Th232ChainGenInCathode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg13 = new TH1D("h_KE_bkg13", "K40GenInAnode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg14 = new TH1D("h_KE_bkg14", "U238ChainGenInAnode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg15 = new TH1D("h_KE_bkg15", "Th232ChainGenInAnode; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg16 = new TH1D("h_KE_bkg16", "Rn222ChainGenInPDS; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg17 = new TH1D("h_KE_bkg17", "K42From42ArGenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg18 = new TH1D("h_KE_bkg18", "Rn222ChainFromPo218GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg19 = new TH1D("h_KE_bkg19", "Rn222ChainFromPb214GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg20 = new TH1D("h_KE_bkg20", "Rn222ChainFromBi214GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg21 = new TH1D("h_KE_bkg21", "Rn222ChainFromPb210GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg22 = new TH1D("h_KE_bkg22", "Rn222ChainFromBi210GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg23 = new TH1D("h_KE_bkg23", "Rn220ChainFromPb212GenInUpperMesh; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg24 = new TH1D("h_KE_bkg24", "CavernwallGammasAtLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg25 = new TH1D("h_KE_bkg25", "foamGammasAtLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg26 = new TH1D("h_KE_bkg26", "CavernwallNeutronsAtLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg27 = new TH1D("h_KE_bkg27", "CryostatNGammasAtLAr; KE [MeV]", 80, 0, 20);
  TH1D *h_KE_bkg28 = new TH1D("h_KE_bkg28", "CavernNGammasAtLAr; KE [MeV]", 80, 0, 20);

  THStack *h_KE_stack = new THStack("h_KE_stack",    "Bkg Process; KE [MeV]; Counts/(kt*yr*MeV)");

  // these are branches you want to read from the above root file
  Double_t marleynueX, marleynueY, marleynueZ, marleynueTime, marleynueE;
  std::vector<double> *OpHitsPE = 0;
  std::vector<double> *OpHitsTime = 0;
  std::vector<double> *OpHitsY = 0;
  std::vector<double> *OpHitsZ = 0;
  int FlashesNum;
  std::vector<int> *FlashesTotOphits = 0;
  std::vector<double> *FlashesTotPE = 0; // Initialize to 0 for safety
  std::vector<double> *FlashesPuritySignal = 0; // Initialize to -1 for clarity
  std::vector<double> *FlashesX = 0;
  std::vector<double> *FlashesY = 0;
  std::vector<double> *FlashesZ = 0;
  std::vector<double> *FlashesTime = 0; // us, inherit from ophit
  std::vector<int> *Flashesbt = 0;
  std::vector<std::vector<int>> *FlashesOphitIndex=0;
  //std::vector<std::vector<int>> *FlashesOphitIndex = nullptr;
  std::vector<double> *bkgProcess = 0;
  std::vector<double> *bkgK = 0;


  TTree *AnaTree = (TTree*)file0->Get("QLMatchAna/AnaTree");

  AnaTree->SetBranchAddress("marleynueX",       &marleynueX);
  AnaTree->SetBranchAddress("marleynueY",       &marleynueY);
  AnaTree->SetBranchAddress("marleynueZ",       &marleynueZ);
  AnaTree->SetBranchAddress("marleynueTime",    &marleynueTime); //ns
  AnaTree->SetBranchAddress("marleynueE",       &marleynueE);
  AnaTree->SetBranchAddress("OpHitsPE",         &OpHitsPE);
  AnaTree->SetBranchAddress("OpHitsTime",       &OpHitsTime);
  AnaTree->SetBranchAddress("OpHitsY",          &OpHitsY);
  AnaTree->SetBranchAddress("OpHitsZ",          &OpHitsZ);
  AnaTree->SetBranchAddress("FlashesNum",       &FlashesNum);
  AnaTree->SetBranchAddress("FlashesTotPE",     &FlashesTotPE);
  AnaTree->SetBranchAddress("FlashesPuritySignal",  &FlashesPuritySignal);
  AnaTree->SetBranchAddress("FlashesX",         &FlashesX);
  AnaTree->SetBranchAddress("FlashesY",         &FlashesY);
  AnaTree->SetBranchAddress("FlashesZ",         &FlashesZ);
  AnaTree->SetBranchAddress("FlashesTime",      &FlashesTime);
  AnaTree->SetBranchAddress("FlashesTotOphits", &FlashesTotOphits);
  AnaTree->SetBranchAddress("Flashesbt",        &Flashesbt);
  AnaTree->SetBranchAddress("FlashesOphitIndex",&FlashesOphitIndex);
  AnaTree->SetBranchAddress("bkgProcess",       &bkgProcess);
  AnaTree->SetBranchAddress("bkgK",             &bkgK);

  // Loop over all events
  int nentries = 0; // Total input events
  nentries = AnaTree->GetEntries();
  cout<< "nentries:" << nentries<<endl;

  int count_maxpe_T0slice_goodmatch_debug = 0;
  int count_maxpe_T0slice_wrongtime_debug = 0;
  int count_maxpe_T0slice_wrongdist_debug = 0;
  int count_maxpe_T0slice_wrongtimeanddist_debug = 0;

  int count_maxpe_minophits_goodmatch_debug = 0;
  int count_maxpe_minophits_wrongtime_debug = 0;
  int count_maxpe_minophits_wrongdist_debug = 0;
  int count_maxpe_minophits_wrongtimeanddist_debug = 0;

  int count_maxpe_T0slice_minophits_goodmatch_debug = 0;
  int count_maxpe_T0slice_minophits_wrongtime_debug = 0;
  int count_maxpe_T0slice_minophits_wrongdist_debug = 0;
  int count_maxpe_T0slice_minophits_wrongtimeanddist_debug = 0;

  for ( int ientry = 0; ientry < 100; ientry++ )
  {
    AnaTree->GetEntry(ientry);

    if (ientry % 100 ==0) cout<< "@ evt " << ientry<<endl;

    double max_flash_pe_T0slice = -999;
    int max_pe_T0slice_flash_index = -1;
    double max_pe_T0slice_flash_ddist = -9999;
    double max_pe_T0slice_flash_dt = -9999;

    double max_flash_pe_minophits = -999;
    int max_pe_minophits_flash_index = -1;
    double max_pe_minophits_flash_ddist = -9999;
    double max_pe_minophits_flash_dt = -9999;

    double max_flash_pe_minophits_T0slice = -999;
    int max_pe_minophits_T0slice_flash_index = -1;
    double max_pe_minophits_T0slice_flash_ddist = -9999;
    double max_pe_minophits_T0slice_flash_dt = -9999;

    double min_flash_dist_debug = 9999.;
    double min_flash_dt_debug = 9999999.;
    double min_flash_dt_debug_sign = 9999999.;
    int min_dist_flash_index = -2;
    int min_dt_flash_index = -3;

    std::vector<double> signalflashesdistyz;
    std::vector<double> signalflashesdisttime;
    std::vector<double> signalflashespe;
    std::vector<double> signalflashesophits;
    std::vector<double> signalflashespurity;
    signalflashesdistyz.clear();
    signalflashesdisttime.clear();
    signalflashespe.clear();
    signalflashesophits.clear();
    signalflashespurity.clear();

    if (marleynueE >60 && marleynueX > -100 && marleynueX < 0) {
    //if (marleynueE <10 && marleynueX > 100) {
      // check region with poorest performance: low E, high drift

      for (int iFlash = 0; iFlash < FlashesNum; iFlash++){

        // For flashes backtrack to signal in the event
        // how far are they in spcae time, PE distribution, number of hits
        if ((*Flashesbt)[iFlash]  == -1) {
          // there are some 10s of flashes per evt
          // check the mean distance in y and z, y-z plane
          signalflashesdistyz.push_back(sqrt(pow((*FlashesY)[iFlash] - marleynueY, 2) + pow((*FlashesZ)[iFlash] - marleynueZ, 2)));
          signalflashesdisttime.push_back((*FlashesTime)[iFlash] - marleynueTime);
          signalflashespe.push_back((*FlashesTotPE)[iFlash]);
          signalflashesophits.push_back((*FlashesTotOphits)[iFlash]);
          signalflashespurity.push_back((*FlashesPuritySignal)[iFlash]);

        } // bt to signal
      } // end flash lp

      // check distance to marley true location, mean, std in y z, time, Tot PE, Tot Ophits, Purity

      if ( signalflashesdistyz.size() > 0 ) {
        // distance
        h_signalflashes_avg_dyz->Fill(std::accumulate(signalflashesdistyz.begin(), signalflashesdistyz.end(), 0.0)/signalflashesdistyz.size());
        h_signalflashes_min_dyz->Fill(*std::min_element(std::begin(signalflashesdistyz), std::end(signalflashesdistyz)));
        h_signalflashes_max_dyz->Fill(*std::max_element(std::begin(signalflashesdistyz), std::end(signalflashesdistyz)));
        // time
        h_signalflashes_avg_dt->Fill(std::accumulate(signalflashesdisttime.begin(), signalflashesdisttime.end(), 0.0)/signalflashesdisttime.size());
        h_signalflashes_min_dt->Fill(*std::min_element(std::begin(signalflashesdisttime), std::end(signalflashesdisttime)));
        h_signalflashes_max_dt->Fill(*std::max_element(std::begin(signalflashesdisttime), std::end(signalflashesdisttime)));
        // Purity
        h_signalflashes_avg_purity->Fill(std::accumulate(signalflashespurity.begin(), signalflashespurity.end(), 0.0)/signalflashespurity.size());
        h_signalflashes_min_purity->Fill(*std::min_element(std::begin(signalflashespurity), std::end(signalflashespurity)));
        h_signalflashes_max_purity->Fill(*std::max_element(std::begin(signalflashespurity), std::end(signalflashespurity)));
        // pe
        h_signalflashes_avg_pe->Fill(std::accumulate(signalflashespe.begin(), signalflashespe.end(), 0.0)/signalflashespe.size());
        h_signalflashes_min_pe->Fill(*std::min_element(std::begin(signalflashespe), std::end(signalflashespe)));
        h_signalflashes_max_pe->Fill(*std::max_element(std::begin(signalflashespe), std::end(signalflashespe)));
        // ophits
        h_signalflashes_avg_ophits->Fill(std::accumulate(signalflashesophits.begin(), signalflashesophits.end(), 0.0)/signalflashesophits.size());
        h_signalflashes_min_ophits->Fill(*std::min_element(std::begin(signalflashesophits), std::end(signalflashesophits)));
        h_signalflashes_max_ophits->Fill(*std::max_element(std::begin(signalflashesophits), std::end(signalflashesophits)));

        // check property of closest flash to true in spacetime, Tot PE, Tot Ophits, Purity
        // Get an iterator to the minimum dyz
        std::vector<double>::iterator min_dyz_it = std::min_element(std::begin(signalflashesdistyz), std::end(signalflashesdistyz));
        // Get the index:
        int min_dyz_index = std::distance(std::begin(signalflashesdistyz), min_dyz_it);
        h_signalflashes_mindyz_flash_pe->Fill(signalflashespe.at(min_dyz_index));
        h_signalflashes_mindyz_flash_ophits->Fill(signalflashesophits.at(min_dyz_index));

        // same for min dt
        std::vector<double>::iterator min_dt_it = std::min_element(std::begin(signalflashesdisttime), std::end(signalflashesdisttime));
        // Get the index:
        int min_dt_index = std::distance(std::begin(signalflashesdisttime), min_dt_it);
        h_signalflashes_mindt_flash_pe->Fill(signalflashespe.at(min_dt_index));
        h_signalflashes_mindt_flash_ophits->Fill(signalflashesophits.at(min_dt_index));

        // same for max purity
        std::vector<double>::iterator max_purity_it = std::max_element(std::begin(signalflashespurity), std::end(signalflashespurity));
        // Get the index:
        int max_purity_index = std::distance(std::begin(signalflashespurity), max_purity_it);
        h_signalflashes_maxpurity_flash_pe->Fill(signalflashespe.at(max_purity_index));
        h_signalflashes_maxpurity_flash_ophits->Fill(signalflashesophits.at(max_purity_index));
      } // end signal flash exists

      // Fill histogram

    } // marley sel selection

    double flashpestd = 0;
    double flashavghittimediff = 0;
    double flashavghityzdiff = 0;
    double flashhityzStd = 0;

    double neighbor_time = 5; //us
    double neighbor_dist = 500; //cm
    int count_neighbor_flashes = 0;
    double count_neighbor_flashes_totPE = 0;
    double count_neighbor_flashes_totophits = 0;
    double neighbor_flashes_avgophits = 0;
    double tottdiff_neighbor_flashes = 0;
    double avgtdiff_neighbor_flashes = 0;
    double avgyzdiff_neighbor_flashes = 0;
    double totyzdiff_neighbor_flashes = 0;
    double stdyzdiff_neighbor_flashes = 0;
    double neighborflash_mindt = 9999;
    double mindt_neighbor_flash_dt = 0;
    double mindt_neighbor_flash_dyz = 0;
    double mindt_neighbor_flash_totpe = 0;
    double mindt_neighbor_flash_totophits = 0;

    for (int iFlash = 0; iFlash < FlashesNum; iFlash++){

      //=================================
      // Calculate single flash property
      //=================================
      //std::vector<int> iFlashophitsindex      = FlashesOphitIndex->at(iFlash);
      const std::vector<int>& iFlashophitsindex = FlashesOphitIndex->at(iFlash);

      flashpestd = 0;
      flashavghittimediff = 0;
      flashavghityzdiff = 0;
      flashhityzStd = 0;

      for (int iophitindex = 0; iophitindex < iFlashophitsindex.size(); iophitindex++){
        flashpestd += pow((*OpHitsPE)[iFlashophitsindex.at(iophitindex)] - (*FlashesTotPE)[iFlash]/(*FlashesTotOphits)[iFlash], 2); // each - mean
        flashavghittimediff += abs( (*OpHitsTime)[iFlashophitsindex.at(iophitindex)] -  (*OpHitsTime)[iFlashophitsindex.at(0)] ); // time diff to primarty ophit
        flashavghityzdiff += sqrt(pow((*OpHitsY)[iFlashophitsindex.at(iophitindex)] - (*OpHitsY)[iFlashophitsindex.at(0)], 2) + pow((*OpHitsZ)[iFlashophitsindex.at(iophitindex)] - (*OpHitsZ)[iFlashophitsindex.at(0)], 2));
      }
      flashpestd = sqrt(flashpestd/(*FlashesTotOphits)[iFlash]); // here use N instead of N-1
      flashavghittimediff =  flashavghittimediff/(*FlashesTotOphits)[iFlash];
      flashavghityzdiff = flashavghityzdiff/(*FlashesTotOphits)[iFlash];

      // now you have avg info
      // calculate std
      for (int iophitindex = 0; iophitindex < iFlashophitsindex.size(); iophitindex++){
        flashhityzStd += pow(sqrt(pow((*OpHitsY)[iFlashophitsindex.at(iophitindex)] - (*OpHitsY)[iFlashophitsindex.at(0)], 2) + pow((*OpHitsZ)[iFlashophitsindex.at(iophitindex)] - (*OpHitsZ)[iFlashophitsindex.at(0)], 2)) - flashavghityzdiff, 2);
      }
      flashhityzStd = sqrt(flashhityzStd/(*FlashesTotOphits)[iFlash]);


      //======================================
      // Calculate neighbor flash(es) property
      // within certain space time
      //======================================
      count_neighbor_flashes = 0;
      count_neighbor_flashes_totPE = 0;
      count_neighbor_flashes_totophits = 0;
      neighbor_flashes_avgophits = 0;
      tottdiff_neighbor_flashes = 0;
      avgtdiff_neighbor_flashes = 0;
      avgyzdiff_neighbor_flashes = 0;
      totyzdiff_neighbor_flashes = 0;

      neighborflash_mindt = neighbor_time;
      mindt_neighbor_flash_dt = 0;
      mindt_neighbor_flash_dyz = 0;
      mindt_neighbor_flash_totpe = 0;
      mindt_neighbor_flash_totophits = 0;
      double flashesyzdist = 0;
      for (int jFlash = 0; jFlash < FlashesNum; jFlash++){
        if (jFlash == iFlash) continue;
        flashesyzdist = sqrt(pow((*FlashesY)[iFlash] - (*FlashesY)[jFlash], 2) + pow((*FlashesZ)[iFlash] - (*FlashesZ)[jFlash], 2));

        //within certain space time
        if ( flashesyzdist < neighbor_dist && abs((*FlashesTime)[iFlash] - (*FlashesTime)[jFlash]) < neighbor_time ) {
          count_neighbor_flashes ++;
          count_neighbor_flashes_totPE += (*FlashesTotPE)[jFlash];
          count_neighbor_flashes_totophits += (*FlashesTotOphits)[jFlash];
          tottdiff_neighbor_flashes += abs((*FlashesTime)[jFlash] - (*FlashesTime)[iFlash]);
          totyzdiff_neighbor_flashes += flashesyzdist;

          // note min dt flash
          if ( abs((*FlashesTime)[iFlash] - (*FlashesTime)[jFlash]) <= neighborflash_mindt ) {
            neighborflash_mindt = abs((*FlashesTime)[iFlash] - (*FlashesTime)[jFlash]);
            mindt_neighbor_flash_dt = neighborflash_mindt;
            mindt_neighbor_flash_dyz = flashesyzdist;
            mindt_neighbor_flash_totpe = (*FlashesTotPE)[jFlash];
            mindt_neighbor_flash_totophits = (*FlashesTotOphits)[jFlash];
          }

        } // end within certain space time

      } // end loop over neighbouring flash

      // avg properties
      if (count_neighbor_flashes > 0) {
        neighbor_flashes_avgophits = count_neighbor_flashes_totophits*1.0/count_neighbor_flashes;
        avgtdiff_neighbor_flashes  = tottdiff_neighbor_flashes/count_neighbor_flashes;
        avgyzdiff_neighbor_flashes = totyzdiff_neighbor_flashes/count_neighbor_flashes;
      }

      // Now you have avg yz distance
      // loop again to calculate std distance of neighbors
      stdyzdiff_neighbor_flashes = 0;
      for (int jFlash = 0; jFlash < FlashesNum; jFlash++){
        if (jFlash == iFlash) continue;
        flashesyzdist = sqrt(pow((*FlashesY)[iFlash] - (*FlashesY)[jFlash], 2) + pow((*FlashesZ)[iFlash] - (*FlashesZ)[jFlash], 2));

        //within certain space time
        if ( flashesyzdist < neighbor_dist && abs((*FlashesTime)[iFlash] - (*FlashesTime)[jFlash]) < neighbor_time ) {

          stdyzdiff_neighbor_flashes += pow(flashesyzdist - avgyzdiff_neighbor_flashes, 2); // each - mean

        } // end within certain space time

      } // end loop over neighbouring flash
      stdyzdiff_neighbor_flashes = sqrt(stdyzdiff_neighbor_flashes/count_neighbor_flashes); // here use N instead of N-1

      // ================================================================
      // Fill flash related vars:
      //      Understand separation power for signal/bkg
      // ================================================================
      // bkg flash
      if ( (*Flashesbt)[iFlash] > -1 ) {
        h_flash_totophits_bkg->Fill((*FlashesTotOphits)[iFlash]);
        h_flash_pe_bkg->Fill((*FlashesTotPE)[iFlash]);
        h_flash_y_bkg->Fill((*FlashesY)[iFlash]);
        h_flash_z_bkg->Fill((*FlashesZ)[iFlash]);
        h_flash_stdpe_bkg->Fill(flashpestd);
        h_flash_avgpe_bkg->Fill((*FlashesTotPE)[iFlash]/(*FlashesTotOphits)[iFlash]);
        h_flash_avgtimediff_hits_bkg->Fill(flashavghittimediff);
        h_flash_avgyzdiff_hits_bkg->Fill(flashavghityzdiff);
        h_flash_yzStd_hits_bkg->Fill(flashhityzStd);

        h_flash_n_neighbors_bkg->Fill(count_neighbor_flashes);
        h_flash_neighbors_totPE_bkg->Fill(count_neighbor_flashes_totPE);
        h_flash_neighbors_avgophits_bkg->Fill(neighbor_flashes_avgophits);
        h_flash_avgtdiff_neighbors_bkg->Fill(avgtdiff_neighbor_flashes);
        h_flash_avgyzdiff_neighbors_bkg->Fill(avgyzdiff_neighbor_flashes);
        h_flash_stdyzdiff_neighbors_bkg->Fill(stdyzdiff_neighbor_flashes);

        h_mindt_neighbor_flash_dt_bkg->Fill(mindt_neighbor_flash_dt);
        h_mindt_neighbor_flash_dyz_bkg->Fill(mindt_neighbor_flash_dyz);
        h_mindt_neighbor_flash_totPE_bkg->Fill(mindt_neighbor_flash_totpe);
        h_mindt_neighbor_flash_totophits_bkg->Fill(mindt_neighbor_flash_totophits);

      }
      else if ( (*Flashesbt)[iFlash] == -1 ) {
        // signal flash
        h_flash_totophits_marley->Fill((*FlashesTotOphits)[iFlash]);
        h_flash_pe_marley->Fill((*FlashesTotPE)[iFlash]);
        h_flash_y_marley->Fill((*FlashesY)[iFlash]);
        h_flash_z_marley->Fill((*FlashesZ)[iFlash]);
        h_flash_stdpe_marley->Fill(flashpestd);
        h_flash_avgpe_marley->Fill((*FlashesTotPE)[iFlash]/(*FlashesTotOphits)[iFlash]);
        h_flash_avgtimediff_hits_marley->Fill(flashavghittimediff);
        h_flash_avgyzdiff_hits_marley->Fill(flashavghityzdiff);
        h_flash_yzStd_hits_marley->Fill(flashhityzStd);

        h_flash_n_neighbors_marley->Fill(count_neighbor_flashes);
        h_flash_neighbors_totPE_marley->Fill(count_neighbor_flashes_totPE);
        h_flash_neighbors_avgophits_marley->Fill(neighbor_flashes_avgophits);
        h_flash_avgtdiff_neighbors_marley->Fill(avgtdiff_neighbor_flashes);
        h_flash_avgyzdiff_neighbors_marley->Fill(avgyzdiff_neighbor_flashes);
        h_flash_stdyzdiff_neighbors_marley->Fill(stdyzdiff_neighbor_flashes);

        h_mindt_neighbor_flash_dt_marley->Fill(mindt_neighbor_flash_dt);
        h_mindt_neighbor_flash_dyz_marley->Fill(mindt_neighbor_flash_dyz);
        h_mindt_neighbor_flash_totPE_marley->Fill(mindt_neighbor_flash_totpe);
        h_mindt_neighbor_flash_totophits_marley->Fill(mindt_neighbor_flash_totophits);
      }

      /*
      // ================================================================
      // Performance eval plots:
      //      Use biggest PE flash with cuts on ophits and time
      // ================================================================
      // only study purity =1 flash
      if ( (*FlashesPuritySignal)[iFlash] < 1 ) continue;

      if ( (*FlashesTotPE)[iFlash] > max_flash_pe_T0slice && (*FlashesTime)[iFlash]*1000 > signalT0 - timesliceT0 && (*FlashesTime)[iFlash]*1000 < signalT0 + timesliceT0 ) { // selection type 1
        max_flash_pe_T0slice = (*FlashesTotPE)[iFlash];
        max_pe_T0slice_flash_index = iFlash;
      }

      if ( (*FlashesTotPE)[iFlash] > max_flash_pe_minophits && (*FlashesTotOphits)[iFlash] >= flashminophits ) { // selection type 2
        max_flash_pe_minophits = (*FlashesTotPE)[iFlash];
        max_pe_minophits_flash_index = iFlash;
      }

      if ( (*FlashesTotPE)[iFlash] > max_flash_pe_minophits_T0slice && (*FlashesTotOphits)[iFlash] >= flashminophits
        && (*FlashesTime)[iFlash]*1000 > signalT0 - timesliceT0 && (*FlashesTime)[iFlash]*1000 < signalT0 + timesliceT0 ) { // selection type 3 combines 1n2
        max_flash_pe_minophits_T0slice = (*FlashesTotPE)[iFlash];
        max_pe_minophits_T0slice_flash_index = iFlash;
      }

      // ================================================================
      // DEBUG plots:
      //   Find closest flash to signal vtx
      //   as real scenario can't know true vtx to find min dis
      // ================================================================
      double iddist = 0;
      double idt = 0;
      // calculate distance
      iddist = sqrt(pow((*FlashesY)[iFlash] - marleynueY, 2) + pow((*FlashesZ)[iFlash] - marleynueZ, 2));
      idt    = (*FlashesTime)[iFlash]*1000 - marleynueTime; // Careful different time unit, marley signal nue is at 0ns
      if (iddist < min_flash_dist_debug) {
        min_flash_dist_debug = iddist;
        min_dist_flash_index = iFlash;
      }
      if (abs(idt) < min_flash_dt_debug) {
        min_flash_dt_debug = abs(idt); // keep the sign
        min_flash_dt_debug_sign = idt;
        min_dt_flash_index = iFlash;
      }
      */
    } // end loop flash

    // record max pe flash info and fill
    // only look at yz since no resolution on x
/*
    max_pe_T0slice_flash_ddist = sqrt(pow((*FlashesY)[max_pe_T0slice_flash_index] - marleynueY, 2) + pow((*FlashesZ)[max_pe_T0slice_flash_index] - marleynueZ, 2));
    max_pe_T0slice_flash_dt    = (*FlashesTime)[max_pe_T0slice_flash_index]*1000 - marleynueTime; //ns
    hmaxpe_T0slice_flash_d_res->Fill(max_pe_T0slice_flash_ddist);
    hmaxpe_T0slice_flash_t_res->Fill(max_pe_T0slice_flash_dt);

    max_pe_minophits_flash_ddist = sqrt(pow((*FlashesY)[max_pe_minophits_flash_index] - marleynueY, 2) + pow((*FlashesZ)[max_pe_minophits_flash_index] - marleynueZ, 2));
    max_pe_minophits_flash_dt    = (*FlashesTime)[max_pe_minophits_flash_index]*1000 - marleynueTime; //ns
    hmaxpe_minophit_flash_d_res->Fill(max_pe_minophits_flash_ddist);
    hmaxpe_minophit_flash_t_res->Fill(max_pe_minophits_flash_dt);

    max_pe_minophits_T0slice_flash_ddist = sqrt(pow((*FlashesY)[max_pe_minophits_T0slice_flash_index] - marleynueY, 2) + pow((*FlashesZ)[max_pe_minophits_T0slice_flash_index] - marleynueZ, 2));
    max_pe_minophits_T0slice_flash_dt    = (*FlashesTime)[max_pe_minophits_T0slice_flash_index]*1000 - marleynueTime; //ns
    hmaxpe_T0slice_minophit_flash_d_res->Fill(max_pe_minophits_T0slice_flash_ddist);
    hmaxpe_T0slice_minophit_flash_t_res->Fill(max_pe_minophits_T0slice_flash_dt);

    hdebug_flash_min_d->Fill(min_flash_dist_debug);
    hdebug_flash_min_t->Fill(min_flash_dt_debug_sign);

    if (min_dist_flash_index == max_pe_T0slice_flash_index && max_pe_T0slice_flash_index == min_dt_flash_index) count_maxpe_T0slice_goodmatch_debug++;
    if (min_dist_flash_index == max_pe_T0slice_flash_index && max_pe_T0slice_flash_index != min_dt_flash_index) count_maxpe_T0slice_wrongtime_debug++;
    if (min_dist_flash_index != max_pe_T0slice_flash_index && max_pe_T0slice_flash_index == min_dt_flash_index) count_maxpe_T0slice_wrongdist_debug++;
    if (min_dist_flash_index != max_pe_T0slice_flash_index && max_pe_T0slice_flash_index != min_dt_flash_index) count_maxpe_T0slice_wrongtimeanddist_debug++;

    if (min_dist_flash_index == max_pe_minophits_flash_index && max_pe_minophits_flash_index == min_dt_flash_index) count_maxpe_minophits_goodmatch_debug++;
    if (min_dist_flash_index == max_pe_minophits_flash_index && max_pe_minophits_flash_index != min_dt_flash_index) count_maxpe_minophits_wrongtime_debug++;
    if (min_dist_flash_index != max_pe_minophits_flash_index && max_pe_minophits_flash_index == min_dt_flash_index) count_maxpe_minophits_wrongdist_debug++;
    if (min_dist_flash_index != max_pe_minophits_flash_index && max_pe_minophits_flash_index != min_dt_flash_index) count_maxpe_minophits_wrongtimeanddist_debug++;

    if (min_dist_flash_index == max_pe_minophits_T0slice_flash_index && max_pe_minophits_T0slice_flash_index == min_dt_flash_index) count_maxpe_T0slice_minophits_goodmatch_debug++;
    if (min_dist_flash_index == max_pe_minophits_T0slice_flash_index && max_pe_minophits_T0slice_flash_index != min_dt_flash_index) count_maxpe_T0slice_minophits_wrongtime_debug++;
    if (min_dist_flash_index != max_pe_minophits_T0slice_flash_index && max_pe_minophits_T0slice_flash_index == min_dt_flash_index) count_maxpe_T0slice_minophits_wrongdist_debug++;
    if (min_dist_flash_index != max_pe_minophits_T0slice_flash_index && max_pe_minophits_T0slice_flash_index != min_dt_flash_index) count_maxpe_T0slice_minophits_wrongtimeanddist_debug++;
    */

    if (ientry < nentries ){ // all evts accumulate

      for (int ibkg = 0; ibkg < bkgProcess->size(); ibkg++){
        //cout<< "ibkg " << ibkg << " process code: "<< int((*bkgProcess)[ibkg]) <<endl;
        switch ( int((*bkgProcess)[ibkg]) ) {
          case 0:
            h_KE_bkg0->Fill((*bkgK)[ibkg]);
            break;
          case 1:
            h_KE_bkg1->Fill((*bkgK)[ibkg]);
            break;
          case 2:
            h_KE_bkg2->Fill((*bkgK)[ibkg]);
            break;
          case 3:
            h_KE_bkg3->Fill((*bkgK)[ibkg]);
            break;
          case 4:
            h_KE_bkg4->Fill((*bkgK)[ibkg]);
            break;
          case 5:
            h_KE_bkg5->Fill((*bkgK)[ibkg]);
            break;
          case 6:
            h_KE_bkg6->Fill((*bkgK)[ibkg]);
            break;
          case 7:
            h_KE_bkg7->Fill((*bkgK)[ibkg]);
            break;
          case 8:
            h_KE_bkg8->Fill((*bkgK)[ibkg]);
            break;
          case 9:
            h_KE_bkg9->Fill((*bkgK)[ibkg]);
            break;
          case 10:
            h_KE_bkg10->Fill((*bkgK)[ibkg]);
            break;
          case 11:
            h_KE_bkg11->Fill((*bkgK)[ibkg]);
            break;
          case 12:
            h_KE_bkg12->Fill((*bkgK)[ibkg]);
            break;
          case 13:
            h_KE_bkg13->Fill((*bkgK)[ibkg]);
            break;
          case 14:
            h_KE_bkg14->Fill((*bkgK)[ibkg]);
            break;
          case 15:
            h_KE_bkg15->Fill((*bkgK)[ibkg]);
            break;
          case 16:
            h_KE_bkg16->Fill((*bkgK)[ibkg]);
            break;
          case 17:
            h_KE_bkg17->Fill((*bkgK)[ibkg]);
            break;
          case 18:
            h_KE_bkg18->Fill((*bkgK)[ibkg]);
            break;
          case 19:
            h_KE_bkg19->Fill((*bkgK)[ibkg]);
            break;
          case 20:
            h_KE_bkg20->Fill((*bkgK)[ibkg]);
            break;
          case 21:
            h_KE_bkg21->Fill((*bkgK)[ibkg]);
            break;
          case 22:
            h_KE_bkg22->Fill((*bkgK)[ibkg]);
            break;
          case 23:
            h_KE_bkg23->Fill((*bkgK)[ibkg]);
            break;
          case 24:
            h_KE_bkg24->Fill((*bkgK)[ibkg]);
            break;
          case 25:
            h_KE_bkg25->Fill((*bkgK)[ibkg]);
            break;
          case 26:
            h_KE_bkg26->Fill((*bkgK)[ibkg]);
            break;
          case 27:
            h_KE_bkg27->Fill((*bkgK)[ibkg]);
            break;
          case 28:
            h_KE_bkg28->Fill((*bkgK)[ibkg]);
            break;
          default: ;
        }

      } // end loop over all bkg in the evt
    }// end select evt

  }// end ientry

  // Create output files
  TFile * outFile = new TFile("QL_performance_plots.root", "RECREATE");

  h_signalflashes_avg_dyz->Write();
  h_signalflashes_min_dyz->Write();
  h_signalflashes_max_dyz->Write();

  h_signalflashes_avg_dt->Write();
  h_signalflashes_min_dt->Write();
  h_signalflashes_max_dt->Write();

  h_signalflashes_avg_purity->Write();
  h_signalflashes_min_purity->Write();
  h_signalflashes_max_purity->Write();

  h_signalflashes_avg_pe->Write();
  h_signalflashes_min_pe->Write();
  h_signalflashes_max_pe->Write();
  h_signalflashes_mindyz_flash_pe->Write();
  h_signalflashes_mindt_flash_pe->Write();
  h_signalflashes_maxpurity_flash_pe->Write();

  h_signalflashes_avg_ophits->Write();
  h_signalflashes_min_ophits->Write();
  h_signalflashes_max_ophits->Write();
  h_signalflashes_mindyz_flash_ophits->Write();
  h_signalflashes_mindt_flash_ophits->Write();
  h_signalflashes_maxpurity_flash_ophits->Write();

  // normalize var hists of signal and bkg
  auto flashvarcomp = new TCanvas("flashvarcomp", "flashvarcomp", 700, 500);
  flashvarcomp->cd();
  flashvarcomp->SetLogy();
  h_flash_totophits_marley->Scale(1.0/h_flash_totophits_marley->GetEntries());
  h_flash_totophits_bkg->Scale(1.0/h_flash_totophits_bkg->GetEntries());
  h_flash_totophits_marley->SetLineColor(4); h_flash_totophits_marley->Draw();
  h_flash_totophits_bkg->SetLineColor(2); h_flash_totophits_bkg->Draw("SAME");
  flashvarcomp->Write("flashtotophits");

  h_flash_pe_marley->Scale(1.0/h_flash_pe_marley->GetEntries());
  h_flash_pe_bkg->Scale(1.0/h_flash_pe_bkg->GetEntries());
  h_flash_pe_marley->SetLineColor(4); h_flash_pe_marley->Draw();
  h_flash_pe_bkg->SetLineColor(2); h_flash_pe_bkg->Draw("SAME");
  flashvarcomp->Write("flashtotpe");

  h_flash_y_marley->Scale(1.0/h_flash_y_marley->GetEntries());
  h_flash_y_bkg->Scale(1.0/h_flash_y_bkg->GetEntries());
  h_flash_y_marley->SetLineColor(4); h_flash_y_marley->Draw();
  h_flash_y_bkg->SetLineColor(2); h_flash_y_bkg->Draw("SAME");
  flashvarcomp->Write("flashy");

  h_flash_z_marley->Scale(1.0/h_flash_z_marley->GetEntries());
  h_flash_z_bkg->Scale(1.0/h_flash_z_bkg->GetEntries());
  h_flash_z_marley->SetLineColor(4); h_flash_z_marley->Draw();
  h_flash_z_bkg->SetLineColor(2); h_flash_z_bkg->Draw("SAME");
  flashvarcomp->Write("flashz");

  h_flash_stdpe_marley->Scale(1.0/h_flash_stdpe_marley->GetEntries());
  h_flash_stdpe_bkg->Scale(1.0/h_flash_stdpe_bkg->GetEntries());
  h_flash_stdpe_marley->SetLineColor(4); h_flash_stdpe_marley->Draw();
  h_flash_stdpe_bkg->SetLineColor(2); h_flash_stdpe_bkg->Draw("SAME");
  flashvarcomp->Write("flashstdpe");

  h_flash_avgpe_marley->Scale(1.0/h_flash_avgpe_marley->GetEntries());
  h_flash_avgpe_bkg->Scale(1.0/h_flash_avgpe_bkg->GetEntries());
  h_flash_avgpe_marley->SetLineColor(4); h_flash_avgpe_marley->Draw();
  h_flash_avgpe_bkg->SetLineColor(2); h_flash_avgpe_bkg->Draw("SAME");
  flashvarcomp->Write("flashavgpe");

  h_flash_avgtimediff_hits_marley->Scale(1.0/h_flash_avgtimediff_hits_marley->GetEntries());
  h_flash_avgtimediff_hits_bkg->Scale(1.0/h_flash_avgtimediff_hits_bkg->GetEntries());
  h_flash_avgtimediff_hits_marley->SetLineColor(4); h_flash_avgtimediff_hits_marley->Draw();
  h_flash_avgtimediff_hits_bkg->SetLineColor(2); h_flash_avgtimediff_hits_bkg->Draw("SAME");
  flashvarcomp->Write("flashhitavgdifftime");

  h_flash_avgyzdiff_hits_marley->Scale(1.0/h_flash_avgyzdiff_hits_marley->GetEntries());
  h_flash_avgyzdiff_hits_bkg->Scale(1.0/h_flash_avgyzdiff_hits_bkg->GetEntries());
  h_flash_avgyzdiff_hits_marley->SetLineColor(4); h_flash_avgyzdiff_hits_marley->Draw();
  h_flash_avgyzdiff_hits_bkg->SetLineColor(2); h_flash_avgyzdiff_hits_bkg->Draw("SAME");
  flashvarcomp->Write("flashAvghitdyz");

  h_flash_yzStd_hits_marley->Scale(1.0/h_flash_yzStd_hits_marley->GetEntries());
  h_flash_yzStd_hits_bkg->Scale(1.0/h_flash_yzStd_hits_bkg->GetEntries());
  h_flash_yzStd_hits_marley->SetLineColor(4); h_flash_yzStd_hits_marley->Draw();
  h_flash_yzStd_hits_bkg->SetLineColor(2); h_flash_yzStd_hits_bkg->Draw("SAME");
  flashvarcomp->Write("flashStdhitdyz");



  h_flash_n_neighbors_marley->Scale(1.0/h_flash_n_neighbors_marley->GetEntries());
  h_flash_n_neighbors_bkg->Scale(1.0/h_flash_n_neighbors_bkg->GetEntries());
  h_flash_n_neighbors_bkg->SetLineColor(2); h_flash_n_neighbors_bkg->Draw();
  h_flash_n_neighbors_marley->SetLineColor(4); h_flash_n_neighbors_marley->Draw("SAME");
  flashvarcomp->Write("nNeighborflashes");

  h_flash_neighbors_totPE_marley->Scale(1.0/h_flash_neighbors_totPE_marley->GetEntries());
  h_flash_neighbors_totPE_bkg->Scale(1.0/h_flash_neighbors_totPE_bkg->GetEntries());
  h_flash_neighbors_totPE_bkg->SetLineColor(2); h_flash_neighbors_totPE_bkg->Draw();
  h_flash_neighbors_totPE_marley->SetLineColor(4); h_flash_neighbors_totPE_marley->Draw("SAME");
  flashvarcomp->Write("neighborflashesTotPE");

  h_flash_neighbors_avgophits_marley->Scale(1.0/h_flash_neighbors_avgophits_marley->GetEntries());
  h_flash_neighbors_avgophits_bkg->Scale(1.0/h_flash_neighbors_avgophits_bkg->GetEntries());
  h_flash_neighbors_avgophits_marley->SetLineColor(4); h_flash_neighbors_avgophits_marley->Draw();
  h_flash_neighbors_avgophits_bkg->SetLineColor(2); h_flash_neighbors_avgophits_bkg->Draw("SAME");
  flashvarcomp->Write("neighborflashesAvgOphits");

  h_flash_avgtdiff_neighbors_marley->Scale(1.0/h_flash_avgtdiff_neighbors_marley->GetEntries());
  h_flash_avgtdiff_neighbors_bkg->Scale(1.0/h_flash_avgtdiff_neighbors_bkg->GetEntries());
  h_flash_avgtdiff_neighbors_marley->SetLineColor(4); h_flash_avgtdiff_neighbors_marley->Draw();
  h_flash_avgtdiff_neighbors_bkg->SetLineColor(2); h_flash_avgtdiff_neighbors_bkg->Draw("SAME");
  flashvarcomp->Write("tdiff2Neighbors");

  h_flash_avgyzdiff_neighbors_marley->Scale(1.0/h_flash_avgyzdiff_neighbors_marley->GetEntries());
  h_flash_avgyzdiff_neighbors_bkg->Scale(1.0/h_flash_avgyzdiff_neighbors_bkg->GetEntries());
  h_flash_avgyzdiff_neighbors_marley->SetLineColor(4); h_flash_avgyzdiff_neighbors_marley->Draw();
  h_flash_avgyzdiff_neighbors_bkg->SetLineColor(2); h_flash_avgyzdiff_neighbors_bkg->Draw("SAME");
  flashvarcomp->Write("yzdist2NeighborsAvg");

  h_flash_stdyzdiff_neighbors_marley->Scale(1.0/h_flash_stdyzdiff_neighbors_marley->GetEntries());
  h_flash_stdyzdiff_neighbors_bkg->Scale(1.0/h_flash_stdyzdiff_neighbors_bkg->GetEntries());
  h_flash_stdyzdiff_neighbors_marley->SetLineColor(4); h_flash_stdyzdiff_neighbors_marley->Draw();
  h_flash_stdyzdiff_neighbors_bkg->SetLineColor(2); h_flash_stdyzdiff_neighbors_bkg->Draw("SAME");
  flashvarcomp->Write("yzdist2NeighborsStd");

  h_mindt_neighbor_flash_dt_marley->Scale(1.0/h_mindt_neighbor_flash_dt_marley->GetEntries());
  h_mindt_neighbor_flash_dt_bkg->Scale(1.0/h_mindt_neighbor_flash_dt_bkg->GetEntries());
  h_mindt_neighbor_flash_dt_marley->SetLineColor(4); h_mindt_neighbor_flash_dt_marley->Draw();
  h_mindt_neighbor_flash_dt_bkg->SetLineColor(2); h_mindt_neighbor_flash_dt_bkg->Draw("SAME");
  flashvarcomp->Write("mindtNeighbordt");

  h_mindt_neighbor_flash_dyz_marley->Scale(1.0/h_mindt_neighbor_flash_dyz_marley->GetEntries());
  h_mindt_neighbor_flash_dyz_bkg->Scale(1.0/h_mindt_neighbor_flash_dyz_bkg->GetEntries());
  h_mindt_neighbor_flash_dyz_marley->SetLineColor(4); h_mindt_neighbor_flash_dyz_marley->Draw();
  h_mindt_neighbor_flash_dyz_bkg->SetLineColor(2); h_mindt_neighbor_flash_dyz_bkg->Draw("SAME");
  flashvarcomp->Write("mindtNeighbordyz");

  h_mindt_neighbor_flash_totPE_marley->Scale(1.0/h_mindt_neighbor_flash_totPE_marley->GetEntries());
  h_mindt_neighbor_flash_totPE_bkg->Scale(1.0/h_mindt_neighbor_flash_totPE_bkg->GetEntries());
  h_mindt_neighbor_flash_totPE_marley->SetLineColor(4); h_mindt_neighbor_flash_totPE_marley->Draw();
  h_mindt_neighbor_flash_totPE_bkg->SetLineColor(2); h_mindt_neighbor_flash_totPE_bkg->Draw("SAME");
  flashvarcomp->Write("mindtNeighborTotPE");

  h_mindt_neighbor_flash_totophits_marley->Scale(1.0/h_mindt_neighbor_flash_totophits_marley->GetEntries());
  h_mindt_neighbor_flash_totophits_bkg->Scale(1.0/h_mindt_neighbor_flash_totophits_bkg->GetEntries());
  h_mindt_neighbor_flash_totophits_marley->SetLineColor(4); h_mindt_neighbor_flash_totophits_marley->Draw();
  h_mindt_neighbor_flash_totophits_bkg->SetLineColor(2); h_mindt_neighbor_flash_totophits_bkg->Draw("SAME");
  flashvarcomp->Write("mindtNeighborTotophits");

  /*
  // performance eval
  hmaxpe_T0slice_flash_d_res->Write();
  hmaxpe_T0slice_flash_t_res->Write();

  hmaxpe_minophit_flash_d_res->Write();
  hmaxpe_minophit_flash_t_res->Write();

  hmaxpe_T0slice_minophit_flash_d_res->Write();
  hmaxpe_T0slice_minophit_flash_t_res->Write();

  hdebug_flash_min_d->Write();
  hdebug_flash_min_t->Write();

  cout<< "=== max pe flash within T0 +/- "<< timesliceT0 << "ns ===" <<endl;
  cout<< "goodmatch_debug:" << count_maxpe_T0slice_goodmatch_debug <<endl;
  cout<< "wrongtime_debug:" << count_maxpe_T0slice_wrongtime_debug <<endl;
  cout<< "wrongdist_debug:" << count_maxpe_T0slice_wrongdist_debug <<endl;
  cout<< "wrongtimeanddist_debug:" << count_maxpe_T0slice_wrongtimeanddist_debug <<endl;

  cout<< "=== max pe flash with min "<< flashminophits << "ophits ===" <<endl;
  cout<< "goodmatch_debug:" << count_maxpe_minophits_goodmatch_debug <<endl;
  cout<< "wrongtime_debug:" << count_maxpe_minophits_wrongtime_debug <<endl;
  cout<< "wrongdist_debug:" << count_maxpe_minophits_wrongdist_debug <<endl;
  cout<< "wrongtimeanddist_debug:" << count_maxpe_minophits_wrongtimeanddist_debug <<endl;

  cout<< "=== max pe flash with T0 +/- "<< timesliceT0 << "ns, min "<< flashminophits << "ophits ===" <<endl;
  cout<< "goodmatch_debug:" << count_maxpe_T0slice_minophits_goodmatch_debug <<endl;
  cout<< "wrongtime_debug:" << count_maxpe_T0slice_minophits_wrongtime_debug <<endl;
  cout<< "wrongdist_debug:" << count_maxpe_T0slice_minophits_wrongdist_debug <<endl;
  cout<< "wrongtimeanddist_debug:" << count_maxpe_T0slice_minophits_wrongtimeanddist_debug <<endl;
  */

  auto cs = new TCanvas("cs", "cs", 700, 500);
  cs->cd();
  cs->SetLogy();
  // stack hists
  // plot bkg in decreasing rate order
  // Caverngamma(24), Ar39(0), Th232ChainGenInCathode(12), Kr85GenInLAr(1), foamGammas(25),
  // U238ChainGenInCathode (11), Th232ChainGenInAnode (15), U238ChainGenInAnode(14), K40GenInCathode(10),
  // K40GenInAnode(13), Rn222ChainPb214GenInLAr(6), Rn222ChainFromPb210GenInUpperMesh(21), Rn222ChainFromBi214GenInUpperMesh(20),
  // Rn220ChainFromPb212GenInUpperMesh(23), Rn222ChainBi214GenInLAr(7), Rn222ChainFromBi210GenInUpperMesh(22),
  // Rn222ChainRn222GenInLAr(4), Rn220ChainPb212GenInLAr(9), Rn222ChainPo218GenInLAr(5), Rn222ChainPb210GenInLAr(8),
  // Rn222ChainFromPb214GenInUpperMesh(19), Ar42GenInLAr(2), K42From42ArGenInUpperMesh(17), Rn222ChainFromPo218GenInUpperMesh(18),
  // Rn222ChainGenInPDS(16), K42From42ArGenInLAr(3), CavernwallNeutronsAtLAr(26), CryostatNGammasAtLAr(27), CavernNGammasAtLAr(28)
  h_KE_bkg24->Scale(Exposurescale); h_KE_bkg24->Write();  h_KE_bkg24->SetFillColor(38); h_KE_stack->Add(h_KE_bkg24);
  h_KE_bkg0->Scale(Exposurescale);  h_KE_bkg0->Write();   h_KE_bkg0->SetFillColor(2);   h_KE_stack->Add(h_KE_bkg0);
  h_KE_bkg12->Scale(Exposurescale); h_KE_bkg12->Write();  h_KE_bkg12->SetFillColor(3);  h_KE_stack->Add(h_KE_bkg12);
  h_KE_bkg1->Scale(Exposurescale);  h_KE_bkg1->Write();   h_KE_bkg1->SetFillColor(4);   h_KE_stack->Add(h_KE_bkg1);
  h_KE_bkg25->Scale(Exposurescale); h_KE_bkg25->Write();  h_KE_bkg25->SetFillColor(5);  h_KE_stack->Add(h_KE_bkg25);
  h_KE_bkg11->Scale(Exposurescale); h_KE_bkg11->Write();  h_KE_bkg11->SetFillColor(6);  h_KE_stack->Add(h_KE_bkg11);
  h_KE_bkg15->Scale(Exposurescale); h_KE_bkg15->Write();  h_KE_bkg15->SetFillColor(7);  h_KE_stack->Add(h_KE_bkg15);
  h_KE_bkg14->Scale(Exposurescale); h_KE_bkg14->Write();  h_KE_bkg14->SetFillColor(8);  h_KE_stack->Add(h_KE_bkg14);
  h_KE_bkg10->Scale(Exposurescale); h_KE_bkg10->Write();  h_KE_bkg10->SetFillColor(9);  h_KE_stack->Add(h_KE_bkg10);
  h_KE_bkg13->Scale(Exposurescale); h_KE_bkg13->Write();  h_KE_bkg13->SetFillColor(10); h_KE_stack->Add(h_KE_bkg13);
  h_KE_bkg6->Scale(Exposurescale);  h_KE_bkg6->Write();   h_KE_bkg6->SetFillColor(11);  h_KE_stack->Add(h_KE_bkg6);
  h_KE_bkg21->Scale(Exposurescale); h_KE_bkg21->Write();  h_KE_bkg21->SetFillColor(12); h_KE_stack->Add(h_KE_bkg21);
  h_KE_bkg20->Scale(Exposurescale); h_KE_bkg20->Write();  h_KE_bkg20->SetFillColor(13); h_KE_stack->Add(h_KE_bkg20);
  h_KE_bkg23->Scale(Exposurescale); h_KE_bkg23->Write();  h_KE_bkg23->SetFillColor(14); h_KE_stack->Add(h_KE_bkg23);
  h_KE_bkg7->Scale(Exposurescale);  h_KE_bkg7->Write();   h_KE_bkg7->SetFillColor(15);  h_KE_stack->Add(h_KE_bkg7);
  h_KE_bkg22->Scale(Exposurescale); h_KE_bkg22->Write();  h_KE_bkg22->SetFillColor(16); h_KE_stack->Add(h_KE_bkg22);
  h_KE_bkg4->Scale(Exposurescale);  h_KE_bkg4->Write();   h_KE_bkg4->SetFillColor(17);  h_KE_stack->Add(h_KE_bkg4);
  h_KE_bkg9->Scale(Exposurescale);  h_KE_bkg9->Write();   h_KE_bkg9->SetFillColor(18);  h_KE_stack->Add(h_KE_bkg9);
  h_KE_bkg5->Scale(Exposurescale);  h_KE_bkg5->Write();   h_KE_bkg5->SetFillColor(19);  h_KE_stack->Add(h_KE_bkg5);
  h_KE_bkg8->Scale(Exposurescale);  h_KE_bkg8->Write();   h_KE_bkg8->SetFillColor(20);  h_KE_stack->Add(h_KE_bkg8);
  h_KE_bkg19->Scale(Exposurescale); h_KE_bkg19->Write();  h_KE_bkg19->SetFillColor(21); h_KE_stack->Add(h_KE_bkg19);
  h_KE_bkg2->Scale(Exposurescale);  h_KE_bkg2->Write();   h_KE_bkg2->SetFillColor(22);  h_KE_stack->Add(h_KE_bkg2);
  h_KE_bkg17->Scale(Exposurescale); h_KE_bkg17->Write();  h_KE_bkg17->SetFillColor(23); h_KE_stack->Add(h_KE_bkg17);
  h_KE_bkg18->Scale(Exposurescale); h_KE_bkg18->Write();  h_KE_bkg18->SetFillColor(24); h_KE_stack->Add(h_KE_bkg18);
  h_KE_bkg16->Scale(Exposurescale); h_KE_bkg16->Write();  h_KE_bkg16->SetFillColor(25); h_KE_stack->Add(h_KE_bkg16);
  h_KE_bkg3->Scale(Exposurescale);  h_KE_bkg3->Write();   h_KE_bkg3->SetFillColor(26);  h_KE_stack->Add(h_KE_bkg3);
  h_KE_bkg26->Scale(Exposurescale); h_KE_bkg26->Write();  h_KE_bkg26->SetFillColor(27); h_KE_stack->Add(h_KE_bkg26);
  h_KE_bkg27->Scale(Exposurescale); h_KE_bkg27->Write();  h_KE_bkg27->SetFillColor(28); h_KE_stack->Add(h_KE_bkg27);
  h_KE_bkg28->Scale(Exposurescale); h_KE_bkg28->Write();  h_KE_bkg28->SetFillColor(29); h_KE_stack->Add(h_KE_bkg28);
  h_KE_stack->SetMinimum(0.5);
  h_KE_stack->Draw("nostack hist");
  cs->Write();


  outFile->Close();

  //TH2F *hnuvtx_zy;
  //TH2F *hnuvtx_zx;
  //TH2F *hnuvtx_yx;
  //TH2F *hophit_zy_t0_20us;
  //TH2F *hophit_yx_t0_20us_posz;
  //TH2F *hophit_yx_t0_20us_negz;
  //TH2F *hophit_zx_t0_20us_posy;
  //TH2F *hophit_zx_t0_20us_negy;

  //hnuvtx_zy          = tfs->make<TH2F>("hnuvtx_zy",          "edeps zy view; z [cm]; y [cm]", 210, 0, 2100, 140, -700, 700);
  //hnuvtx_zx          = tfs->make<TH2F>("hnuvtx_zx",          "edeps zx view; z [cm]; x [cm]", 210, 0, 2100, 50, -250, 250);
  //hnuvtx_yx          = tfs->make<TH2F>("hnuvtx_yx",          "edeps yx view; y [cm]; x [cm]", 140, -700, 700, 50, -250, 250);

  //hophit_zy_t0_20us  = tfs->make<TH2F>("hophit_zy_t0_20us",  "opchs zy t0 - t0+20us view; z [cm]; y [cm];", 42, 0, 2100, 28, -700, 700);
  //hophit_zx_t0_20us_posy  = tfs->make<TH2F>("hophit_zx_t0_20us_posy",  "opchs zx t0 - t0+20us view; z [cm]; x [cm];", 42, 0, 2100,   5, 0, 250); // only interested in membrane PD in upper vol.
  //hophit_zx_t0_20us_negy  = tfs->make<TH2F>("hophit_zx_t0_20us_negy",  "opchs zx t0 - t0+20us view; z [cm]; x [cm];", 42, 0, 2100,   5, 0, 250);
  //hophit_yx_t0_20us_posz  = tfs->make<TH2F>("hophit_yx_t0_20us_posz",  "opchs yx t0 - t0+20us view; y [cm]; x [cm];", 28, -700, 700, 5, 0, 250);
  //hophit_yx_t0_20us_negz  = tfs->make<TH2F>("hophit_yx_t0_20us_negz",  "opchs yx t0 - t0+20us view; y [cm]; x [cm];", 28, -700, 700, 5, 0, 250);

  // Make plots for validation
  /*if (Event == fExampleevt) {

    if (iophittime >= 0 && iophittime < fPDSTrigT0TimeWindow) {
      hophit_zy_t0_20us->Fill(iophitz, iophity, iophitpe);
      if (iophitz > 0) hophit_yx_t0_20us_posz->Fill(iophity, iophitx, iophitpe);
      if (iophitz < 0) hophit_yx_t0_20us_negz->Fill(iophity, iophitx, iophitpe);
      if (iophity > 0) hophit_zx_t0_20us_posy->Fill(iophitz, iophitx, iophitpe);
      if (iophity < 0) hophit_zx_t0_20us_negy->Fill(iophitz, iophitx, iophitpe);
    }

    //std::cout << "OpHit Num #" << iOpHit << ": PE = "<< iophitpe << ", time = " << iophittime << ", channel: " << iophitch << " X = " << iophitx << ", Y = "<< iophity << ", Z = "<< iophitz << std::endl;
  }*/


  /*
  TH2F *hedeps_zy_noEthres;
  TH2F *hedeps_yx_noEthres;
  TH2F *hedeps_zx_noEthres;
  TH2F *hedeps_zy_100keV;
  TH2F *hedeps_yx_100keV;
  TH2F *hedeps_zx_100keV;
  hedeps_zy_noEthres = tfs->make<TH2F>("hedeps_zy_noEthres", "edeps zy view; z [cm]; y [cm]", 2100, 0, 2100, 1400, -700, 700);
  hedeps_yx_noEthres = tfs->make<TH2F>("hedeps_yx_noEthres", "edeps yx view; y [cm]; x [cm]", 1400, -700, 700, 500, -250, 250);
  hedeps_zx_noEthres = tfs->make<TH2F>("hedeps_zx_noEthres", "edeps zx view; z [cm]; x [cm]", 2100, 0, 2100, 500, -250, 250);
  hedeps_zy_100keV   = tfs->make<TH2F>("hedeps_zy_100keV",   "edeps zy view; z [cm]; y [cm]", 2100, 0, 2100, 1400, -700, 700);
  hedeps_yx_100keV   = tfs->make<TH2F>("hedeps_yx_100keV",   "edeps yx view; y [cm]; x [cm]", 1400, -700, 700, 500, -250, 250);
  hedeps_zx_100keV   = tfs->make<TH2F>("hedeps_zx_100keV",   "edeps zx view; z [cm]; x [cm]", 2100, 0, 2100, 500, -250, 250);

  if (Event == fExampleevt) {

    for (size_t i=0; i<Nstep; i++) {
      edepe[i] = simedeplist[i]->Energy(); // MeV
      edepx[i] = simedeplist[i]->X(); // cm, midpoint
      edepy[i] = simedeplist[i]->Y();
      edepz[i] = simedeplist[i]->Z();
      edept[i] = simedeplist[i]->T(); // ns
      num_photons[i]   = simedeplist[i]->NumPhotons();
      num_electrons[i] = simedeplist[i]->NumElectrons();

      hedeps_zy_noEthres->Fill(edepz[i], edepy[i], edepe[i]); // x is drift
      hedeps_yx_noEthres->Fill(edepy[i], edepx[i], edepe[i]);
      hedeps_zx_noEthres->Fill(edepz[i], edepx[i], edepe[i]);
      if (edepe[i] > 0.1) {
        hedeps_zy_100keV->Fill(edepz[i], edepy[i], edepe[i]); // x is drift
        hedeps_yx_100keV->Fill(edepy[i], edepx[i], edepe[i]);
        hedeps_zx_100keV->Fill(edepz[i], edepx[i], edepe[i]);
      } // 100 keV thres

    } // end loop edeps

    // Fill the neutrino true position
    //hnuvtx_zy->Fill(marleynueZ, marleynueY, 1000); // put high weight so its visible
    //hnuvtx_zx->Fill(marleynueZ, marleynueX, 1000);
    //hnuvtx_yx->Fill(marleynueY, marleynueX, 1000);

  } // end example evt*/

  //TH2F *hflash_zy;
  //hflash_zy  = tfs->make<TH2F>("hflash_zy",  "flash zy; z [cm]; y [cm];", 42, 0, 2100, 28, -700, 700);
  // Make plots for validation
  /*if (Event == fExampleevt) {
    hflash_zy->Fill(iFlashZ, iFlashY, iFlashTotPE);
  }*/


}
