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
  TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalandBkg/cluster100ns3m_min1hits_1PE_nomemb_100kevts/Marleyandbkg_fullstream_opcluster100ns3m_min1hits_1PE_nomemb_newbtmaxEfrac_2026-1-14_QLMatch_reco22ana_001.root");
  // signal only
  //TFile *file0 = TFile::Open("/pnfs/dune/persistent/users/weishi/VDMarleyCCBkgAna/SignalOnly/100ns3mcluster_min1hits_minophit1PE/MarleyOnly_fullstream_opcluster100ns3m_min1hit_1PE_nomemb_newbtmaxEfrac_2026-1-14_QLMatch_reco22ana.root");

  gInterpreter->GenerateDictionary("vector<vector<int>>", "vector");

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

  std::vector<double> FlashesUsedinGroup;
  double grpflashdt = 2; // us
  double grpflashdist = 5; // m

  for ( int ientry = 0; ientry < 3000; ientry++ )
  {
    AnaTree->GetEntry(ientry);

    if (ientry % 100 ==0) cout<< "@ evt " << ientry<<endl;

    // =====================================
    // Study bkg
    // =====================================
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
}
