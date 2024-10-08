//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jun  9 17:58:10 2024 by ROOT version 6.30/06
// from TChain HitsTree/
//////////////////////////////////////////////////////////

#ifndef vbox_h
#define vbox_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
const Int_t MAXHITS = 400000;
const Int_t MAXINEHITS = 400000;
  const Int_t MXS = 500000;
// Header file for the classes stored in the TTree if any.

class vbox {
public :

   TTree          *fheader;   //!pointer to the analyzed TTree or TChain
   TTree          *fhits;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           RunNum;
   Int_t           EveNum;
   Int_t           NIncHits;
   Int_t           IdInc[MAXHITS];   //[NIncHits]
   Int_t           IdParInc[MAXHITS];   //[NIncHits]
   Int_t           TrInc[MAXHITS];   //[NIncHits]
   Int_t           ParTrInc[MAXHITS];   //[NIncHits]
   Int_t           IntParInc[MAXHITS];   //[NIncHits]
   Int_t           RegInc[MAXHITS];   //[NIncHits]
   Float_t         PosInc[MAXHITS][3];   //[NIncHits]
   Float_t         PInc[MAXHITS][5];   //[NIncHits]
   Float_t         TimeInc[MAXHITS];   //[NIncHits]
   Int_t           NCalHits;
   Int_t           WhCal[MAXHITS];   //[NCalHits]
   Float_t         PosCal[MAXHITS][3];   //[NCalHits]
   Float_t         EneCal[MAXHITS];   //[NCalHits]
   Int_t           NCHitNCap;
   Int_t           WhCNCap[MAXHITS];   //[NCHitNCap]
   Float_t         PosCNCap[MAXHITS][3];   //[NCHitNCap]
   Float_t         EneCNCap[MAXHITS];   //[NCHitNCap]
   Int_t           NPheHits;
   Float_t         Phe[MAXHITS];   //[NPheHits]
   Int_t           PheTile[MAXHITS];   //[NPheHits]
   Float_t         PheTime[MAXHITS];   //[NPheHits]
   Float_t         TotPhe[4];
   Int_t           NPheHitsNCap;
   Float_t         PheNCap[MAXHITS];   //[NPheHitsNCap]
   Int_t           PheTileNCap[MAXHITS];   //[NPheHitsNCap]
   Float_t         PheTimeNCap[MAXHITS];   //[NPheHitsNCap]
   Float_t         TotPheNCap[4];
   Int_t           NIneHits;
   Int_t           TypeIne[MAXINEHITS];   //[NIneHits]
   Int_t           IdIne[MAXINEHITS];   //[NIneHits]
   Int_t           TrIne[MAXINEHITS];   //[NIneHits]
   Int_t           IdParIne[MAXINEHITS];   //[NIneHits]
   Int_t           IntParIne[MAXHITS];   //[NIncHits]
   Float_t         PIne[MAXINEHITS][5];   //[NIneHits]
   Float_t         PosIne[MAXINEHITS][3];   //[NIneHits]
   Float_t         TimeIne[MAXINEHITS];   //[NIneHits]
   Int_t           RegIne[MAXINEHITS];   
  //   Int_t           NeuOldIne[MAXINEHITS];   
  //   Int_t           NeuRegIne[MAXINEHITS];   
   Int_t           NSecIne[MAXINEHITS];   //[NIneHits]
   Int_t           FirstSec[MAXINEHITS];   //[NIneHits]
   Int_t           NTIneSec;
   Int_t           HitSecIne[MXS];   //[NTIneSec]
   Int_t           IdSecIne[MXS];   //[NTIneSec]
   Int_t           ASecIne[MXS];   //[NTIneSec]
   Int_t           ZSecIne[MXS];   //[NTIneSec]
   Int_t           TrSecIne[MXS];   //[NTIneSec]
   Int_t           NeuOldSec[MXS];   //[NTIneSec]
   Int_t           NeuRegSec[MXS];   //[NTIneSec]
   Int_t           NeuOldVtx[MXS];   //[NTIneSec]
   Float_t         PSec[MXS][5];   //[NTIneSec]

