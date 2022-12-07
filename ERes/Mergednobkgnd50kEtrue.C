#define Mergednobkgnd50kEtrue_cxx
#include "Mergednobkgnd50kEtrue.h"
#include <TH2.h>
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
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
int LYcutoff = 0; // PE/MeV, cut off on LY, LY smaller than this won't be considered

void Mergednobkgnd50kEtrue::Loop()
{
  TFile f(Form("Merged_Etruecal_%d.root", LYcutoff),"RECREATE");

  // voxels in x, y, z
  int nbinx=35;
  int xmin=-350;
  int xmax=350;

  int nbiny=12;
  int ymin=-600;
  int ymax=600;

  int nbinz=8;
  int zmin=600;
  int zmax=1400;

  int ebin=30;
  double emin=0.5;
  double emax=30.5;
  int bine=1;

  int binx=20;
  int biny=100;
  int binz=100;

  vector<vector<vector<vector<float>>>> LY_XYZ;
  vector<vector<vector<float>>> LY_XY;
  TH1D *LY[nbinx][nbiny][nbinz];

  // 3D vector for light yield-energy calibration
  LY_XYZ.resize(nbinx);
  for(int i=0;i<nbinx;i++) LY_XYZ[i].resize(nbiny);
  for(int i=0;i<nbinx;i++){
    for(int j=0;j<nbiny;j++){
      LY_XYZ[i][j].resize(nbinz);
    }
  }

  // 2D vector for light yield-energy calibration
  LY_XY.resize(nbiny);
  for(int i=0;i<nbiny;i++) LY_XY[i].resize(nbinx);

  int xval=0;
  int yval=0;
  int zval=0;
  int energy_bin=0;
  TH2D *PEvsEnergy1=new TH2D("PEvsEnergy1", ";PE;Energy[MeV]", 500,0,5000, 100,0,100);
  TH2D *PEvsEnergy2=new TH2D("PEvsEnergy2", ";PE;Energy[MeV]", 500,0,5000, 100,0,100);

  TProfile2D *xy_pe=new TProfile2D("xy_pe", ";Y [cm];X [cm]", nbiny,ymin,ymax, nbinx,xmin,xmax);
  TProfile   *LY_x =new TProfile("LY_x", ";X [cm];LY [PE/MeV]", nbinx,xmin,xmax);
  TProfile   *LY_y =new TProfile("LY_y", ";Y [cm];LY [PE/MeV]", 5*nbiny,ymin,ymax);
  TProfile   *LY_z =new TProfile("LY_z", ";Z [cm];LY [PE/MeV]", 5*nbinz,zmin,zmax);

  TH3D *LY_XYZcal             =new TH3D("LY_XYZcal", "", nbinx,xmin,xmax, nbiny,ymin,ymax, nbinz,zmin,zmax);
  TH2D *LY_map                =new TH2D("LY_map", "", nbiny,ymin,ymax, nbinx,xmin,xmax);
  TH2D *TrueE_vs_TrueE        =new TH2D("TrueE_vs_TrueE", "", 50,0,50, 50,0,50);
  TH2D *X_vs_Purity           =new TH2D("X_vs_Purity", "", nbinx,xmin,xmax, 20,0,1);
  TH2D *LY_vs_visenergy       =new TH2D("LY_vs_visenergy", ";visible energy[MeV];LY [PE/MeV]", 100,0,100, 100,0,100);
  TH2D *LY_vs_tenergy         =new TH2D("LY_vs_tenergy", ";true energy[MeV];LY [PE/MeV]", 100,0,100, 100,0,100);
  TH2D *LY_vs_visenergyallbins=new TH2D("LY_vs_visenergyallbins", ";visible energy[MeV];LY [PE/MeV]", 100,0,100, 100,0,100);

  int bin=0;
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  std::cout<< "nentries: " << nentries <<std::endl;

  std::cout<<"event loop start "<<std::endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChain->GetEntry(ientry);

    float TotalPE=0.0;
    for(Size_t i=0; i<TotalPEVector->size(); i++){
      TotalPE += TotalPEVector->at(i);
      X_vs_Purity->Fill(TrueX, Purity->at(i));
    }

    TrueE = TrueE*1000; // unit MeV
    Edep  = Edep*1000;  // unit MeV
    if( TotalPE == 0 ) continue; // no light
    if( TrueE > 35 ) continue;   // we only care below 35MeV
    if( TotalPE/TrueE < LYcutoff ) continue; // Skip is LY is below cut-off

    TrueE_vs_TrueE->Fill(TrueE,TrueE); // This seems useless

    if(TrueZ>1000 && TrueZ<1100){ // why?
    xy_pe->Fill(TrueY, TrueX, TotalPE/TrueE);
    energy_bin=int(TrueE/ebin);
    //  if(energy_bin>ebin-1) energy_bin=ebin-1;
    xval=(TrueX-xmin)/binx;
    yval=(TrueY-ymin)/biny;
    zval=(TrueZ-zmin)/binz;
    LY_x->Fill(TrueX, TotalPE/Edep);
    LY_y->Fill(TrueY, TotalPE/Edep);
    LY_z->Fill(TrueZ, TotalPE/Edep);

    if(xval>=0 && xval<nbinx && yval>=0 && yval<nbiny && zval>=0 && zval<nbinz){
      LY_XYZ[xval][yval][zval].push_back(TotalPE/TrueE);
      LY_XY[yval][xval].push_back(TotalPE/TrueE);
    }
    LY_vs_visenergyallbins->Fill(TrueE,TotalPE/TrueE);
    if(TrueX>-50 && TrueX<50 && TrueY>-50 && TrueY<50 && TrueZ>900 && TrueZ<1100){

      LY_vs_visenergy->Fill(TrueE,TotalPE/TrueE);
      LY_vs_tenergy->Fill(TrueE,TotalPE/TrueE);

    }

    if(TrueX>-250 && TrueX<-150 && TrueY>-50 && TrueY<50 && TrueZ>900 && TrueZ<1100){
      PEvsEnergy1->Fill(TotalPE, TrueE);
    }
    if(TrueX>-250 && TrueX<-150 && TrueY>-50 && TrueY<50 && TrueZ>700 && TrueZ<900){
      PEvsEnergy2->Fill(TotalPE, TrueE);
    }

    }//Z pos
  } // end event loop

  std::cout<<"event loop end "<<std::endl;

  for(int i=0;i<nbinx;i++){
    for(int j=0;j<nbiny;j++){
      for(int k=0;k<nbinz;k++){
        LY_XYZcal->SetBinContent(i+1,j+1,k+1,TMath::Median(LY_XYZ[i][j][k].size(),&LY_XYZ[i][j][k][0]));
      }
    }
  }

 for(int i=0;i<nbiny;i++){
    for(int j=0;j<nbinx;j++){
      LY_map->SetBinContent(i+1,j+1,TMath::Median(LY_XY[i][j].size(),&LY_XY[i][j][0]));
    }
  }
 int xcoor=0;
 int ycoor=0;
 int zcoor=0;

 for(int i=0;i<nbinx;i++){
   for(int j=0;j<nbiny;j++){
     for(int k=0;k<nbinz;k++){
       LY[i][j][k]=new TH1D(Form("LY_%d_%d_%d",i,j,k),"",200,0,50);
       for(int l=0;l<LY_XYZ[i][j][k].size();l++){
         LY[i][j][k]->Fill(LY_XYZ[i][j][k][l]);
       }
       xcoor=xmin+i*binx+binx/2;
       ycoor=ymin+j*biny+biny/2;
       zcoor=zmin+k*binz+binz/2;
       LY[i][j][k]->Write(Form("LYdist_X_%d_Y_%d_Z_%d",xcoor,ycoor,zcoor));
     }
   }
 }

  TrueE_vs_TrueE->Write();
  xy_pe->Write();
  LY_map->Write();
  LY_XYZcal->Write();
  X_vs_Purity->Write();
  LY_vs_visenergy->Write();
  LY_vs_visenergyallbins->Write();
  LY_vs_tenergy->Write();
  LY_vs_visenergy->Draw("colz");
  PEvsEnergy1->Write();
  PEvsEnergy2->Write();
  LY_x->Write();
  LY_y->Write();
  LY_z->Write();
  f.Close();
}
