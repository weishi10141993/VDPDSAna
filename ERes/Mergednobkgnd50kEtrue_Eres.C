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

int EvtLYCut = 0;

void Mergednobkgnd50kEtrue_Eres::Loop()
{
  TFile f1(Form("LY_Calib_Etrue_EvtLYCut_%d.root", EvtLYCut));
  TH3D *Xcorr=(TH3D*)f1.Get("LY_XYZcal");

  TFile f(Form("PDS_ERes_Etrue_%d.root", EvtLYCut), "RECREATE");

  int nbinx = 35;
  int xmin  = -350;
  int xmax  = 350;

  int nbiny = 12;
  int ymin  = -600;
  int ymax  = 600;

  int nbinz = 8;
  int zmin  = 600;
  int zmax  = 1400;

  // True energy binning
  // this macro assumes fitting several mono-energies from emin to emax with bin step bine [MeV]
  int ebin=6;
  double emin=5;
  double emax=35;

  // Calculate bin size
  int binx = (xmax-xmin)/nbinx;
  int biny = (ymax-ymin)/nbiny;
  int binz = (zmax-zmin)/nbinz;
  int bine = (emax-emin)/ebin;

  std::vector<double> LYavg;

  int energy_bin=0;

  TProfile2D *xy_pe=new TProfile2D("xy_pe",";Y [cm];X [cm]",nbiny,ymin,ymax,nbinx,xmin,xmax);
  TH2D *LY_map=new TH2D("LY_map","",nbiny,ymin,ymax,nbinx,xmin,xmax);
  TH2D *TrueE_vs_Edep=new TH2D("TrueE_vs_Edep","",50,0,50,50,0,50);
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
    double recoE=0.0;
    if( TotalPE == 0 ) continue;
    if( TrueE > 40 ) continue;   // we only care low E events below 40MeV
    if( TotalPE/TrueE < EvtLYCut ) continue;

    TrueE_vs_Edep->Fill(TrueE,Edep);
    if(TrueX>-325 && TrueX<325 && TrueY>-550 && TrueY<550 && TrueZ>650 && TrueZ<1350){
      xy_pe->Fill(TrueY,TrueX,TotalPE/(TrueE));
      energy_bin=int((TrueE-emin)/bine);

      if(energy_bin>ebin-1 || energy_bin<0) continue;

      recoE = TotalPE/(Xcorr->Interpolate(TrueX, TrueY, TrueZ));

      LYvaluesall->Fill(TotalPE/TrueE);
      LYavg.push_back(TotalPE/TrueE);

      if(Xcorr->Interpolate(TrueX, TrueY, TrueZ)<1 || Xcorr->Interpolate(TrueX, TrueY, TrueZ)>100) continue;

      TrueE_vs_recoE->Fill(TrueE, recoE);
      eres[energy_bin]->Fill((recoE - TrueE)/(TrueE));
      res_vs_eng->Fill(TrueE,(recoE - TrueE)/(TrueE));
    } // if pos
  } //ientry

  //
  // Let it fit!
  //
  double mean1=0;
  double sig1=0;

  std::vector<double> E, errE, ERes, errERes, EBias, errEBias;
  for(int i=0; i<ebin; i++){

    if(eres[i]->GetEntries()<20) continue; // can't fit with too small stats
    int binmax=eres[i]->GetMaximumBin();
    mean1=eres[i]->GetXaxis()->GetBinCenter(binmax);
    sig1=eres[i]->GetStdDev();

    eres[i]->GetXaxis()->SetRangeUser(-2.5,2.5);
    std::cout<<"Entries vs bin "<<i<<" "<<eres[i]->GetEntries()<<std::endl;

    eres[i]->Fit("gaus","","",mean1-sig1/2.0,mean1+sig1/2.0);
    //  eres[i]->Fit("gaus","","",mean1-sig1,mean1+sig1);//LY =0
    eres[i]->Write(Form("eres_%f",i*bine + emin));
    TF1 *fun=eres[i]->GetFunction("gaus");

    E.push_back((i*bine + emin)/1000.0); // GeV
    errE.push_back(0.0);

    EBias.push_back(fun->GetParameter(1));
    errEBias.push_back(fun->GetParError(1));

    ERes.push_back(fun->GetParameter(2));
    errERes.push_back(fun->GetParError(2));
  }

  TGraphErrors *EvsBias=new TGraphErrors(E.size(),&E[0],&EBias[0],&errE[0],&errEBias[0]);
  EvsBias->SetTitle(";True neutrino energy [GeV];Energy bias;");
  EvsBias->Write("EvsBias");

  TGraphErrors *EvsRes=new TGraphErrors(E.size(),&E[0],&ERes[0],&errE[0],&errERes[0]);
  EvsRes->SetTitle(";True neutrino energy [GeV];Energy resolution;");
  EvsRes->Write("EvsRes");

  double avgLY=std::accumulate(LYavg.begin(), LYavg.end(),0.0)/LYavg.size();
  LYavg.clear();
  std::cout<<"Average LY (over all events): " << avgLY << " PE/MeV, min event LY: " << EvtLYCut << " PE/MeV" <<std::endl;

  TrueE_vs_Edep->Write();
  xy_pe->Write();
  TrueE_vs_recoE->Write();
  LYvaluesall->Write();
  res_vs_eng->Write();

  f.Close();
}