   // List of branches
   TBranch        *b_RunNum;   //!
   TBranch        *b_EveNum;   //!
   TBranch        *b_NIncHits;   //!
   TBranch        *b_IdInc;   //!
   TBranch        *b_IdParInc;   //!
   TBranch        *b_TrInc;   //!
   TBranch        *b_ParTrInc;   //!
   TBranch        *b_IntParInc;   //!
   TBranch        *b_RegInc;   //!
   TBranch        *b_PosInc;   //!
   TBranch        *b_PInc;   //!
   TBranch        *b_TimeInc;   //!
   TBranch        *b_NCalHits;   //!
   TBranch        *b_WhCal;   //!
   TBranch        *b_PosCal;   //!
   TBranch        *b_EneCal;   //!
   TBranch        *b_NCHitNCap;   //!
   TBranch        *b_WhCNCap;   //!
   TBranch        *b_PosCNCap;   //!
   TBranch        *b_EneCNCap;   //!
   TBranch        *b_NPheHits;   //!
   TBranch        *b_Phe;   //!
   TBranch        *b_PheTile;   //!
   TBranch        *b_PheTime;   //!
   TBranch        *b_TotPhe;   //!
   TBranch        *b_NPheHitsNCap;   //!
   TBranch        *b_PheNCap;   //!
   TBranch        *b_PheTileNCap;   //!
   TBranch        *b_PheTimeNCap;   //!
   TBranch        *b_TotPheNCap;   //!
   TBranch        *b_NIneHits;   //!
   TBranch        *b_TypeIne;   //!
   TBranch        *b_IdIne;   //!
   TBranch        *b_TrIne;   //!
   TBranch        *b_IdParIne;   //!
   TBranch        *b_IntParIne;   //!
   TBranch        *b_PIne;   //!
   TBranch        *b_PosIne;   //!
   TBranch        *b_TimeIne;   //!
   TBranch        *b_RegIne;   //!
   TBranch        *b_NeuOldIne;   //!
   TBranch        *b_NeuRegIne;   //!
   TBranch        *b_NSecIne;   //!
   TBranch        *b_FirstSec;   //!
   TBranch        *b_NTIneSec;   //!
   TBranch        *b_HitSecIne;   //!
   TBranch        *b_IdSecIne;   //!
   TBranch        *b_ASecIne;   //!
   TBranch        *b_ZSecIne;   //!
   TBranch        *b_TrSecIne;   //!
   TBranch        *b_NeuOldSec;   //!
   TBranch        *b_NeuRegSec;   //!
   TBranch        *b_NeuOldVtx;   //!
   TBranch        *b_PSec;   //!
  ///////////////////////////////////////
     // Declaration of leaf types
  //   Int_t           RunNum;
  //   Int_t           EveNum;
   Int_t           ReacType;
   Int_t           IntType;
   Float_t         Vertex[3];
   Int_t           TargZ;
   Int_t           TargA;
   Int_t           Target;
   Int_t           Primary;
   Float_t         P_Primary[5];
   Int_t           ncaptures;
   Float_t         Edeptot;
   Float_t         phetot;
   Int_t           NumLep;
   Int_t           IdLep[1];   //[NumLep]
   Int_t           TrLep[1];   //[NumLep]
   Float_t         P_Lep[1][5];   //[NumLep]
   Int_t           NumHad;
   Int_t           IdHad[1];   //[NumHad]
   Int_t           TrHad[1];   //[NumHad]
   Float_t         P_Had[1][5];   //[NumHad]
   Int_t           NumPhot;
   Int_t           IdPhot[1];   //[NumPhot]
   Int_t           TrPhot[1];   //[NumPhot]
   Float_t         P_Phot[1][5];   //[NumPhot]
   Int_t           NumHeavy;
   Int_t           IdHeavy[1];   //[NumHeavy]
   Int_t           TrHeavy[1];   //[NumHeavy]
   Float_t         P_Heavy[1][5];   //[NumHeavy]
   Int_t           NumTracks;
   Int_t           IdTrack[1];   //[NumTracks]
   Int_t           Int_Track[1];   //[NumTracks]
   Float_t         Vtx_Track[1][3];   //[NumTracks]
   Int_t           Nsec_Track[1];   //[NumTracks]
   Float_t         Esec_Track[1][3];   //[NumTracks]
   Float_t         Psec_Track[1][3][3];   //[NumTracks]

