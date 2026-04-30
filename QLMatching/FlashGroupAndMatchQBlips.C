// source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
// spack load root@6.28.12
// root -l -b -q  FlashGroupAndMatchQBlips.C

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

void FlashGroupAndMatchQBlips()
{
  // 1/6 of 14kt, 9767 events, each -4.2 to +4.2 ms, bin 0.25 MeV
  Double_t Exposurescale = (1/(14.0/6)) * (1/(8.4*9767/1000/3600/24/365)) * (1/0.25);
  //int flashminophits = 2;
  //Double_t timesliceT0 = 1000; // ns this looks at flash within T0 +/- 1000ns
  //Double_t signalT0 = 0; // this number shouldn't change for all VD sample production

  // signal + bkg
  TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalandBkg/1/001/Marleyandbkg_fullstream_opcluster100ns3m_min1hits_1PE_nomemb_addshowerdir_Mar2026_reco22ana_001.root");
  // signal only
  //TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalOnly/1/001/MarleyOnly_fullstream_opcluster100ns3m_min1hit_1PE_nomemb_addshowerdir_Mar2026_reco22ana.root");

  // Flash group criteria
  double grpflashdt = 2; // us
  double grpflashdist = 5; // m
  double grpflashminophit = 2; // minimum ophits for flash to be grped
  // global parameters
  double LightROIradius = 100; //cm, radius for matching to charge object
  // calculate location in drift
  double vdmmpus = 1.606;  // mm/us
  double anodeX =  320; // coordinate in cm
  double pde = 0.03;

  gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");

  // Control plots to study grp criteria
  TH1F *h_sig_flash_timespan  = new TH1F("h_sig_flash_timespan", "Signal Flash Time Span;Time Span [#mus];Events", 200, 0, 100);
  TH1F *h_sig_flash_y_dist    = new TH1F("h_sig_flash_y_dist",  "Signal Flash Y Distance Span; per evt flashes maxY - minY [cm];Events", 200, 0, 2000);
  TH1F *h_sig_flash_z_dist    = new TH1F("h_sig_flash_z_dist",  "Signal Flash Z Distance Span; per evt flashes maxZ - minZ [cm];Events", 300, 0, 3000);
  TH1F *h_sig_flash_max_yz_dist   = new TH1F("h_sig_flash_max_yz_dist",  "Signal Flash Y-Z Plane Max Distance;per evt flashes max yz [cm];Events", 300, 0, 3000);
  TH2F *h_sig_flash_max_yz_above5m_dist_ophits   = new TH2F("h_sig_flash_max_yz_above5m_dist_ophits",  "Signal Y-Z Plane Max Distance above 5m Flash Ophits;flash1 ophits;flash2 ophits", 50, 0, 50, 50, 0, 50);
  TH2F *h_sig_flash_max_yz_below5m_dist_ophits   = new TH2F("h_sig_flash_max_yz_below5m_dist_ophits",  "Signal Y-Z Plane Max Distance above 5m Flash Ophits;flash1 ophits;flash2 ophits", 50, 0, 50, 50, 0, 50);
  TH1F *h_sig_flash_ophits    = new TH1F("h_sig_flash_ophits",   "Number of OpHits in Signal Flashes;Number of OpHits;Flashes", 50, 0, 50);
  // histogram for the Earliest vs Largest PE flash check
  // Bin 0: Not same, Bin 1: Same
  TH1F *h_sig_earliest_is_largest = new TH1F("h_sig_earliest_is_largest", "Is Earliest Flash the Largest PE?;0=No, 1=Yes;Events", 2, 0, 2);
  TH2F *h_sig_maxdist_above5m_flash_2ophits_samech = new TH2F("h_sig_maxdist_above5m_flash_2ophits_samech", "Is two hit Flash the same channel?;Flash 1: 0=No, 1=Yes;Flash 2: 0=No, 1=Yes", 2, 0, 2,  2, 0, 2);
  TH2F *h_sig_maxdist_below5m_flash_2ophits_samech = new TH2F("h_sig_maxdist_below5m_flash_2ophits_samech", "Is two hit Flash the same channel?;Flash 1: 0=No, 1=Yes;Flash 2: 0=No, 1=Yes", 2, 0, 2,  2, 0, 2);
  //TH2F *h_sig_flash_time_vs_pe = new TH2F("h_sig_flash_time_vs_pe", "Signal Flashes: PE vs. Time;Flash Time [#mus];Total PE", 500, 0, 5000, 100, 0, 4000);

  // Evaluate group flash
  TH2F *h_grp_flash_totpe_purity = new TH2F("h_grp_flash_totpe_purity",  "h_grp_flash_totpe_purity; group flash tot pe; group flash purity", 100, 0, 4000, 10, 0, 1);
  TH2F *h_grp_flash_totpe_time = new TH2F("h_grp_flash_totpe_time",  "h_grp_flash_totpe_time;  group flash tot pe; group flash time [us]", 100, 0, 4000, 1000, 0, 5000);
  TH2F *h_grp_flash_totophits_purity = new TH2F("h_grp_flash_totophits_purity",  "h_grp_flash_totophits_purity; group flash tot ophits; group flash purity", 20, 0, 20, 10, 0, 1);
  TH2F *h_grp_flash_totophits_time = new TH2F("h_grp_flash_totophits_time",  "h_grp_flash_totophits_time; group flash tot ophits; group flash time [us]", 20, 0, 20, 1000, 0, 5000);
  TH2F *h_grp_flash_totflashes_purity = new TH2F("h_grp_flash_totflashes_purity",  "h_grp_flash_totflashes_purity; group tot flashes; group flash purity", 20, 0, 20, 10, 0, 1);
  TH2F *h_grp_flash_totflashes_time = new TH2F("h_grp_flash_totflashes_time",  "h_grp_flash_totflashes_time; group tot flashes; group flash time [us]", 20, 0, 20, 1000, 0, 5000);

  TH1F *h_selflashgrp_t_res   = new TH1F("h_selflashgrp_t_res",    "h_selflashgrp_t_res;t_{highest purity flash grp - true nue} [ns]", 400, -20000, 20000);
  TH1F *h_selflashgrp_flashPEweighted_dyz_res = new TH1F("h_selflashgrp_flashPEweighted_dyz_res",     "h_selflashgrp_flashPEweighted_dyz_res;    d_{| highest purity flash grp - true nue |, yz} [cm]", 210, 0, 2100);
  TH1F *h_selflashgrp_earliestflash_dyz_res   = new TH1F("h_selflashgrp_earliestflash_dyz_res",  "h_selflashgrp_earliestflash_dyz_res; d_{| highest purity flash grp - true nue |, yz} [cm]", 210, 0, 2100);

  TH1F *h_sel_flashgrp_t_res   = new TH1F("h_sel_flashgrp_t_res",    "h_sel_flashgrp_t_res;t_{selected flash grp - true nue} [ns]", 400, -20000, 20000);
  TH1F *h_sel_flashgrp_dyz_res = new TH1F("h_sel_flashgrp_dyz_res",  "h_sel_flashgrp_dyz_res; d_{| selected flash grp - true nue |, yz} [cm]", 210, 0, 2100);

  TH1F *h_marleyX = new TH1F("h_marleyX",     "h_marleyX;    marley true X [cm]", 40, -400, 400);
  TH1F *h_marleyE = new TH1F("h_marleyE",     "h_marleyE;    marley true E [MeV]", 80, 0, 80);

  //TH2F *h_maxblipn2ll_evol   = new TH2F("h_maxblipn2ll_evol",    "h_maxblipn2ll_evol;blip number;blipn2ll", 10, 0, 10, 100, -100, 100);

  TH1F *h_selflashgrp_selblip_X_res = new TH1F("h_selflashgrp_selblip_X_res",     "h_selflashgrp_selblip_X_res;    blip x - true blip x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_selblip_Y_res = new TH1F("h_selflashgrp_selblip_Y_res",     "h_selflashgrp_selblip_Y_res;    blip y - true blip y [cm]", 80, -800, 800);
  TH1F *h_selflashgrp_selblip_Z_res = new TH1F("h_selflashgrp_selblip_Z_res",     "h_selflashgrp_selblip_Z_res;    blip z - true blip z [cm]", 210, 0, 2100);
  TH1F *h_selflashgrp_selblip_T_res = new TH1F("h_selflashgrp_selblip_T_res",     "h_selflashgrp_selblip_T_res;    blip t - true blip t [us]", 400, -4000, 4000);
  TH1F *h_selflashgrp_selblip_E_res = new TH1F("h_selflashgrp_selblip_E_res",     "h_selflashgrp_selblip_E_res;    blip E - true blip E [MeV]", 400, -100, 100);
  TH1F *h_selflashgrp_maxn2llblip_bt    = new TH1F("h_selflashgrp_maxn2llblip_bt",        "h_selflashgrp_maxn2llblip_bt;       true processes", 32, -2, 30);

  // check performance at event level
  TH1F *h_selflashgrp_selblip_evt_X_res = new TH1F("h_selflashgrp_selblip_evt_X_res",     "h_selflashgrp_selblip_evt_X_res;    blip x - true marley x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_selblip_evt_Y_res = new TH1F("h_selflashgrp_selblip_evt_Y_res",     "h_selflashgrp_selblip_evt_Y_res;    blip y - true marley y [cm]",  80, -800, 800);
  TH1F *h_selflashgrp_selblip_evt_Z_res = new TH1F("h_selflashgrp_selblip_evt_Z_res",     "h_selflashgrp_selblip_evt_Z_res;    blip z - true marley z [cm]", 210, 0, 2100);
  TH1F *h_selflashgrp_selblip_evt_T_res = new TH1F("h_selflashgrp_selblip_evt_T_res",     "h_selflashgrp_selblip_evt_T_res;    blip t - true marley t [us]", 400, -4000, 4000);

  TH1F *h_selflashgrp_matched_blipbt2signal_marleyX = new TH1F("h_selflashgrp_matched_blipbt2signal_marleyX",     "h_selflashgrp_matched_blipbt2signal_marleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_blipbt2signal_marleyE = new TH1F("h_selflashgrp_matched_blipbt2signal_marleyE",     "h_selflashgrp_matched_blipbt2signal_marleyE;    marley true E [MeV]", 80, 0, 80);

  TH1F *h_sig_preROImatch_flash2shower_dist   = new TH1F("h_sig_preROImatch_flash2shower_dist",  "Pre ROI match Signal Flash to Shower distance;distance [cm]", 300, 0, 3000);

  TH1F *h_maxshowern2ll   = new TH1F("h_maxshowern2ll",    "h_maxshowern2ll; showern2ll", 100, -100, 100);
  TH1F *h_control_plt_dy_flashgrp_tpcbtshowerstart   = new TH1F("h_control_plt_dy_flashgrp_tpcbtshowerstart",   "h_control_plt_dy_flashgrp_tpcbtshowerstart;   dy (flashgrp y - shower start y)[cm]", 400, -2000, 2000);
  TH1F *h_control_plt_dz_flashgrp_tpcbtshowerstart   = new TH1F("h_control_plt_dz_flashgrp_tpcbtshowerstart",   "h_control_plt_dz_flashgrp_tpcbtshowerstart;   dz (flashgrp z - shower start z)[cm]", 600, -3000, 3000);
  TH1F *h_control_plt_dyz_flashgrp_tpcbtshowerstart  = new TH1F("h_control_plt_dyz_flashgrp_tpcbtshowerstart",  "h_control_plt_dyz_flashgrp_tpcbtshowerstart;  dyz (flashgrp - shower start)[cm]",    300, 0, 3000);
  TH1F *h_control_plt_dy_flashgrp_tpcbtshowercenter  = new TH1F("h_control_plt_dy_flashgrp_tpcbtshowercenter",  "h_control_plt_dy_flashgrp_tpcbtshowercenter;  dy (flashgrp y - shower center y)[cm]", 400, -2000, 2000);
  TH1F *h_control_plt_dz_flashgrp_tpcbtshowercenter  = new TH1F("h_control_plt_dz_flashgrp_tpcbtshowercenter",  "h_control_plt_dz_flashgrp_tpcbtshowercenter;  dz (flashgrp z - shower center z)[cm]", 600, -3000, 3000);
  TH1F *h_control_plt_dyz_flashgrp_tpcbtshowercenter = new TH1F("h_control_plt_dyz_flashgrp_tpcbtshowercenter", "h_control_plt_dyz_flashgrp_tpcbtshowercenter; dyz (flashgrp - shower center)[cm]",    300, 0, 3000);

  TH1F *h_showerreco_marleyX = new TH1F("h_showerreco_marleyX",     "h_showerreco_marleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_showerreco_marleyE = new TH1F("h_showerreco_marleyE",     "h_showerreco_marleyE;    marley true E [MeV]", 80, 0, 80);
  TH1F *h_signalshowerreco_marleyX = new TH1F("h_signalshowerreco_marleyX",     "h_signalshowerreco_marleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_signalshowerreco_marleyE = new TH1F("h_signalshowerreco_marleyE",     "h_signalshowerreco_marleyE;    marley true E [MeV]", 80, 0, 80);
  TH1F *h_signalshowerreco_6MeVmarleyX = new TH1F("h_signalshowerreco_6MeVmarleyX",     "h_signalshowerreco_6MeVmarleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_signalshowerreco_10MeVmarleyX = new TH1F("h_signalshowerreco_10MeVmarleyX",     "h_signalshowerreco_10MeVmarleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_signalshowerreco_18MeVmarleyX = new TH1F("h_signalshowerreco_18MeVmarleyX",     "h_signalshowerreco_18MeVmarleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_signalshowerreco_25MeVmarleyX = new TH1F("h_signalshowerreco_25MeVmarleyX",     "h_signalshowerreco_25MeVmarleyX;    marley true X [cm]",  40, -400, 400);
  TH1F *h_signalshowerreco_50MeVmarleyX = new TH1F("h_signalshowerreco_50MeVmarleyX",     "h_signalshowerreco_50MeVmarleyX;    marley true X [cm]",  40, -400, 400);

  TH1F *h_selflashgrp_matched_showerbt2signal_marleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_marleyX",     "h_selflashgrp_matched_showerbt2signal_marleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2signal_marleyE = new TH1F("h_selflashgrp_matched_showerbt2signal_marleyE",     "h_selflashgrp_matched_showerbt2signal_marleyE;    marley true E [MeV]", 80, 0, 80);
  TH1F *h_selflashgrp_matched_showerbt2bkg_marleyX = new TH1F("h_selflashgrp_matched_showerbt2bkg_marleyX",     "h_selflashgrp_matched_showerbt2bkg_marleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2bkg_marleyE = new TH1F("h_selflashgrp_matched_showerbt2bkg_marleyE",     "h_selflashgrp_matched_showerbt2bkg_marleyE;    marley true E [MeV]", 80, 0, 80);
  // breakdown by energy
  TH1F *h_selflashgrp_matched_showerbt2signal_6MeVmarleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_6MeVmarleyX",     "h_selflashgrp_matched_showerbt2signal_6MeVmarleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2signal_10MeVmarleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_10MeVmarleyX",     "h_selflashgrp_matched_showerbt2signal_10MeVmarleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2signal_18MeVmarleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_18MeVmarleyX",     "h_selflashgrp_matched_showerbt2signal_18MeVmarleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2signal_25MeVmarleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_25MeVmarleyX",     "h_selflashgrp_matched_showerbt2signal_25MeVmarleyX;    marley true x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_matched_showerbt2signal_50MeVmarleyX = new TH1F("h_selflashgrp_matched_showerbt2signal_50MeVmarleyX",     "h_selflashgrp_matched_showerbt2signal_50MeVmarleyX;    marley true x [cm]", 40, -400, 400);

  // Post shower matching performance
  TH1F *h_selflashgrp_matched_showerbt2signal_pdsrecoT0 = new TH1F("h_selflashgrp_matched_showerbt2signal_pdsrecoT0",   "h_selflashgrp_matched_showerbt2signal_pdsrecoT0;    flash matched shower pds t0 [us]", 400, -4000, 4000);
  TH1F *h_selflashgrp_matched_showerbt2signal_tpcrecoX_t0corr = new TH1F("h_selflashgrp_matched_showerbt2signal_tpcrecoX_t0corr",     "h_selflashgrp_matched_showerbt2signal_tpcrecoX_t0corr;    flash matched tpc shower x t0 corrected [cm]", 40, -400, 400);

  TH1F *h_selflashgrp_matched_showerbt2signal_tpcrecoY = new TH1F("h_selflashgrp_matched_showerbt2signal_tpcrecoY",     "h_selflashgrp_matched_showerbt2signal_tpcrecoY;    flash matched shower tpc y [cm]", 80, -800, 800);
  TH1F *h_selflashgrp_matched_showerbt2signal_tpcrecoZ = new TH1F("h_selflashgrp_matched_showerbt2signal_tpcrecoZ",     "h_selflashgrp_matched_showerbt2signal_tpcrecoZ;    flash matched shower tpc z [cm]", 210, 0, 2100);
  TH1F *h_selflashgrp_matched_showerbt2signal_tpcrecoE = new TH1F("h_selflashgrp_matched_showerbt2signal_tpcrecoE",     "h_selflashgrp_matched_showerbt2signal_tpcrecoE;    flash matched shower tpc E [MeV]", 80, 0, 80);

  TH1F *h_selflashgrp_matched_showerbt2signal_pdsrecoY = new TH1F("h_selflashgrp_matched_showerbt2signal_pdsrecoY",     "h_selflashgrp_matched_showerbt2signal_pdsrecoY;    flash matched shower pds y [cm]", 80, -800, 800);
  TH1F *h_selflashgrp_matched_showerbt2signal_pdsrecoZ = new TH1F("h_selflashgrp_matched_showerbt2signal_pdsrecoZ",     "h_selflashgrp_matched_showerbt2signal_pdsrecoZ;    flash matched shower pds z [cm]", 210, 0, 2100);

  //  tpc reco resolution
  TH1F *h_selflashgrp_selshower_evt_X_tpc_res = new TH1F("h_selflashgrp_selshower_evt_X_tpc_res",     "h_selflashgrp_selshower_evt_X_tpc_res;    matched shower tpc x corrected - true marley x [cm]", 40, -400, 400);
  TH1F *h_selflashgrp_selshower_evt_Y_tpc_res = new TH1F("h_selflashgrp_selshower_evt_Y_tpc_res",     "h_selflashgrp_selshower_evt_Y_tpc_res;    matched shower tpc y - true marley y [cm]",  80, -800, 800);
  TH1F *h_selflashgrp_selshower_evt_Z_tpc_res = new TH1F("h_selflashgrp_selshower_evt_Z_tpc_res",     "h_selflashgrp_selshower_evt_Z_tpc_res;    matched shower tpc z - true marley z [cm]", 210, 0, 2100);

  TH1F *h_selflashgrp_selshower_evt_T_pds_res = new TH1F("h_selflashgrp_selshower_evt_T_pds_res",     "h_selflashgrp_selshower_evt_T_pds_res;    matched shower flash t - true marley t [us]", 400, -4000, 4000);
  TH1F *h_selflashgrp_selshower_evt_Y_pds_res = new TH1F("h_selflashgrp_selshower_evt_Y_pds_res",     "h_selflashgrp_selshower_evt_Y_pds_res;    matched shower flash y - true marley y [cm]",  80, -800, 800);
  TH1F *h_selflashgrp_selshower_evt_Z_pds_res = new TH1F("h_selflashgrp_selshower_evt_Z_pds_res",     "h_selflashgrp_selshower_evt_Z_pds_res;    matched shower flash z - true marley z [cm]", 210, 0, 2100);


  // these are branches you want to read from the above root file
  Double_t marleynueX, marleynueY, marleynueZ, marleynueTime, marleynueE;
  std::vector<double> *OpHitsPE = 0;
  std::vector<double> *OpHitsTime = 0;
  std::vector<double> *OpHitsY = 0;
  std::vector<double> *OpHitsZ = 0;
  std::vector<double> *OpHitsChannel = 0;
  int FlashesNum, nblips, ShowerNum;
  std::vector<int> *FlashesTotOphits = 0;
  std::vector<double> *FlashesTotPE = 0; // Initialize to 0 for safety
  std::vector<double> *FlashesPuritySignal = 0; // Initialize to -1 for clarity
  std::vector<double> *FlashesX = 0;
  std::vector<double> *FlashesY = 0;
  std::vector<double> *FlashesZ = 0;
  std::vector<double> *FlashesTime = 0; // us, inherit from ophit
  std::vector<int> *Flashesbt = 0;
  std::vector<std::vector<int>> *FlashesOphitIndex=0;
  std::vector<double> *blipY = 0;
  std::vector<double> *blipZ = 0;
  std::vector<double> *blipDriftT = 0;
  std::vector<double> *blipE = 0;
  std::vector<double> *blipX_true = 0;
  std::vector<double> *blipY_true = 0;
  std::vector<double> *blipZ_true = 0;
  std::vector<double> *blipT_true = 0;
  std::vector<double> *blipE_true = 0;
  std::vector<double> *blipbt = 0;
  std::vector<double> *ShowerStartX = 0;
  std::vector<double> *ShowerStartY = 0;
  std::vector<double> *ShowerStartZ = 0;
  std::vector<double> *ShowerE = 0;
  std::vector<double> *Showerbt = 0;
  std::vector<double> *ShowerLength = 0;
  std::vector<double> *ShowerDCosStartX = 0;
  std::vector<double> *ShowerDCosStartY = 0;
  std::vector<double> *ShowerDCosStartZ = 0;

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
  AnaTree->SetBranchAddress("OpHitsChannel",    &OpHitsChannel);
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
  AnaTree->SetBranchAddress("nblips",           &nblips);
  AnaTree->SetBranchAddress("blipY",            &blipY);
  AnaTree->SetBranchAddress("blipZ",            &blipZ);
  AnaTree->SetBranchAddress("blipE",            &blipE);
  AnaTree->SetBranchAddress("blipDriftT",       &blipDriftT);
  AnaTree->SetBranchAddress("blipX_true",       &blipX_true);
  AnaTree->SetBranchAddress("blipY_true",       &blipY_true);
  AnaTree->SetBranchAddress("blipZ_true",       &blipZ_true);
  AnaTree->SetBranchAddress("blipT_true",       &blipT_true);
  AnaTree->SetBranchAddress("blipE_true",       &blipE_true);
  AnaTree->SetBranchAddress("blipbt",           &blipbt);
  AnaTree->SetBranchAddress("ShowerNum",        &ShowerNum);
  AnaTree->SetBranchAddress("ShowerStartX",     &ShowerStartX);
  AnaTree->SetBranchAddress("ShowerStartY",     &ShowerStartY);
  AnaTree->SetBranchAddress("ShowerStartZ",     &ShowerStartZ);
  AnaTree->SetBranchAddress("ShowerE",          &ShowerE);
  AnaTree->SetBranchAddress("Showerbt",         &Showerbt);
  AnaTree->SetBranchAddress("ShowerLength",     &ShowerLength);
  AnaTree->SetBranchAddress("ShowerDCosStartX", &ShowerDCosStartX);
  AnaTree->SetBranchAddress("ShowerDCosStartY", &ShowerDCosStartY);
  AnaTree->SetBranchAddress("ShowerDCosStartZ", &ShowerDCosStartZ);

  // Loop over all events
  int nentries = 0; // Total input events
  nentries = AnaTree->GetEntries();
  cout<< "nentries:" << nentries<<endl;

  std::vector<double> FlashesUsedinGroup;

  for ( int ientry = 0; ientry < nentries; ientry++ )
  {
    AnaTree->GetEntry(ientry);

    if (ientry % 100 ==0) cout<< "@ evt " << ientry<<endl;

    // =========================================================================
    // control plot
    // study signal and bkg flashes distribution in time, distance, and ophits
    // =========================================================================
    // =========================================================================
    // Study signal flashes (FlashesPuritySignal > 0) per event
    // =========================================================================
    std::vector<int> sigFlashIndices;
    double maxSigPE = -1.0;
    double minSigTime = 999999.0;
    int maxPEIdx = -1;
    int earliestIdx = -1;
    for (int f = 0; f < FlashesNum; ++f) {
      if ( (*FlashesPuritySignal)[f] > 0  &&  (*FlashesTotOphits)[f] >= grpflashminophit ) {
        sigFlashIndices.push_back(f);
        //double fTime = (*FlashesTime)[f];
        //double fPE   = (*FlashesTotPE)[f];

        // Fill the Time vs PE plot
        //h_sig_flash_time_vs_pe->Fill(fTime, fPE);
        h_sig_flash_ophits->Fill((*FlashesTotOphits)[f]);

        // Track Largest PE
        if ((*FlashesTotPE)[f] > maxSigPE) {
          maxSigPE = (*FlashesTotPE)[f];
          maxPEIdx = f;
        }
        // Track Earliest Time
        if ((*FlashesTime)[f] < minSigTime) {
          minSigTime = (*FlashesTime)[f];
          earliestIdx = f;
        }

      } // end signal flash check if
    }// end loop over flash

    if (!sigFlashIndices.empty()) {
      // Check if Earliest is Largest
      if (maxPEIdx != -1 && earliestIdx != -1) {
        if (maxPEIdx == earliestIdx) h_sig_earliest_is_largest->Fill(1);
        else h_sig_earliest_is_largest->Fill(0);
      }
    }

    // --- Absolute Max Distance Calculation for Signal Flashes ---
    if (sigFlashIndices.size() > 1) {
      double minT = 99999, maxT = -99999;
      double minY = 99999, maxY = -99999;
      double minZ = 99999, maxZ = -99999;
      double maxYZDistInEvent = -999;
      int flash1ophits = 0;
      int flash2ophits = 0;
      int flash1ophitssamechannel = 0;
      int flash2ophitssamechannel = 0;

      // Nested loop to check every unique pair of signal flashes
      for (size_t i = 0; i < sigFlashIndices.size(); ++i) {
          int idx1 = sigFlashIndices.at(i);

          double t = (*FlashesTime)[idx1];
          double y = (*FlashesY)[idx1];
          double z = (*FlashesZ)[idx1];

          if (t < minT) minT = t;
          if (t > maxT) maxT = t;
          if (y < minY) minY = y;
          if (y > maxY) maxY = y;
          if (z < minZ) minZ = z;
          if (z > maxZ) maxZ = z;

          for (size_t j = i + 1; j < sigFlashIndices.size(); ++j) {
              int idx2 = sigFlashIndices.at(j);

              // Calculate 3D distance between flash i and flash j
              //double dx = (*FlashesX)[idx1] - (*FlashesX)[idx2]; // this is zero for cathode PDS
              double dy = (*FlashesY)[idx1] - (*FlashesY)[idx2];
              double dz = (*FlashesZ)[idx1] - (*FlashesZ)[idx2];

              double dist = std::sqrt(dy*dy + dz*dz);

              if (dist > maxYZDistInEvent) {
                  maxYZDistInEvent = dist;
                  // register the two flash ophits
                  flash1ophits = (*FlashesTotOphits)[idx1];
                  flash2ophits = (*FlashesTotOphits)[idx2];
                  // if the flash has two ophits, register whether they are the same detector channel
                  if (flash1ophits == 2){
                    const std::vector<int>& idx1ophitsindex = FlashesOphitIndex->at(idx1);
                    if ( (*OpHitsChannel)[idx1ophitsindex.at(0)] == (*OpHitsChannel)[idx1ophitsindex.at(1)] ) {
                      flash1ophitssamechannel = 1;
                    } // same channel
                  } // two ophits
                  if (flash2ophits == 2){
                    const std::vector<int>& idx2ophitsindex = FlashesOphitIndex->at(idx2);
                    if ( (*OpHitsChannel)[idx2ophitsindex.at(0)] == (*OpHitsChannel)[idx2ophitsindex.at(1)] ) {
                      flash2ophitssamechannel = 1;
                    } // same channel
                  } // two ophits
              }
          } // end second flash loop

      } // end first flash lp

      h_sig_flash_timespan->Fill(maxT - minT);
      h_sig_flash_y_dist->Fill(maxY - minY);
      h_sig_flash_z_dist->Fill(maxZ - minZ);
      h_sig_flash_max_yz_dist->Fill(maxYZDistInEvent);
      if (maxYZDistInEvent > 500) {
        // large distance bt flashes, what's going on?
        // check flash hits
        h_sig_flash_max_yz_above5m_dist_ophits->Fill(flash1ophits, flash2ophits);
        h_sig_maxdist_above5m_flash_2ophits_samech->Fill(flash1ophitssamechannel, flash2ophitssamechannel);
      } else { // normal expected
        // check flash hits
        h_sig_flash_max_yz_below5m_dist_ophits->Fill(flash1ophits, flash2ophits);
        h_sig_maxdist_below5m_flash_2ophits_samech->Fill(flash1ophitssamechannel, flash2ophitssamechannel);
      }

    } else if (sigFlashIndices.size() == 1) {
        // If only one signal flash exists, the maximum distance is zero
        h_sig_flash_timespan->Fill(0); // Only one flash, span is zero
        h_sig_flash_y_dist->Fill(0);
        h_sig_flash_z_dist->Fill(0);
        h_sig_flash_max_yz_dist->Fill(0.0);
        h_sig_flash_max_yz_below5m_dist_ophits->Fill(-1, -1);
        h_sig_flash_max_yz_above5m_dist_ophits->Fill(-1, -1);
        h_sig_maxdist_above5m_flash_2ophits_samech->Fill(-1, -1);
        h_sig_maxdist_below5m_flash_2ophits_samech->Fill(-1, -1);
    }



    // ================================================================
    // Group flashes at event level - build event flashes grp
    // same physics event should produce flashes close in space and time
    // *************************************************
    // criteria: 2us, 5m, earliest flash has largest PE
    // *************************************************
    // ================================================================
    //
    // per event can have many grps, per grp of flash has no limit of number of flahses
    // but signal events can have 0 flash up to tens of flashes
    // understand how many grps can be built, and use the grp that has largest # of flashes
    // build flash group property: totPE, tothits, x, y, z, t, purity, bt, flashindex
    // ================================================================
    FlashesUsedinGroup.clear();
    FlashesUsedinGroup.resize(FlashesNum, 0);
    double iFlashGroupX = -9999.;
    double iFlashGroupY = -9999.;
    double iFlashGroupZ = -9999.;
    double iFlashGroupT = -9999.;
    double iFlashGroupEarliestFlashY = -9999.;
    double iFlashGroupEarliestFlashZ = -9999.;
    double iFlashGroupTotPE = 0;
    int iFlashGroupTotOphits = 0;
    double iFlashGroupPuritySig = 0;
    double GroupmaxPEFlashindex = -1; // index of maxPE flash
    double GroupearliestFlashindex = -1; // index of earliest flash
    double GroupFlashmaxPE = 0;
    double GroupFlashearliesttime = 9999;
    //int selFlashGroupTotOphits = 0;
    //double selFlashGroupTotPE = 0;
    //int selFlashGroupFlashNum = 0;
    //double selFlashGroupX = 0;
    //double selFlashGroupY = 0;
    //double selFlashGroupZ = 0;
    //double selFlashGroupT = 0;
    double ctrlPurestFlashGroupDist = 9999;
    double ctrlPurestFlashGroupPurity = 0;
    double ctrlPurestFlashGroupTotPE = 0;
    int ctrlPurestFlashGroupFlashNum = 0;
    double ctrlPurestFlashGroupX;
    double ctrlPurestFlashGroupY, ctrlPurestFlashGroupEarliestFlashY;
    double ctrlPurestFlashGroupZ, ctrlPurestFlashGroupEarliestFlashZ;
    double ctrlPurestFlashGroupT;

    double ctrlClosestFlashGroupDist = 9999;
    double ctrlClosestFlashGroupPurity = 0;
    double ctrlClosestFlashGroupTotPE = 0;
    int ctrlClosestFlashGroupFlashNum = 0;
    double ctrlClosestFlashGroupX;
    double ctrlClosestFlashGroupY, ctrlClosestFlashGroupEarliestFlashY;
    double ctrlClosestFlashGroupZ, ctrlClosestFlashGroupEarliestFlashZ;
    double ctrlClosestFlashGroupT;
    std::vector<double> FlashGroupTotPE, FlashGroupX, FlashGroupY, FlashGroupZ, FlashGroupTime, FlashGroupSignalPurity;
    std::vector<int> FlashGroupFlashNum, FlashGroupTotOphits, iFlashGroupFlashIndex;
    std::vector<double> iFlashGroupOphitsPE, iFlashGroupOphitsY, iFlashGroupOphitsZ, iFlashGroupOphitsT;
    std::vector<double> ctrlPurestFlashGroupOphitsPE, ctrlPurestFlashGroupOphitsY, ctrlPurestFlashGroupOphitsZ, ctrlPurestFlashGroupOphitsT;
    std::vector<double> ctrlClosestFlashGroupOphitsPE, ctrlClosestFlashGroupOphitsY, ctrlClosestFlashGroupOphitsZ, ctrlClosestFlashGroupOphitsT;

    for (int iFlash = 0; iFlash < FlashesNum; iFlash++){

      // ophit cut
      if (  (*FlashesTotOphits)[iFlash] < grpflashminophit ) continue;

      if (FlashesUsedinGroup.at(iFlash) == 1) continue; // skip flash already used in group of flashes

      // iFlash is not the max PE flash
      iFlashGroupX = (*FlashesX)[iFlash]*(*FlashesTotPE)[iFlash];
      iFlashGroupY = (*FlashesY)[iFlash]*(*FlashesTotPE)[iFlash];
      iFlashGroupZ = (*FlashesZ)[iFlash]*(*FlashesTotPE)[iFlash];
      iFlashGroupTotPE = (*FlashesTotPE)[iFlash];
      iFlashGroupTotOphits = (*FlashesTotOphits)[iFlash];
      iFlashGroupPuritySig = (*FlashesPuritySignal)[iFlash];

      GroupFlashmaxPE = (*FlashesTotPE)[iFlash];
      GroupmaxPEFlashindex = iFlash;
      GroupFlashearliesttime = (*FlashesTime)[iFlash];
      GroupearliestFlashindex = iFlash;
      iFlashGroupEarliestFlashY = (*FlashesY)[iFlash];
      iFlashGroupEarliestFlashZ = (*FlashesZ)[iFlash];

      // Take note flashes used in this grp
      iFlashGroupFlashIndex.clear();
      iFlashGroupFlashIndex.push_back(iFlash);

      // Store all ophits PE, Y, Z, T in this flash
      // for later to calculate likelihood with charge cluster
      iFlashGroupOphitsPE.clear();
      iFlashGroupOphitsY.clear();
      iFlashGroupOphitsZ.clear();
      iFlashGroupOphitsT.clear();
      const std::vector<int>& iFlashophitsindex = FlashesOphitIndex->at(iFlash);
      for (int iophitindex = 0; iophitindex < iFlashophitsindex.size(); iophitindex++){
        iFlashGroupOphitsPE.push_back( (*OpHitsPE)[iFlashophitsindex.at(iophitindex)] );
        iFlashGroupOphitsY.push_back( (*OpHitsY)[iFlashophitsindex.at(iophitindex)] );
        iFlashGroupOphitsZ.push_back( (*OpHitsZ)[iFlashophitsindex.at(iophitindex)] );
        iFlashGroupOphitsT.push_back( (*OpHitsTime)[iFlashophitsindex.at(iophitindex)] );
      }


      for (int jFlash = 0; jFlash < FlashesNum; jFlash++){
        if (jFlash == iFlash) continue;

        // ophit cut
        if (  (*FlashesTotOphits)[jFlash] < grpflashminophit ) continue;

        // time cut
        if ( abs( (*FlashesTime)[jFlash] - (*FlashesTime)[iFlash] ) > grpflashdt ) continue;

        // distance cut
        double d_ji = std::sqrt( ((*FlashesX)[jFlash] - (*FlashesX)[iFlash])*((*FlashesX)[jFlash] - (*FlashesX)[iFlash]) + ((*FlashesY)[jFlash] - (*FlashesY)[iFlash])*((*FlashesY)[jFlash] - (*FlashesY)[iFlash]) + ((*FlashesZ)[jFlash] - (*FlashesZ)[iFlash])*((*FlashesZ)[jFlash] - (*FlashesZ)[iFlash]) );
        if ( d_ji > grpflashdist ) continue;

        // skip ophits already used in other flash
        if ( FlashesUsedinGroup.at(jFlash) == 1 ) continue;

        // Calculate flash group properties
        iFlashGroupX += (*FlashesX)[jFlash]*(*FlashesTotPE)[jFlash];
        iFlashGroupY += (*FlashesY)[jFlash]*(*FlashesTotPE)[jFlash];
        iFlashGroupZ += (*FlashesZ)[jFlash]*(*FlashesTotPE)[jFlash];
        iFlashGroupTotPE += (*FlashesTotPE)[jFlash];
        iFlashGroupTotOphits += (*FlashesTotOphits)[jFlash];
        iFlashGroupPuritySig += (*FlashesPuritySignal)[jFlash];

        if ((*FlashesTotPE)[jFlash] > GroupFlashmaxPE) { // max PE flash in grp
          GroupFlashmaxPE = (*FlashesTotPE)[jFlash];
          GroupmaxPEFlashindex = jFlash;
        }
        if ((*FlashesTime)[jFlash] <= GroupFlashearliesttime) { // earliest flash
          GroupFlashearliesttime = (*FlashesTime)[jFlash];
          GroupearliestFlashindex = jFlash;
          iFlashGroupEarliestFlashY = (*FlashesY)[jFlash];
          iFlashGroupEarliestFlashZ = (*FlashesZ)[jFlash];
        }

        // Take note flash
        iFlashGroupFlashIndex.push_back(jFlash);

        // Store all ophits PE, Y, Z, T in this flash
        // for later to calculate likelihood with charge cluster
        const std::vector<int>& jFlashophitsindex = FlashesOphitIndex->at(jFlash);
        for (int jophitindex = 0; jophitindex < jFlashophitsindex.size(); jophitindex++){
          iFlashGroupOphitsPE.push_back( (*OpHitsPE)[jFlashophitsindex.at(jophitindex)] );
          iFlashGroupOphitsY.push_back( (*OpHitsY)[jFlashophitsindex.at(jophitindex)] );
          iFlashGroupOphitsZ.push_back( (*OpHitsZ)[jFlashophitsindex.at(jophitindex)] );
          iFlashGroupOphitsT.push_back( (*OpHitsTime)[jFlashophitsindex.at(jophitindex)] );
        }

      } // loop over jflash

      // At this point a group of flash is build, ask for largest PE flash is earliest flash
      // THIS CUT HAS NO EFFECT WITH OR WITHOUT
      //if (GroupmaxPEFlashindex != GroupearliestFlashindex) continue;

      // Set group flash property
      iFlashGroupX = iFlashGroupX/iFlashGroupTotPE;
      iFlashGroupY = iFlashGroupY/iFlashGroupTotPE;
      iFlashGroupZ = iFlashGroupZ/iFlashGroupTotPE;
      iFlashGroupT = GroupFlashearliesttime;
      int iFlashGroupFlashNum = iFlashGroupFlashIndex.size();
      iFlashGroupPuritySig = iFlashGroupPuritySig/iFlashGroupFlashNum;
      double iFlashGroupDist2MarleyTruth = sqrt(pow(iFlashGroupY - marleynueY, 2) + pow(iFlashGroupZ - marleynueZ, 2));

      // At this point, we flag used flashes in this grp so that shouldn't be used again
      for (int iUsedFlash = 0; iUsedFlash < iFlashGroupFlashNum; iUsedFlash++){
        FlashesUsedinGroup.at(iFlashGroupFlashIndex.at(iUsedFlash)) = 1;
      }

      // =======================================
      // **********************************
      //
      // At this point a group of flash is formed
      //
      // **********************************
      // =======================================

      // And store group of flash
      /*FlashGroupTotPE.push_back(iFlashGroupTotPE);
      FlashGroupX.push_back(iFlashGroupX);
      FlashGroupY.push_back(iFlashGroupY);
      FlashGroupZ.push_back(iFlashGroupZ);
      FlashGroupTime.push_back(iFlashGroupT);
      FlashGroupTotOphits.push_back(iFlashGroupTotOphits);
      FlashGroupFlashNum.push_back(iFlashGroupFlashNum);
      FlashGroupSignalPurity.push_back(iFlashGroupPuritySig);*/

      // Evaluate the group flash
      h_grp_flash_totpe_purity->Fill(iFlashGroupTotPE, iFlashGroupPuritySig);
      h_grp_flash_totpe_time->Fill(iFlashGroupTotPE, iFlashGroupT);
      h_grp_flash_totophits_purity->Fill(iFlashGroupTotOphits, iFlashGroupPuritySig);
      h_grp_flash_totophits_time->Fill(iFlashGroupTotOphits, iFlashGroupT);
      h_grp_flash_totflashes_purity->Fill(iFlashGroupFlashNum, iFlashGroupPuritySig);
      h_grp_flash_totflashes_time->Fill(iFlashGroupFlashNum, iFlashGroupT);


      // ====================
      // control plot at event level
      // ===================
      // pick max grp flash with highest purity and check performance -  this is "the best" you will get - is it?
      if (iFlashGroupPuritySig > ctrlPurestFlashGroupPurity) {
        ctrlPurestFlashGroupPurity = iFlashGroupPuritySig;
        ctrlPurestFlashGroupDist = iFlashGroupDist2MarleyTruth;
        ctrlPurestFlashGroupTotPE = iFlashGroupTotPE;
        ctrlPurestFlashGroupFlashNum = iFlashGroupFlashNum;
        ctrlPurestFlashGroupX = iFlashGroupX;
        ctrlPurestFlashGroupY = iFlashGroupY;
        ctrlPurestFlashGroupZ = iFlashGroupZ;
        ctrlPurestFlashGroupT = iFlashGroupT;
        ctrlPurestFlashGroupEarliestFlashY = iFlashGroupEarliestFlashY;
        ctrlPurestFlashGroupEarliestFlashZ = iFlashGroupEarliestFlashZ;

        ctrlPurestFlashGroupOphitsPE = iFlashGroupOphitsPE;
        ctrlPurestFlashGroupOphitsY = iFlashGroupOphitsY;
        ctrlPurestFlashGroupOphitsZ = iFlashGroupOphitsZ;
        ctrlPurestFlashGroupOphitsT = iFlashGroupOphitsT;
      } // control plot: max purity

      // ====================
      // control plot at event level
      // ===================
      // pick grp flash with closest dist to marley and check performance -  this is "the closest" you will get for matching - isn't it?
      if (iFlashGroupDist2MarleyTruth < ctrlClosestFlashGroupDist && iFlashGroupPuritySig > 0) { // put purity requirement to avoid picking up too many bkg flash
        ctrlClosestFlashGroupDist = iFlashGroupDist2MarleyTruth;
        ctrlClosestFlashGroupPurity = iFlashGroupPuritySig;
        ctrlClosestFlashGroupTotPE = iFlashGroupTotPE;
        ctrlClosestFlashGroupFlashNum = iFlashGroupFlashNum;
        ctrlClosestFlashGroupX = iFlashGroupX;
        ctrlClosestFlashGroupY = iFlashGroupY;
        ctrlClosestFlashGroupZ = iFlashGroupZ;
        ctrlClosestFlashGroupT = iFlashGroupT;
        ctrlClosestFlashGroupEarliestFlashY = iFlashGroupEarliestFlashY;
        ctrlClosestFlashGroupEarliestFlashZ = iFlashGroupEarliestFlashZ;

        ctrlClosestFlashGroupOphitsPE = iFlashGroupOphitsPE;
        ctrlClosestFlashGroupOphitsY = iFlashGroupOphitsY;
        ctrlClosestFlashGroupOphitsZ = iFlashGroupOphitsZ;
        ctrlClosestFlashGroupOphitsT = iFlashGroupOphitsT;
      } // control plot: min dist

      /*
      // Which flash to select for signal, maxtotPE, maxtothit, other criteria?
      // use group flash with max ophits
      // select maxtothit grp
      if (iFlashGroupTotOphits >= selFlashGroupTotOphits) {
        if (iFlashGroupTotOphits == selFlashGroupTotOphits) {
          // check totpe
          if (iFlashGroupTotPE>selFlashGroupTotPE || iFlashGroupFlashNum>selFlashGroupFlashNum){
            selFlashGroupTotOphits = iFlashGroupTotOphits;
            selFlashGroupTotPE = iFlashGroupTotPE;
            selFlashGroupFlashNum = iFlashGroupFlashNum;
            selFlashGroupX = iFlashGroupX;
            selFlashGroupY = iFlashGroupY;
            selFlashGroupZ = iFlashGroupZ;
            selFlashGroupT = iFlashGroupT;
          } else {
            ; // do nothing
          }
        } else {
          selFlashGroupTotOphits = iFlashGroupTotOphits;
          selFlashGroupTotPE = iFlashGroupTotPE;
          selFlashGroupFlashNum = iFlashGroupFlashNum;
          selFlashGroupX = iFlashGroupX;
          selFlashGroupY = iFlashGroupY;
          selFlashGroupZ = iFlashGroupZ;
          selFlashGroupT = iFlashGroupT;
        } // if equal hits check totpe and tot flash
      } // select grp with max op hits*/

    } // loop over iflash


    // =====================================
    // Evaluate group flash
    // =====================================
    h_selflashgrp_t_res->Fill(ctrlPurestFlashGroupT*1000 - marleynueTime); //ns
    h_selflashgrp_flashPEweighted_dyz_res->Fill(sqrt(pow(ctrlPurestFlashGroupY - marleynueY, 2) + pow(ctrlPurestFlashGroupZ - marleynueZ, 2)));
    h_selflashgrp_earliestflash_dyz_res->Fill(sqrt(pow(ctrlPurestFlashGroupEarliestFlashY - marleynueY, 2) + pow(ctrlPurestFlashGroupEarliestFlashZ - marleynueZ, 2)));
    //h_sel_flashgrp_t_res->Fill(selFlashGroupT*1000 - marleynueTime); //ns
    //h_sel_flashgrp_dyz_res->Fill(sqrt(pow(selFlashGroupY - marleynueY, 2) + pow(selFlashGroupZ - marleynueZ, 2)));

    // +++++++++++++++++++++++++++++++++++++
    //
    // Light analysis done at per evt level
    //
    // +++++++++++++++++++++++++++++++++++++


    //=======================
    // Event LEVEL
    // Event LEVEL
    // Event LEVEL
    //=======================

    h_marleyX->Fill(marleynueX);
    h_marleyE->Fill(marleynueE);
    // Use highest purity group flash as a start - final selection is supposed to be close to this
    // ROI is ctrlPurestFlashGroupT, ctrlPurestFlashGroupY, ctrlPurestFlashGroupZ, +/- 50cm
    // =====================
    // USE PUREST FLASH GRP
    // =====================
    /*double LightROIcenterY = ctrlPurestFlashGroupY;
    double LightROIcenterZ = ctrlPurestFlashGroupZ;
    double LightROIcenterT = ctrlPurestFlashGroupT;
    std::vector<double> LightROIOphitsPE = ctrlPurestFlashGroupOphitsPE;
    std::vector<double> LightROIOphitsY = ctrlPurestFlashGroupOphitsY;
    std::vector<double> LightROIOphitsZ = ctrlPurestFlashGroupOphitsZ;*/
    // =====================
    // USE CLOSEST FLASH GRP
    // =====================
    double LightROIcenterY = ctrlClosestFlashGroupY;
    double LightROIcenterZ = ctrlClosestFlashGroupZ;
    double LightROIcenterT = ctrlClosestFlashGroupT;
    std::vector<double> LightROIOphitsPE = ctrlClosestFlashGroupOphitsPE;
    std::vector<double> LightROIOphitsY = ctrlClosestFlashGroupOphitsY;
    std::vector<double> LightROIOphitsZ = ctrlClosestFlashGroupOphitsZ;

    //=======================
    // Event LEVEL
    // Event LEVEL
    // Event LEVEL
    //=======================

    // ==============================
    // Similarly extract shower info
    // use highest purity / sel group flash to define ROI
    // calculate likelihood for each hypothesized shower
    // study performance of the SHOWER with max LL (MOST CASES ONLY 1)
    // extract E_reco, Q and E_reco, L
    // ===============================
    double ishower_X_coordinate_t0corr = -999;
    double matchedshowerdt = -999, ishowerdistance2anode = -999.;
    int countROIshowers = 0;
    double maxshowern2ll = -999;
    double ishowern2ll=0;
    double maxn2llshowertpcrecoX_t0corr=-999;
    double maxn2llshowertpcrecoY=-999;
    double maxn2llshowertpcrecoZ=-999;
    double maxn2llshowertpcrecoE=-999;
    double maxn2llshower_bt = -999;

    double maxn2llblippdsrecoY=-999;
    double maxn2llblippdsrecoZ=-999;
    double maxn2llshowerpdsrecoT0=-999;

    double dy_flashgrp_tpcbtshowerstart = -999;
    double dz_flashgrp_tpcbtshowerstart = -999;
    double dyz_flashgrp_tpcbtshowerstart = -999;
    double dy_flashgrp_tpcbtshowercenter = -999;
    double dz_flashgrp_tpcbtshowercenter = -999;
    double dyz_flashgrp_tpcbtshowercenter = -999;

    double recoshowerbtsig = 0; // no shower reconstructed for signal electron

    for (int ishower = 0; ishower < ShowerNum; ishower++){
      ishower_X_coordinate_t0corr = -999;
      matchedshowerdt = -999;
      ishowerdistance2anode = -999.;
      maxshowern2ll = -999;
      ishowern2ll=0;
      maxn2llshowertpcrecoX_t0corr=-999;
      maxn2llshowertpcrecoY=-999;
      maxn2llshowertpcrecoZ=-999;
      maxn2llshowerpdsrecoT0=-999;
      maxn2llshowertpcrecoE=-999;
      maxn2llshower_bt = -999;

      maxn2llblippdsrecoY=-999;
      maxn2llblippdsrecoZ=-999;
      maxn2llshowerpdsrecoT0=-999;

      // *************
      // Control plot
      // *************
      if ( (*Showerbt)[ishower] == -1 ) {
        recoshowerbtsig = 1;
        // what's the distance between highest purity flash grp and the backtrack shower start yz
        dy_flashgrp_tpcbtshowerstart = LightROIcenterY - (*ShowerStartY)[ishower];
        dz_flashgrp_tpcbtshowerstart = LightROIcenterZ - (*ShowerStartZ)[ishower];
        dyz_flashgrp_tpcbtshowerstart = sqrt( pow((*ShowerStartY)[ishower] - LightROIcenterY, 2) + pow((*ShowerStartZ)[ishower] - LightROIcenterZ, 2) );
        h_control_plt_dy_flashgrp_tpcbtshowerstart->Fill(dy_flashgrp_tpcbtshowerstart);
        h_control_plt_dz_flashgrp_tpcbtshowerstart->Fill(dz_flashgrp_tpcbtshowerstart);
        h_control_plt_dyz_flashgrp_tpcbtshowerstart->Fill(dyz_flashgrp_tpcbtshowerstart);

        // what's the distance between highest purity flash grp and the backtrack shower center yz
        // First find the center location of the shower
        // take half the length
        // do not consider X here yet
        double iShowerCenterY = (*ShowerStartY)[ishower] + (*ShowerLength)[ishower]*0.5*(*ShowerDCosStartY)[ishower];
        double iShowerCenterZ = (*ShowerStartZ)[ishower] + (*ShowerLength)[ishower]*0.5*(*ShowerDCosStartZ)[ishower];

        dy_flashgrp_tpcbtshowercenter = LightROIcenterY - iShowerCenterY;
        dz_flashgrp_tpcbtshowercenter = LightROIcenterZ - iShowerCenterZ;
        dyz_flashgrp_tpcbtshowercenter = sqrt( pow(iShowerCenterY - LightROIcenterY, 2) + pow(iShowerCenterZ - LightROIcenterZ, 2) );
        h_control_plt_dy_flashgrp_tpcbtshowercenter->Fill(dy_flashgrp_tpcbtshowercenter);
        h_control_plt_dz_flashgrp_tpcbtshowercenter->Fill(dz_flashgrp_tpcbtshowercenter);
        h_control_plt_dyz_flashgrp_tpcbtshowercenter->Fill(dyz_flashgrp_tpcbtshowercenter);

      }

      // ********************************************
      // matching within light ROI and select shower
      // ********************************************
      // control plot before apply matching ROI
      h_sig_preROImatch_flash2shower_dist->Fill(sqrt( pow((*ShowerStartY)[ishower] - LightROIcenterY, 2) + pow((*ShowerStartZ)[ishower] - LightROIcenterZ, 2) ));

      if ( sqrt( pow((*ShowerStartY)[ishower] - LightROIcenterY, 2) + pow((*ShowerStartZ)[ishower] - LightROIcenterZ, 2) )  > LightROIradius) continue;
      // report number of showers meet the criteria
      countROIshowers++;

      // assume the t0 of this shower is selected FlashGroupT [us]
      // Get driftT from showerX (this is fake driftT as it assumes T0 is readout start)
      double ishowerdriftT = (anodeX - (*ShowerStartX)[ishower]) / (vdmmpus/10) ; // this is alreay in us (not tick!!!)
      matchedshowerdt = ishowerdriftT - LightROIcenterT; // us

      ishowerdistance2anode = matchedshowerdt*vdmmpus/10; // cm
      ishower_X_coordinate_t0corr = anodeX - ishowerdistance2anode;

      // now the shower is located @ ishower_X_coordinate_t0corr, ShowerStartY, and ShowerStartZ, selected FlashGroupT
      // calculate expected amount of light from the shower to each photodetector
      // assume MIP, 25k ph/MeV, 3% PDE, XA size 60x60cm
      // LOOP OVER EACH PHOTODETECTOR
      for (int iflashgrphit = 0; iflashgrphit < LightROIOphitsPE.size(); iflashgrphit++){
        // Calculate solid angle based on square detector in Eq.4-6 in http://zaluzec.com/NJZTools/Zaluzec-FinalAsPublished-SolidAngleFormulaePaper.pdf
        // add non radial correction: tilt angle > 0
        // det X fixed at -320 cm
        // IN THE FUTURE MAY CONSIDER ITERATE OVER SHOWER HITS TO BE MORE ACCURATE
        double d = sqrt( pow((*ShowerStartY)[ishower] - LightROIOphitsY.at(iflashgrphit), 2) + pow((*ShowerStartZ)[ishower] - LightROIOphitsZ.at(iflashgrphit), 2) + pow(ishower_X_coordinate_t0corr + 320, 2) ); // distance from shower to det center
        double D = ishower_X_coordinate_t0corr + 320; // distance to YZ plane;  det X fixed at -320 cm
        double H = sqrt( pow((*ShowerStartY)[ishower] - LightROIOphitsY.at(iflashgrphit), 2) + pow((*ShowerStartZ)[ishower] - LightROIOphitsZ.at(iflashgrphit), 2) ); // distance in YZ plane;
        double cosinetilt = D/sqrt(D*D + H*H);
        double alpha = TMath::ATan(60/(2*d)); // cm
        double beta = TMath::ATan(60*cosinetilt/(2*d)); // cm
        double iSr = 4*TMath::ASin(TMath::Sin(alpha)*TMath::Sin(beta));
        double iexpectediflashgrphit = (*ShowerE)[ishower]*25000*iSr*pde/(4*TMath::Pi());
        // calculate likelihood for this shower to generate the light pattern on this group of flash
        ishowern2ll += iexpectediflashgrphit - LightROIOphitsPE.at(iflashgrphit) + LightROIOphitsPE.at(iflashgrphit) * log(LightROIOphitsPE.at(iflashgrphit) / iexpectediflashgrphit);
      }

      // find max ishowern2ll
      if (ishowern2ll > maxshowern2ll) {
        maxshowern2ll = ishowern2ll;
        maxn2llshower_bt = (*Showerbt)[ishower];
        // max n2ll shower tpc reco
        maxn2llshowertpcrecoX_t0corr = ishower_X_coordinate_t0corr;
        maxn2llshowertpcrecoY = (*ShowerStartY)[ishower];
        maxn2llshowertpcrecoZ = (*ShowerStartZ)[ishower];
        maxn2llshowertpcrecoE = (*ShowerE)[ishower];
        // max n2ll shower pds reco
        maxn2llblippdsrecoY = LightROIcenterY;
        maxn2llblippdsrecoZ = LightROIcenterZ;
        maxn2llshowerpdsrecoT0 = LightROIcenterT;
        // light can have E reco - need VD LY map!!!

      }// max likelihood shower


    }// end shower loop

    // control plot
    h_maxshowern2ll->Fill(maxshowern2ll);

    //=======================
    // Event LEVEL
    // Event LEVEL
    // Event LEVEL
    //=======================

    // *************************************
    // Evaluate light matched shower performance
    // *************************************

    if (ShowerNum > 0) {
      // denominator for matching purity:
      // you got the a shower reconstructed, regardkess of signal/bkg
      h_showerreco_marleyX->Fill(marleynueX);
      h_showerreco_marleyE->Fill(marleynueE);

      if ( recoshowerbtsig == 1 ) {
        // denominator for matching efficiency:
        // you got the a shower reconstructed and can bt to signal
        h_signalshowerreco_marleyX->Fill(marleynueX);
        h_signalshowerreco_marleyE->Fill(marleynueE);

        // break down by energy
        if (marleynueE>5.5 && marleynueE<6.5) {
          h_signalshowerreco_6MeVmarleyX->Fill(marleynueX); // 6 MeV signal
        } else if (marleynueE>9.5 && marleynueE<10.5) {
          h_signalshowerreco_10MeVmarleyX->Fill(marleynueX); // 10 MeV signal
        } else if (marleynueE>17.5 && marleynueE<18.5) {
          h_signalshowerreco_18MeVmarleyX->Fill(marleynueX); // 18 MeV signal
        } else if (marleynueE>24.5 && marleynueE<25.5) {
          h_signalshowerreco_25MeVmarleyX->Fill(marleynueX); // 25 MeV signal
        } else if (marleynueE>49.5 && marleynueE<50.5) {
          h_signalshowerreco_50MeVmarleyX->Fill(marleynueX); // 50 MeV signal
        }
      } // shower reco

      // on top of exist shower, whether you flash match picked the shower bt to true marley sig,
      if (maxn2llshower_bt == -1) {// bt to signal
        //as a func of event drift X
        h_selflashgrp_matched_showerbt2signal_marleyX->Fill(marleynueX);
        h_selflashgrp_matched_showerbt2signal_marleyE->Fill(marleynueE); // as a func of event energy

        // break down by energy
        if (marleynueE>5.5 && marleynueE<6.5) {
          h_selflashgrp_matched_showerbt2signal_6MeVmarleyX->Fill(marleynueX); // 6 MeV signal
        } else if (marleynueE>9.5 && marleynueE<10.5) {
          h_selflashgrp_matched_showerbt2signal_10MeVmarleyX->Fill(marleynueX); // 10 MeV signal
        } else if (marleynueE>17.5 && marleynueE<18.5) {
          h_selflashgrp_matched_showerbt2signal_18MeVmarleyX->Fill(marleynueX); // 18 MeV signal
        } else if (marleynueE>24.5 && marleynueE<25.5) {
          h_selflashgrp_matched_showerbt2signal_25MeVmarleyX->Fill(marleynueX); // 25 MeV signal
        } else if (marleynueE>49.5 && marleynueE<50.5) {
          h_selflashgrp_matched_showerbt2signal_50MeVmarleyX->Fill(marleynueX); // 50 MeV signal
        }

        // fill performance
        h_selflashgrp_matched_showerbt2signal_pdsrecoT0->Fill(maxn2llshowerpdsrecoT0);
        h_selflashgrp_matched_showerbt2signal_tpcrecoX_t0corr->Fill(maxn2llshowertpcrecoX_t0corr);
        // tpc reco
        h_selflashgrp_matched_showerbt2signal_tpcrecoY->Fill(maxn2llshowertpcrecoY);
        h_selflashgrp_matched_showerbt2signal_tpcrecoZ->Fill(maxn2llshowertpcrecoZ);
        h_selflashgrp_matched_showerbt2signal_tpcrecoE->Fill(maxn2llshowertpcrecoE);
        // pds reco
        h_selflashgrp_matched_showerbt2signal_pdsrecoY->Fill(maxn2llblippdsrecoY);
        h_selflashgrp_matched_showerbt2signal_pdsrecoZ->Fill(maxn2llblippdsrecoZ);
        // evt level info
        h_selflashgrp_selshower_evt_X_tpc_res->Fill(maxn2llshowertpcrecoX_t0corr-marleynueX);
        h_selflashgrp_selshower_evt_Y_tpc_res->Fill(maxn2llshowertpcrecoY-marleynueY);
        h_selflashgrp_selshower_evt_Z_tpc_res->Fill(maxn2llshowertpcrecoZ-marleynueZ);

        h_selflashgrp_selshower_evt_T_pds_res->Fill(maxn2llshowerpdsrecoT0-marleynueTime);
        h_selflashgrp_selshower_evt_Y_pds_res->Fill(maxn2llblippdsrecoY-marleynueY);
        h_selflashgrp_selshower_evt_Z_pds_res->Fill(maxn2llblippdsrecoZ-marleynueZ);
      } else if (maxn2llshower_bt > -1) {//bkg
        //as a func of event drift X
        h_selflashgrp_matched_showerbt2bkg_marleyX->Fill(marleynueX);
        h_selflashgrp_matched_showerbt2bkg_marleyE->Fill(marleynueE); // as a func of event energy
      }

    } // shower reco exist


    //=======================
    // Event LEVEL
    // Event LEVEL
    // Event LEVEL
    //=======================


    // ==============================
    // Extract blipreco info
    // use true / sel group flash to define ROI
    // calculate likelihood for each hypothesized blip
    // study performance of the blip with max LL
    // extract E_reco, Q and E_reco, L
    // ===============================
    // Take blips collection
    double blip_X_coordinate_t0corr = -999;
    double dt = -999, iblipdistance2anode = -999;
    int countROIblips = 0;
    double maxblipn2ll = -999;
    double blipn2ll=0;
    double maxn2llbliptpcrecoX_t0corr=-999;
    double maxn2llbliptpcrecoY=-999;
    double maxn2llbliptpcrecoZ=-999;
    double maxn2llblippdsrecoT0=-999;
    double maxn2llbliptpcrecoE=-999;

    double maxn2llblip_trueX=-999;
    double maxn2llblip_trueY=-999;
    double maxn2llblip_trueZ=-999;
    double maxn2llblip_trueT=-999;
    double maxn2llblip_trueE=-999;
    int maxn2llblip_bt = -999;

    for (int iblip = 0; iblip < nblips; iblip++){
      blip_X_coordinate_t0corr = -999;
      dt = -999;
      iblipdistance2anode = -999;
      maxblipn2ll = -999;
      blipn2ll=0;
      maxn2llbliptpcrecoX_t0corr=-999;
      maxn2llbliptpcrecoY=-999;
      maxn2llbliptpcrecoZ=-999;
      maxn2llblippdsrecoT0=-999;
      maxn2llbliptpcrecoE=-999;

      maxn2llblip_trueX=-999;
      maxn2llblip_trueY=-999;
      maxn2llblip_trueZ=-999;
      maxn2llblip_trueT=-999;
      maxn2llblip_trueE=-999;
      maxn2llblip_bt = -999;

      if ( sqrt( pow((*blipY)[iblip] - LightROIcenterY, 2) + pow((*blipZ)[iblip] - LightROIcenterZ, 2) )  > LightROIradius) continue; // skip stuff outside ROI
      // report number of blips meet the criteria
      countROIblips++;

      // assume the t0 of this blip is FlashGroupT [us]
      // Drift T in ticks (1 tick = 0.5 us), blip true T in us
      dt = (*blipDriftT)[iblip]*0.5 - LightROIcenterT; // us

      iblipdistance2anode = dt*vdmmpus/10; // cm
      blip_X_coordinate_t0corr = anodeX - iblipdistance2anode;

      // now the blip is located @ blip_X_coordinate_t0corr, blipY, and blipZ, FlashGroupT
      // calculate expected amount of light from the blip to each detector
      // assume MIP, 25k ph/MeV, 3% PDE, XA size 60x60cm
      for (int iflashgrphit = 0; iflashgrphit < LightROIOphitsPE.size(); iflashgrphit++){
        // Calculate solid angle based on square detector in Eq.4-6 in http://zaluzec.com/NJZTools/Zaluzec-FinalAsPublished-SolidAngleFormulaePaper.pdf
        // add non radial correction: tilt angle > 0
        // det X fixed at -320 cm
        double d = sqrt( pow((*blipY)[iblip] - LightROIOphitsY.at(iflashgrphit), 2) + pow((*blipZ)[iblip] - LightROIOphitsZ.at(iflashgrphit), 2) + pow(blip_X_coordinate_t0corr + 320, 2) ); // distance from blip to det center
        double D = blip_X_coordinate_t0corr + 320; // distance to YZ plane;  det X fixed at -320 cm
        double H = sqrt( pow((*blipY)[iblip] - LightROIOphitsY.at(iflashgrphit), 2) + pow((*blipZ)[iblip] - LightROIOphitsZ.at(iflashgrphit), 2) ); // distance in YZ plane;
        double cosinetilt = D/sqrt(D*D + H*H);
        double alpha = TMath::ATan(60/(2*d)); // cm
        double beta = TMath::ATan(60*cosinetilt/(2*d)); // cm
        double iSr = 4*TMath::ASin(TMath::Sin(alpha)*TMath::Sin(beta));
        double iexpectediflashgrphit = (*blipE)[iblip]*25000*iSr*pde/(4*TMath::Pi());
        // calculate likelihood for this blip to generate the light pattern on this group of flash
        blipn2ll += iexpectediflashgrphit - LightROIOphitsPE.at(iflashgrphit) + LightROIOphitsPE.at(iflashgrphit) * log(LightROIOphitsPE.at(iflashgrphit) / iexpectediflashgrphit);
      }

      // add track collection - pandora? ask Will, why there is blipX, what track collection it takes

      // find max blipn2ll
      if (blipn2ll > maxblipn2ll) {
        maxblipn2ll = blipn2ll;
        // control plot
        //if (ientry == 0) {
          //h_maxblipn2ll_evol->Fill(countROIblips, maxblipn2ll);
          //cout<< "@ evt " << ientry << ", countROIblips: "<< countROIblips << ", maxblipn2ll: " <<maxblipn2ll  <<endl;
        //}

        // selected max n2ll blip info
        maxn2llblip_bt    = (*blipbt)[iblip];
        // max n2ll blip tpc reco
        maxn2llbliptpcrecoX_t0corr = blip_X_coordinate_t0corr;
        maxn2llbliptpcrecoY = (*blipY)[iblip];
        maxn2llbliptpcrecoZ = (*blipZ)[iblip];
        maxn2llbliptpcrecoE = (*blipE)[iblip];
        // max n2ll blip pds reco
        maxn2llblippdsrecoY = LightROIcenterY;
        maxn2llblippdsrecoZ = LightROIcenterZ;
        maxn2llblippdsrecoT0 = LightROIcenterT;

        maxn2llblip_trueX = (*blipX_true)[iblip];
        maxn2llblip_trueY = (*blipY_true)[iblip];
        maxn2llblip_trueZ = (*blipZ_true)[iblip];
        maxn2llblip_trueT = (*blipT_true)[iblip];
        maxn2llblip_trueE = (*blipE_true)[iblip];

      }// max likelihood blip


    }// end iblip loop

    //=======================
    // Event LEVEL
    // Event LEVEL
    // Event LEVEL
    //=======================

    maxblipn2ll = 2*maxblipn2ll; // -2 log likelihood for blips part
    //cout<< "@ evt " << ientry << ", blips within ROI 50cm radius: " <<countROIblips  <<endl;
    // *************************************
    // Evaluate light matched blip performance
    // *************************************
    // blip level performance
    h_selflashgrp_selblip_X_res->Fill(maxn2llbliptpcrecoX_t0corr-maxn2llblip_trueX);// X location compare: most important
    h_selflashgrp_selblip_Y_res->Fill(maxn2llbliptpcrecoY-maxn2llblip_trueY);
    h_selflashgrp_selblip_Z_res->Fill(maxn2llbliptpcrecoZ-maxn2llblip_trueZ);
    h_selflashgrp_selblip_T_res->Fill(maxn2llblippdsrecoT0-maxn2llblip_trueT);
    h_selflashgrp_selblip_E_res->Fill(maxn2llbliptpcrecoE-maxn2llblip_trueE);
    h_selflashgrp_maxn2llblip_bt->Fill(maxn2llblip_bt);

    // Event level blip performance
    h_selflashgrp_selblip_evt_X_res->Fill(maxn2llbliptpcrecoX_t0corr-marleynueX);
    h_selflashgrp_selblip_evt_Y_res->Fill(maxn2llbliptpcrecoY-marleynueY);
    h_selflashgrp_selblip_evt_Z_res->Fill(maxn2llbliptpcrecoZ-marleynueZ);
    h_selflashgrp_selblip_evt_T_res->Fill(maxn2llblippdsrecoT0-marleynueTime);


    // whether you got the blip bt to true sign, as a func of event drift X
    if (maxn2llblip_bt == -1) {// found blip bt to signal
      h_selflashgrp_matched_blipbt2signal_marleyX->Fill(marleynueX);
    }

    // as a func of event energy
    if (maxn2llblip_bt == -1) {// found blip bt to signal
      h_selflashgrp_matched_blipbt2signal_marleyE->Fill(marleynueE);
    }







    // reco energy =  shower + blips in shower ROI 50cm

  }// end ientry

  // Create output files
  TFile * outFile = new TFile("QL_performance_plots.root", "RECREATE");

  outFile->mkdir("MC_truth");
  outFile->cd("MC_truth");
  h_marleyX->Write();
  h_marleyE->Write();

  outFile->mkdir("preFlashgrp_controlplots");
  outFile->cd("preFlashgrp_controlplots");

  // control plots for grping flash
  h_sig_flash_timespan->Write();
  h_sig_flash_y_dist->Write();
  h_sig_flash_z_dist->Write();
  h_sig_flash_max_yz_dist->Write();
  h_sig_flash_max_yz_above5m_dist_ophits->Write();
  h_sig_flash_max_yz_below5m_dist_ophits->Write();
  h_sig_maxdist_above5m_flash_2ophits_samech->Write();
  h_sig_maxdist_below5m_flash_2ophits_samech->Write();
  h_sig_flash_ophits->Write();
  h_sig_earliest_is_largest->Write();
  //h_sig_flash_time_vs_pe->Write();

  outFile->mkdir("Flashgrp_vars");
  outFile->cd("Flashgrp_vars");
  // normalize var hists of signal and bkg
  auto flashvarcomp = new TCanvas("flashvarcomp", "flashvarcomp", 700, 500);
  flashvarcomp->cd();
  flashvarcomp->SetLogz();
  h_grp_flash_totpe_purity->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totpe_purity");
  h_grp_flash_totophits_purity->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totophits_purity"); // most useful var
  h_grp_flash_totflashes_purity->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totflashes_purity");
  flashvarcomp->SetLogy();
  h_grp_flash_totpe_time->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totpe_time");
  h_grp_flash_totophits_time->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totophits_time");
  h_grp_flash_totflashes_time->Draw("COLZ");
  flashvarcomp->Write("grp_flash_2D_totflashes_time");

  outFile->mkdir("Maxpurity_Flashgrp");
  outFile->cd("Maxpurity_Flashgrp");
  h_selflashgrp_t_res->Write();
  h_selflashgrp_flashPEweighted_dyz_res->Write();
  h_selflashgrp_earliestflash_dyz_res->Write();


  // Shower
  // Shower
  // Shower
  outFile->mkdir("shower_reco_eval");
  outFile->cd("shower_reco_eval");
  // shower reco performance
  h_showerreco_marleyX->Write();
  h_showerreco_marleyE->Write();

  h_signalshowerreco_marleyX->Write();
  h_signalshowerreco_6MeVmarleyX->Write();
  h_signalshowerreco_10MeVmarleyX->Write();
  h_signalshowerreco_18MeVmarleyX->Write();
  h_signalshowerreco_25MeVmarleyX->Write();
  h_signalshowerreco_50MeVmarleyX->Write();

  h_signalshowerreco_marleyE->Write();
  // signal shower reco eff
  TH1F *h_signalshowerreco_eff_marleyX = (TH1F*)h_signalshowerreco_marleyX->Clone("h_signalshowerreco_eff_marleyX");
  h_signalshowerreco_eff_marleyX->Divide(h_marleyX);
  h_signalshowerreco_eff_marleyX->Write();
  TH1F *h_signalshowerreco_eff_marleyE = (TH1F*)h_signalshowerreco_marleyE->Clone("h_signalshowerreco_eff_marleyE");
  h_signalshowerreco_eff_marleyE->Divide(h_marleyE);
  h_signalshowerreco_eff_marleyE->Write();


  outFile->mkdir("Pre_flash_shower_matching_eval");
  outFile->cd("Pre_flash_shower_matching_eval");
  // control plots
  h_control_plt_dy_flashgrp_tpcbtshowerstart->Write();
  h_control_plt_dz_flashgrp_tpcbtshowerstart->Write();
  h_control_plt_dyz_flashgrp_tpcbtshowerstart->Write();
  h_control_plt_dy_flashgrp_tpcbtshowercenter->Write();
  h_control_plt_dz_flashgrp_tpcbtshowercenter->Write();
  h_control_plt_dyz_flashgrp_tpcbtshowercenter->Write();
  // pre matching signal shower performance
  h_sig_preROImatch_flash2shower_dist->Write();

  outFile->mkdir("Flash_shower_matching_eval");
  outFile->cd("Flash_shower_matching_eval");
  h_maxshowern2ll->Write();
  h_selflashgrp_matched_showerbt2signal_marleyX->Write();
  h_selflashgrp_matched_showerbt2signal_6MeVmarleyX->Write();
  h_selflashgrp_matched_showerbt2signal_10MeVmarleyX->Write();
  h_selflashgrp_matched_showerbt2signal_18MeVmarleyX->Write();
  h_selflashgrp_matched_showerbt2signal_25MeVmarleyX->Write();
  h_selflashgrp_matched_showerbt2signal_50MeVmarleyX->Write();

  h_selflashgrp_matched_showerbt2signal_marleyE->Write();


  TH1F *h_selflashgrp_showerbt2signal_marleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_marleyX->Clone("h_selflashgrp_showerbt2signal_marleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_marleyX_MatchingEff->Divide(h_signalshowerreco_marleyX);
  h_selflashgrp_showerbt2signal_marleyX_MatchingEff->Write();
  TH1F *h_selflashgrp_showerbt2signal_6MeVmarleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_6MeVmarleyX->Clone("h_selflashgrp_showerbt2signal_6MeVmarleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_6MeVmarleyX_MatchingEff->Divide(h_signalshowerreco_6MeVmarleyX);
  h_selflashgrp_showerbt2signal_6MeVmarleyX_MatchingEff->Write();
  TH1F *h_selflashgrp_showerbt2signal_10MeVmarleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_10MeVmarleyX->Clone("h_selflashgrp_showerbt2signal_10MeVmarleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_10MeVmarleyX_MatchingEff->Divide(h_signalshowerreco_10MeVmarleyX);
  h_selflashgrp_showerbt2signal_10MeVmarleyX_MatchingEff->Write();
  TH1F *h_selflashgrp_showerbt2signal_18MeVmarleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_18MeVmarleyX->Clone("h_selflashgrp_showerbt2signal_18MeVmarleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_18MeVmarleyX_MatchingEff->Divide(h_signalshowerreco_18MeVmarleyX);
  h_selflashgrp_showerbt2signal_18MeVmarleyX_MatchingEff->Write();
  TH1F *h_selflashgrp_showerbt2signal_25MeVmarleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_25MeVmarleyX->Clone("h_selflashgrp_showerbt2signal_25MeVmarleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_25MeVmarleyX_MatchingEff->Divide(h_signalshowerreco_25MeVmarleyX);
  h_selflashgrp_showerbt2signal_25MeVmarleyX_MatchingEff->Write();
  TH1F *h_selflashgrp_showerbt2signal_50MeVmarleyX_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_50MeVmarleyX->Clone("h_selflashgrp_showerbt2signal_50MeVmarleyX_MatchingEff");
  h_selflashgrp_showerbt2signal_50MeVmarleyX_MatchingEff->Divide(h_signalshowerreco_50MeVmarleyX);
  h_selflashgrp_showerbt2signal_50MeVmarleyX_MatchingEff->Write();

  h_selflashgrp_matched_showerbt2bkg_marleyX->Write();
  h_selflashgrp_matched_showerbt2bkg_marleyE->Write();

  // total matched: X
  TH1F *h_selflashgrp_matched_shower_tot_marleyX = (TH1F*)h_selflashgrp_matched_showerbt2bkg_marleyX->Clone("h_selflashgrp_matched_shower_tot_marleyX");
  h_selflashgrp_matched_shower_tot_marleyX->Add(h_selflashgrp_matched_showerbt2signal_marleyX);
  h_selflashgrp_matched_shower_tot_marleyX->Write();
  TH1F *h_selflashgrp_showerbt2signal_marleyX_MatchingPurity = (TH1F*)h_selflashgrp_matched_showerbt2signal_marleyX->Clone("h_selflashgrp_showerbt2signal_marleyX_MatchingPurity");
  h_selflashgrp_showerbt2signal_marleyX_MatchingPurity->Divide(h_selflashgrp_matched_shower_tot_marleyX);
  h_selflashgrp_showerbt2signal_marleyX_MatchingPurity->Write();

  // total matched: E
  TH1F *h_selflashgrp_showerbt2signal_marleyE_MatchingEff = (TH1F*)h_selflashgrp_matched_showerbt2signal_marleyE->Clone("h_selflashgrp_showerbt2signal_marleyE_MatchingEff");
  h_selflashgrp_showerbt2signal_marleyE_MatchingEff->Divide(h_signalshowerreco_marleyE);
  h_selflashgrp_showerbt2signal_marleyE_MatchingEff->Write();

  TH1F *h_selflashgrp_matched_shower_tot_marleyE = (TH1F*)h_selflashgrp_matched_showerbt2bkg_marleyE->Clone("h_selflashgrp_matched_shower_tot_marleyE");
  h_selflashgrp_matched_shower_tot_marleyE->Add(h_selflashgrp_matched_showerbt2signal_marleyE);
  h_selflashgrp_matched_shower_tot_marleyE->Write();
  TH1F *h_selflashgrp_showerbt2signal_marleyE_MatchingPurity = (TH1F*)h_selflashgrp_matched_showerbt2signal_marleyE->Clone("h_selflashgrp_showerbt2signal_marleyE_MatchingPurity");
  h_selflashgrp_showerbt2signal_marleyE_MatchingPurity->Divide(h_selflashgrp_matched_shower_tot_marleyE);
  h_selflashgrp_showerbt2signal_marleyE_MatchingPurity->Write();

  outFile->mkdir("Post_matching_showereval");
  outFile->cd("Post_matching_showereval");

  // matching time
  h_selflashgrp_matched_showerbt2signal_pdsrecoT0->Write();
  h_selflashgrp_matched_showerbt2signal_tpcrecoX_t0corr->Write();
  // tpc reco quantities
  h_selflashgrp_matched_showerbt2signal_tpcrecoY->Write();
  h_selflashgrp_matched_showerbt2signal_tpcrecoZ->Write();
  h_selflashgrp_matched_showerbt2signal_tpcrecoE->Write();
  // light reco quantities
  h_selflashgrp_matched_showerbt2signal_pdsrecoY->Write();
  h_selflashgrp_matched_showerbt2signal_pdsrecoZ->Write();
  // resolution tpc
  h_selflashgrp_selshower_evt_X_tpc_res->Write();
  h_selflashgrp_selshower_evt_Y_tpc_res->Write();
  h_selflashgrp_selshower_evt_Z_tpc_res->Write();
  // resolution pds
  h_selflashgrp_selshower_evt_T_pds_res->Write();
  h_selflashgrp_selshower_evt_Y_pds_res->Write();
  h_selflashgrp_selshower_evt_Z_pds_res->Write();



  // Blip
  // Blip
  // Blip
  outFile->mkdir("Flash_blip_matching_eval");
  outFile->cd("Flash_blip_matching_eval");
  h_selflashgrp_matched_blipbt2signal_marleyX->Write();
  h_selflashgrp_matched_blipbt2signal_marleyE->Write();
  // PROBLEM HERE - Denominator should be events with signal blips recoed, not all events???
  TH1F *h_selflashgrp_matched_blipbt2signal_marleyX_eff = (TH1F*)h_selflashgrp_matched_blipbt2signal_marleyX->Clone("h_selflashgrp_matched_blipbt2signal_marleyX_eff");
  h_selflashgrp_matched_blipbt2signal_marleyX_eff->Divide(h_marleyX);
  h_selflashgrp_matched_blipbt2signal_marleyX_eff->Write();
  TH1F *h_selflashgrp_matched_blipbt2signal_marleyE_eff = (TH1F*)h_selflashgrp_matched_blipbt2signal_marleyE->Clone("h_selflashgrp_matched_blipbt2signal_marleyE_eff");
  h_selflashgrp_matched_blipbt2signal_marleyE_eff->Divide(h_marleyE);
  h_selflashgrp_matched_blipbt2signal_marleyE_eff->Write();


  outFile->mkdir("Post_matching_blipeval");
  outFile->cd("Post_matching_blipeval");
  // blip level performance
  //h_maxblipn2ll_evol->Write();
  h_selflashgrp_selblip_X_res->Write();
  h_selflashgrp_selblip_Y_res->Write();
  h_selflashgrp_selblip_Z_res->Write();
  h_selflashgrp_selblip_T_res->Write();
  h_selflashgrp_selblip_E_res->Write();
  h_selflashgrp_maxn2llblip_bt->Write();

  // check performance at event level Evt location compare
  h_selflashgrp_selblip_evt_X_res->Write();
  h_selflashgrp_selblip_evt_Y_res->Write();
  h_selflashgrp_selblip_evt_Z_res->Write();
  h_selflashgrp_selblip_evt_T_res->Write();

  outFile->Close();

}
