#define Mergednobkgnd50kEtrue_Eres_cxx
#include "Mergednobkgnd50kEtrue_Eres.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <iostream>
#include <vector>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TProfile2D.h>
#include <TSpline.h>
#include <TMath.h>
#include<TH3.h>
#include <numeric>
#include <functional>
#include <fstream>
#include <iomanip>

int LYcutoff = 0;

void Mergednobkgnd50kEtrue_Eres::Loop()
{
  TFile f1(Form("Merged_Etruecal_%d.root", LYcutoff));
  TH3D *Xcorr=(TH3D*)f1.Get("LY_XYZcal");
  TFile f(Form("Merged_Etrue_Eres%d.root", LYcutoff), "RECREATE");

  ofstream outfile;
  outfile.open("minLY_avgLY_Etruev1.txt",std::ios_base::app);

  std::cout<<"code starting "<<std::endl;
  int nbinx=35;
  int xmin=-350;
  int xmax=350;

  int nbiny=12;
  int ymin=-600;
  int ymax=600;
  std::vector<double> LYavg;

  // this macro assumes fitting several mono-energies from emin to emax with bin step bine [MeV]
  int ebin=5;
  double emin=5;
  double emax=30;
  double bine=5;

  int nbinz=8;
  int zmin=600;
  int zmax=1400;

  int binx=20;
  int biny=100;
  int binz=100;

  int energy_bin=0;

  TProfile2D *xy_pe=new TProfile2D("xy_pe",";Y [cm];X [cm]",nbiny,ymin,ymax,nbinx,xmin,xmax);
  TH2D *LY_map=new TH2D("LY_map","",nbiny,ymin,ymax,nbinx,xmin,xmax);
  TH2D *TrueE_vs_Edep=new TH2D("TrueE_vs_Edep","",50,0,50,50,0,50);
  TH2D *X_vs_Purity=new TH2D("X_vs_Purity","",nbinx,xmin,xmax,20,0,1);
  TH1D *LYvaluesall=new TH1D("LYvaluesall","",200,0,200);

  TH2D *res_vs_eng=new TH2D("res_vs_eng","",7,0,35, 500,-10,10);

  TH1D *eres[ebin];
  for(int i=0;i<ebin;i++) eres[i]=new TH1D(Form("eres_%d",i),";(reco energy -true E)/true E;entries",500,-10,10);
  TProfile *E_resolution=new TProfile("E_resolution",";TrueE [MeV];(recoE-trueE)/TrueE",6,2.5,32.5);
  TH2D *TrueE_vs_recoE=new TH2D("TrueE_vs_recoE",";TrueE[MeV];recoE[MeV]",100,0,100,100,0,100);

  int bin=0;
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  std::cout<<"nentries: "<<nentries<<std::endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChain->GetEntry(ientry);

    float TotalPE=0.0;
    for(Size_t i=0;i<TotalPEVector->size();i++) TotalPE+=TotalPEVector->at(i);

    TrueE=TrueE*1000; // MeV
    Edep=Edep*1000;
    if( TotalPE == 0 ) continue;
    double recoE=0.0;
    if( TrueE > 35) continue;

    TrueE_vs_Edep->Fill(TrueE,Edep);
    if(TrueX>-325 && TrueX<325 && TrueY>-550 && TrueY<550 && TrueZ>650 && TrueZ<1350){
      xy_pe->Fill(TrueY,TrueX,TotalPE/(TrueE));
      energy_bin=int((TrueE-emin)/bine);

      if(energy_bin>ebin-1 || energy_bin<0) continue;

      recoE=TotalPE/Xcorr->Interpolate(TrueX, TrueY, TrueZ);
      if(TotalPE/TrueE<LYcutoff) continue;
      LYvaluesall->Fill(TotalPE/TrueE);
      LYavg.push_back(TotalPE/TrueE);
      if(Xcorr->Interpolate(TrueX, TrueY, TrueZ)<1 || Xcorr->Interpolate(TrueX, TrueY, TrueZ)>100) continue;
      TrueE_vs_recoE->Fill(TrueE,recoE);
      eres[energy_bin]->Fill((recoE-TrueE)/(TrueE));
      res_vs_eng->Fill(TrueE,(recoE-TrueE)/(TrueE));
    } // if pos
  } //ientry

  double mean1=0;
  double sig1=0;

  /*std::vector<double> E,errE,Res,errRes;
  for(int i=0;i<6;i++){
    //	eres[i]->Write(Form("eres_%f",5*i+2.5));
    eres[i]->GetXaxis()->SetRangeUser(-2.5,2.5);
    std::cout<<"Entries vs bin "<<i<<" "<<eres[i]->GetEntries()<<std::endl;
    if(eres[i]->GetEntries()<20) continue;
    eres[i]->Fit("gaus","R");
    eres[i]->Write(Form("eres_%f",i+5.0));
    TF1 *fun=eres[i]->GetFunction("gaus");
    E.push_back(i*5+5.0);
    errE.push_back(2.5);
    Res.push_back(fun->GetParameter(2));
    errRes.push_back(fun->GetParError(2));
  }
  TGraphErrors *Evsres=new TGraphErrors(E.size(),&E[0],&Res[0],&errE[0],&errRes[0]);*/

  std::vector<double> E,errE,Res,errRes;
  for(int i=0;i<ebin;i++){
    //	eres[i]->Write(Form("eres_%f",5*i+2.5));
    if(eres[i]->GetEntries()<20) continue; // can't fit with too small stats
    int binmax=eres[i]->GetMaximumBin();
    mean1=eres[i]->GetXaxis()->GetBinCenter(binmax);
    sig1=eres[i]->GetStdDev();

    eres[i]->GetXaxis()->SetRangeUser(-2.5,2.5);
    std::cout<<"Entries vs bin "<<i<<" "<<eres[i]->GetEntries()<<std::endl;

    eres[i]->Fit("gaus","","",mean1-sig1/2.0,mean1+sig1/2.0);
    //  eres[i]->Fit("gaus","","",mean1-sig1,mean1+sig1);//LY =0
    eres[i]->Write(Form("eres_%f",i*2.5+5.0));
    TF1 *fun=eres[i]->GetFunction("gaus");
    E.push_back((i*2.5+5.0)/1000.0);
    errE.push_back(0.0);
    Res.push_back(fun->GetParameter(2));
    errRes.push_back(fun->GetParError(2));
  }
  TGraphErrors *Evsres=new TGraphErrors(E.size(),&E[0],&Res[0],&errE[0],&errRes[0]);
  Evsres->SetTitle(";True neutrino energy [MeV];Enenergy resolution;");
  double avgLY=std::accumulate(LYavg.begin(), LYavg.end(),0.0)/LYavg.size();
  LYavg.clear();
  Evsres->Write("Evsres");
  TrueE_vs_Edep->Write();
  xy_pe->Write();
  X_vs_Purity->Write();
  TrueE_vs_recoE->Write();
  std::cout<<"Average LY is "<<avgLY<<std::endl;
  outfile<<"minLY "<<LYcutoff<<"  Avg LY "<<avgLY<<std::endl;
  LYvaluesall->Write();
  res_vs_eng->Write();
  f.Close();
}