   // List of branches
  //   TBranch        *b_RunNum;   //!
  //   TBranch        *b_EveNum;   //!
   TBranch        *b_ReacType;   //!
   TBranch        *b_IntType;   //!
   TBranch        *b_Vertex;   //!
   TBranch        *b_TargZ;   //!
   TBranch        *b_TargA;   //!
   TBranch        *b_Target;   //!
   TBranch        *b_Primary;   //!
   TBranch        *b_P_Primary;   //!
   TBranch        *b_ncaptures;   //!
   TBranch        *b_Edeptot;   //!
   TBranch        *b_phetot;   //!
   TBranch        *b_NumLep;   //!
   TBranch        *b_IdLep;   //!
   TBranch        *b_TrLep;   //!
   TBranch        *b_P_Lep;   //!
   TBranch        *b_NumHad;   //!
   TBranch        *b_IdHad;   //!
   TBranch        *b_TrHad;   //!
   TBranch        *b_P_Had;   //!
   TBranch        *b_NumPhot;   //!
   TBranch        *b_IdPhot;   //!
   TBranch        *b_TrPhot;   //!
   TBranch        *b_P_Phot;   //!
   TBranch        *b_NumHeavy;   //!
   TBranch        *b_IdHeavy;   //!
   TBranch        *b_TrHeavy;   //!
   TBranch        *b_P_Heavy;   //!
   TBranch        *b_NumTracks;   //!
   TBranch        *b_IdTrack;   //!
   TBranch        *b_Int_Track;   //!
   TBranch        *b_Vtx_Track;   //!
   TBranch        *b_Nsec_Track;   //!
   TBranch        *b_Esec_Track;   //!
   TBranch        *b_Psec_Track;   //!

