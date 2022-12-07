//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Aug 30 11:51:20 2022 by ROOT version 6.22/08
// from TChain vdflashmatch/FlashMatchTree/
//////////////////////////////////////////////////////////

#ifndef Mergednobkgnd50kEtrue_h
#define Mergednobkgnd50kEtrue_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class Mergednobkgnd50kEtrue {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           EventID;
   Float_t         TrueX;
   Float_t         TrueY;
   Float_t         TrueZ;
   Float_t         TrueT;
   Float_t         DetectedT;
   Float_t         TrueE;
   Float_t         Edep;
   Float_t         EdepSimE;
   Int_t           TruePDG;
   Int_t           NFlashes;
   vector<int>     *FlashIDVector;
   vector<float>   *YCenterVector;
   vector<float>   *ZCenterVector;
   vector<float>   *YWidthVector;
   vector<float>   *ZWidthVector;
   vector<float>   *TimeVector;
   vector<float>   *TimeWidthVector;
   vector<float>   *TimeDiffVector;
   vector<float>   *TotalPEVector;
   Float_t         SumPE;
   Int_t           NOpDets;
   vector<int>     *NHitOpDetVector;
   vector<float>   *Purity;
   vector<float>   *Distance;
   vector<float>   *RecoXVector;

   // List of branches
   TBranch        *b_EventID;   //!
   TBranch        *b_TrueX;   //!
   TBranch        *b_TrueY;   //!
   TBranch        *b_TrueZ;   //!
   TBranch        *b_TrueT;   //!
   TBranch        *b_DetectedT;   //!
   TBranch        *b_TrueE;   //!
   TBranch        *b_Edep;   //!
   TBranch        *b_EdepSimE;   //!
   TBranch        *b_TruePDG;   //!
   TBranch        *b_NFlashes;   //!
   TBranch        *b_FlashIDVector;   //!
   TBranch        *b_YCenterVector;   //!
   TBranch        *b_ZCenterVector;   //!
   TBranch        *b_YWidthVector;   //!
   TBranch        *b_ZWidthVector;   //!
   TBranch        *b_TimeVector;   //!
   TBranch        *b_TimeWidthVector;   //!
   TBranch        *b_TimeDiffVector;   //!
   TBranch        *b_TotalPEVector;   //!
   TBranch        *b_SumPE;   //!
   TBranch        *b_NOpDets;   //!
   TBranch        *b_NHitOpDetVector;   //!
   TBranch        *b_Purity;   //!
   TBranch        *b_Distance;   //!
   TBranch        *b_RecoXVector;   //!

   Mergednobkgnd50kEtrue(TTree *tree=0);
   virtual ~Mergednobkgnd50kEtrue();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Mergednobkgnd50kEtrue_cxx
Mergednobkgnd50kEtrue::Mergednobkgnd50kEtrue(TTree *tree) : fChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("vdflashmatch/FlashMatchTree",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("vdflashmatch/FlashMatchTree", "");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_6/ana_hist_9f856b7d-3216-4293-8916-984254da9460.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_0/ana_hist_382d717d-d4bd-4d0f-ae9a-947bbfa2aad6.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_7/ana_hist_5094aedb-0dbc-4868-92c1-3c3b73fd7c0c.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_4/ana_hist_75bb4ab9-1414-485a-b387-5f0fb0f0ab8a.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_3/ana_hist_5d957b7d-c42a-43c7-9f01-b3668421fb8a.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_1/ana_hist_0b22ac24-8778-412a-ab2e-159b7b5711d3.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_8/ana_hist_79aca95b-4aa0-4122-8ff8-498e460b4f9b.root");
      chain->Add("/pnfs/dune/persistent/users/weishi/vdpds/v09_58_01d00/prodmarley_nue_mono10_nobkg_darkcount10Hz/ana/41291455_10/ana_hist_6c207ed6-46a0-4093-8889-226f2c22d91c.root");

      std::cout<< "Adding trees to chain" <<std::endl;

      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Mergednobkgnd50kEtrue::~Mergednobkgnd50kEtrue()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Mergednobkgnd50kEtrue::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Mergednobkgnd50kEtrue::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Mergednobkgnd50kEtrue::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   FlashIDVector = 0;
   YCenterVector = 0;
   ZCenterVector = 0;
   YWidthVector = 0;
   ZWidthVector = 0;
   TimeVector = 0;
   TimeWidthVector = 0;
   TimeDiffVector = 0;
   TotalPEVector = 0;
   NHitOpDetVector = 0;
   Purity = 0;
   Distance = 0;
   RecoXVector = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EventID", &EventID, &b_EventID);
   fChain->SetBranchAddress("TrueX", &TrueX, &b_TrueX);
   fChain->SetBranchAddress("TrueY", &TrueY, &b_TrueY);
   fChain->SetBranchAddress("TrueZ", &TrueZ, &b_TrueZ);
   fChain->SetBranchAddress("TrueT", &TrueT, &b_TrueT);
   fChain->SetBranchAddress("DetectedT", &DetectedT, &b_DetectedT);
   fChain->SetBranchAddress("TrueE", &TrueE, &b_TrueE);
   fChain->SetBranchAddress("Edep", &Edep, &b_Edep);
   fChain->SetBranchAddress("EdepSimE", &EdepSimE, &b_EdepSimE);
   fChain->SetBranchAddress("TruePDG", &TruePDG, &b_TruePDG);
   fChain->SetBranchAddress("NFlashes", &NFlashes, &b_NFlashes);
   fChain->SetBranchAddress("FlashIDVector", &FlashIDVector, &b_FlashIDVector);
   fChain->SetBranchAddress("YCenterVector", &YCenterVector, &b_YCenterVector);
   fChain->SetBranchAddress("ZCenterVector", &ZCenterVector, &b_ZCenterVector);
   fChain->SetBranchAddress("YWidthVector", &YWidthVector, &b_YWidthVector);
   fChain->SetBranchAddress("ZWidthVector", &ZWidthVector, &b_ZWidthVector);
   fChain->SetBranchAddress("TimeVector", &TimeVector, &b_TimeVector);
   fChain->SetBranchAddress("TimeWidthVector", &TimeWidthVector, &b_TimeWidthVector);
   fChain->SetBranchAddress("TimeDiffVector", &TimeDiffVector, &b_TimeDiffVector);
   fChain->SetBranchAddress("TotalPEVector", &TotalPEVector, &b_TotalPEVector);
   fChain->SetBranchAddress("SumPE", &SumPE, &b_SumPE);
   fChain->SetBranchAddress("NOpDets", &NOpDets, &b_NOpDets);
   fChain->SetBranchAddress("NHitOpDetVector", &NHitOpDetVector, &b_NHitOpDetVector);
   fChain->SetBranchAddress("Purity", &Purity, &b_Purity);
   fChain->SetBranchAddress("Distance", &Distance, &b_Distance);
   fChain->SetBranchAddress("RecoXVector", &RecoXVector, &b_RecoXVector);
   Notify();
}

Bool_t Mergednobkgnd50kEtrue::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Mergednobkgnd50kEtrue::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Mergednobkgnd50kEtrue::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Mergednobkgnd50kEtrue_cxx