  /////////////
  //all in microseconds
  /*
	  TH1F* timeh1;
	if (timeh1 != NULL )
	{delete timeh1;}
  if (timeh2 != nullptr ) {delete timeh2;}
  if (tall != nullptr ) {delete tall;}
  if (tc != nullptr ) {delete tc;}
  */
  TH1F* timeh1=new TH1F("timeh1","no smear",1200,0.,1200.);
  TH1F* timeh2=new TH1F("timeh2","time smear",1200,0.,1200.);
  TH1F* timeh3=new TH1F("timeh3","time smear all bunches",1200,0.,1200.);
  TH1F* tall= new TH1F("tall","convoluted",75000,0.,1200.); 
  TH1F* tcglob= new TH1F("tcglob","convoluted",75000,0.,1200.); 
  TH1F* tcompact= new TH1F("tcompact","convoluted",1200,0.,1200.); 
  TH1F* tc=nullptr; 
  TH1F* timcap=new TH1F("timcap","capture ",1200,0.,1200.);
  TH1F* timine=new TH1F("timine","inelastic ",1200,0.,1200.);
  TH1F* timoth=new TH1F("timoth","no neutron int in LAr",1200,0.,1200.);
  TH1F* regneu=new TH1F("regneu","outside neutron reg",30,0.5,30.5);
  TH1F* intneu=new TH1F("intneu","outside neutron int",100,299.5,399.5);
  TH1F* energy_all=new TH1F("energy_all","energy deposition",100,0.,10.);
  TH1F* energy_cap1=new TH1F("energy_cap1","energy deposition with capture",100,0.,10.);
  TH1F* energy_cap2=new TH1F("energy_cap2","energy deposition from capture",100,0.,10.);
  TH1F* gamcap=new TH1F("gamcap","gamma energy from capture in Ar",100,0.,10.);
  TH1F* gamcapo=new TH1F("gamcapo","gamma energy from non-Ar int.",100,0.,10.);
  //all these are for events with neutron capture in LAr
  //try to do it log...
  //Energy of the neutron when it first exits from the inner
  //  membrane of the cryostat. Capture in active LAr
	TH1F* ene1st=new TH1F("ene1st","from cryo, first",50,-10,1.);
  //Energy of the neutron when it enters the active lar. Capture in active lar
  TH1F* ene1stin=new TH1F("ene1stin","entering act",50,-10,1.);
  //Energy of the neutron when it last exits from the inner
  //  membrane of the cryostat. Capture in active LAr
  TH1F* enelstcryo=new TH1F("enelstcryo","from cryo, last",50,-10,1.);
  //Energy of the neutron when it first exits from the inner
  //  membrane of the cryostat. Capture in active and inactive LAr
  TH1F* ene1stall=new TH1F("ene1stall","n E at cryo, all cap",50,-10,1);
  //   TH1F* nintlaract=new TH1F("nintlaract","n. of int in act lar",20,0.5,20.5);
  TH1F* nintlar=new TH1F("nintlar","last region bef. laract",30,0.5,30.5);
  //  TH1F* nintine=new TH1F("nintine","n. of int before lar",40,0.5,40.5);
  //Energy of electron clusters from capture in Ar
  TH1F* eelecap=new TH1F("eelecap","Capture in Ar",100.,0.,10.);
  //Energy of electron clusters from other materials
  TH1F* eeleout=new TH1F("eeleout","Other material",100.,0.,10.);
  //Energy of electron clusters from all interactions in Ar
  TH1F* eelear=new TH1F("eelear","All int in Ar",100.,0.,10.);
  TH2F* eall2d=new TH2F("eall2d","energy, all",85,-170.,+170., 75, -150., 150.);
  TH2F* ecap2d=new TH2F("ecap2d","energy, capt",85,-170.,+170., 75, -150., 150.);
  vbox(TTree *hitstree=0, TTree *headertree=0);
   virtual ~vbox();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *hitstree, TTree *headertree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef vbox_cxx
vbox::vbox(TTree *hitstree,TTree *headertree ) : fhits(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (headertree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("vbox_root_side001_Out.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("vbox_root_side001_Out.root");
      }
      f->GetObject("HitsTree",hitstree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * hchain = new TChain("HitsTree","");
      hchain->Add("vbox_sep24_side001_Out.root/HitsTree");
      hchain->Add("vbox_sep24_side002_Out.root/HitsTree");
      hchain->Add("vbox_sep24_side003_Out.root/HitsTree");
      hchain->Add("vbox_sep24_side004_Out.root/HitsTree");
      hchain->Add("vbox_sep24_side005_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sideb001_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sideb002_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sideb003_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sideb004_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sideb005_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sidec001_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sidec002_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sidec003_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sidec004_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sidec005_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sided001_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sided002_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sided003_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sided004_Out.root/HitsTree");
      hchain->Add("vbox_sep24_sided005_Out.root/HitsTree");
      hitstree = hchain;
      TChain * chain = new TChain("HeaderTree","");
      chain->Add("vbox_sep24_side001_Out.root/HeaderTree");
      chain->Add("vbox_sep24_side002_Out.root/HeaderTree");
      chain->Add("vbox_sep24_side003_Out.root/HeaderTree");
      chain->Add("vbox_sep24_side004_Out.root/HeaderTree");
      chain->Add("vbox_sep24_side005_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sideb001_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sideb002_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sideb003_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sideb004_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sideb005_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sidec001_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sidec002_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sidec003_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sidec004_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sidec005_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sided001_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sided002_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sided003_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sided004_Out.root/HeaderTree");
      chain->Add("vbox_sep24_sided005_Out.root/HeaderTree");
      headertree = chain;
#endif // SINGLE_TREE

   }
   Init(hitstree, headertree);
}

vbox::~vbox()
{
   if (!fhits) return;
   delete fhits->GetCurrentFile();
   delete fheader->GetCurrentFile();
}

Int_t vbox::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fhits) return 0;
   return fheader->GetEntry(entry);
}
Long64_t vbox::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fhits) return -5;
   Long64_t centry = fhits->LoadTree(entry);
   if (centry < 0) return centry;
   if (fhits->GetTreeNumber() != fCurrent) {
      fCurrent = fhits->GetTreeNumber();
      Notify();
   }
   return centry;
}

void vbox::Init(TTree *hitstree,TTree *headertree )
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!headertree) return;
   fhits = hitstree;
   fheader = headertree;
      fheader->AddFriend(fhits,"");

   fCurrent = -1;
   fhits->SetMakeClass(1);
   fheader->SetMakeClass(1);
   fheader->SetBranchAddress("RunNum", &RunNum, &b_RunNum);
   fheader->SetBranchAddress("EveNum", &EveNum, &b_EveNum);
   fheader->SetBranchAddress("Vertex", Vertex, &b_Vertex);
   fheader->SetBranchAddress("TargZ", &TargZ, &b_TargZ);
   fheader->SetBranchAddress("TargA", &TargA, &b_TargA);
   fheader->SetBranchAddress("Target", &Target, &b_Target);
   fheader->SetBranchAddress("Primary", &Primary, &b_Primary);
   fheader->SetBranchAddress("P_Primary", P_Primary, &b_P_Primary);
   fheader->SetBranchAddress("ncaptures", &ncaptures, &b_ncaptures);
   fheader->SetBranchAddress("Edeptot", &Edeptot, &b_Edeptot);
   fheader->SetBranchAddress("phetot", &phetot, &b_phetot);
   /*   fheader->SetBranchAddress("NumLep", &NumLep, &b_NumLep);
   fheader->SetBranchAddress("IdLep", &IdLep, &b_IdLep);
   fheader->SetBranchAddress("TrLep", &TrLep, &b_TrLep);
   fheader->SetBranchAddress("P_Lep", &P_Lep, &b_P_Lep);
   fheader->SetBranchAddress("NumHad", &NumHad, &b_NumHad);
   fheader->SetBranchAddress("IdHad", IdHad, &b_IdHad);
   fheader->SetBranchAddress("TrHad", TrHad, &b_TrHad);
   fheader->SetBranchAddress("P_Had", P_Had, &b_P_Had);
   fheader->SetBranchAddress("NumPhot", &NumPhot, &b_NumPhot);
   fheader->SetBranchAddress("IdPhot", &IdPhot, &b_IdPhot);
   fheader->SetBranchAddress("TrPhot", &TrPhot, &b_TrPhot);
   fheader->SetBranchAddress("P_Phot", &P_Phot, &b_P_Phot);
   fheader->SetBranchAddress("NumHeavy", &NumHeavy, &b_NumHeavy);
   fheader->SetBranchAddress("IdHeavy", &IdHeavy, &b_IdHeavy);
   fheader->SetBranchAddress("TrHeavy", &TrHeavy, &b_TrHeavy);
   fheader->SetBranchAddress("P_Heavy", &P_Heavy, &b_P_Heavy);
   fheader->SetBranchAddress("NumTracks", &NumTracks, &b_NumTracks);
   fheader->SetBranchAddress("IdTrack", IdTrack, &b_IdTrack);
   fheader->SetBranchAddress("Int_Track", Int_Track, &b_Int_Track);
   fheader->SetBranchAddress("Vtx_Track", Vtx_Track, &b_Vtx_Track);
   fheader->SetBranchAddress("Nsec_Track", Nsec_Track, &b_Nsec_Track);
   fheader->SetBranchAddress("Esec_Track", Esec_Track, &b_Esec_Track);
   fheader->SetBranchAddress("Psec_Track", Psec_Track, &b_Psec_Track);*/
   //
   //
   fhits->SetBranchAddress("RunNum", &RunNum, &b_RunNum);
   fhits->SetBranchAddress("EveNum", &EveNum, &b_EveNum);
   fhits->SetBranchAddress("NIncHits", &NIncHits, &b_NIncHits);
   fhits->SetBranchAddress("IdInc", IdInc, &b_IdInc);
   fhits->SetBranchAddress("IdParInc", IdParInc, &b_IdParInc);
   fhits->SetBranchAddress("TrInc", TrInc, &b_TrInc);
   fhits->SetBranchAddress("ParTrInc", ParTrInc, &b_ParTrInc);
   fhits->SetBranchAddress("IntParInc", IntParInc, &b_IntParInc);
   fhits->SetBranchAddress("RegInc", RegInc, &b_RegInc);
   fhits->SetBranchAddress("PosInc", PosInc, &b_PosInc);
   fhits->SetBranchAddress("PInc", PInc, &b_PInc);
   fhits->SetBranchAddress("TimeInc", TimeInc, &b_TimeInc);
   fhits->SetBranchAddress("NCalHits", &NCalHits, &b_NCalHits);
   fhits->SetBranchAddress("WhCal", WhCal, &b_WhCal);
   fhits->SetBranchAddress("PosCal", PosCal, &b_PosCal);
   fhits->SetBranchAddress("EneCal", EneCal, &b_EneCal);
   fhits->SetBranchAddress("NCHitNCap", &NCHitNCap, &b_NCHitNCap);
   fhits->SetBranchAddress("WhCNCap", WhCNCap, &b_WhCNCap);
   fhits->SetBranchAddress("PosCNCap", PosCNCap, &b_PosCNCap);
   fhits->SetBranchAddress("EneCNCap", EneCNCap, &b_EneCNCap);
   fhits->SetBranchAddress("NPheHits", &NPheHits, &b_NPheHits);
   fhits->SetBranchAddress("Phe", Phe, &b_Phe);
   fhits->SetBranchAddress("PheTile", PheTile, &b_PheTile);
   fhits->SetBranchAddress("PheTime", PheTime, &b_PheTime);
   fhits->SetBranchAddress("TotPhe", TotPhe, &b_TotPhe);
   fhits->SetBranchAddress("NPheHitsNCap", &NPheHitsNCap, &b_NPheHitsNCap);
   fhits->SetBranchAddress("PheNCap", PheNCap, &b_PheNCap);
   fhits->SetBranchAddress("PheTileNCap", PheTileNCap, &b_PheTileNCap);
   fhits->SetBranchAddress("PheTimeNCap", PheTimeNCap, &b_PheTimeNCap);
   fhits->SetBranchAddress("TotPheNCap", TotPheNCap, &b_TotPheNCap);
   fhits->SetBranchAddress("NIneHits", &NIneHits, &b_NIneHits);
   fhits->SetBranchAddress("TypeIne", TypeIne, &b_TypeIne);
   fhits->SetBranchAddress("IdIne", IdIne, &b_IdIne);
   fhits->SetBranchAddress("TrIne", TrIne, &b_TrIne);
   fhits->SetBranchAddress("IdParIne", IdParIne, &b_IdParIne);
   fhits->SetBranchAddress("IntParIne", IntParIne, &b_IntParIne);
   fhits->SetBranchAddress("PIne", PIne, &b_PIne);
   fhits->SetBranchAddress("PosIne", PosIne, &b_PosIne);
   fhits->SetBranchAddress("TimeIne", TimeIne, &b_TimeIne);
   fhits->SetBranchAddress("RegIne", RegIne, &b_RegIne);
   fhits->SetBranchAddress("NSecIne", NSecIne, &b_NSecIne);
   fhits->SetBranchAddress("FirstSec", FirstSec, &b_FirstSec);
   fhits->SetBranchAddress("NTIneSec", &NTIneSec, &b_NTIneSec);
   fhits->SetBranchAddress("HitSecIne", HitSecIne, &b_HitSecIne);
   fhits->SetBranchAddress("IdSecIne", IdSecIne, &b_IdSecIne);
   fhits->SetBranchAddress("NeuOldSec", NeuOldSec, &b_NeuOldSec);
   fhits->SetBranchAddress("NeuRegSec", NeuRegSec, &b_NeuRegSec);
   fhits->SetBranchAddress("NeuOldVtx", NeuOldVtx, &b_NeuOldVtx);
   fhits->SetBranchAddress("ASecIne", ASecIne, &b_ASecIne);
   fhits->SetBranchAddress("ZSecIne", ZSecIne, &b_ZSecIne);
   fhits->SetBranchAddress("TrSecIne", TrSecIne, &b_TrSecIne);
   fhits->SetBranchAddress("PSec", PSec, &b_PSec);
   Notify();
}

Bool_t vbox::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void vbox::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fhits) return;
   fhits->Show(entry);
   fheader->Show(entry);
}
Int_t vbox::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef vbox_cxx
