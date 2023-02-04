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
int EvtLYCut = 0; // unit: PE/MeV, cut off on LY, events with smaller LY won't be considered

void Mergednobkgnd50kEtrue::Loop()
{
  TFile f(Form("LY_Calib_Etrue_EvtLYCut_%d.root", EvtLYCut),"RECREATE");

  // Marley events are generated in a box, min_position: [ -325., -675., 600. ], max_position: [ 325., 675., 1500. ]

  // Voxels in x, y, z
  // x 20cm resolution
  // y, z 100cm resolution
  int nbinx = 35;
  int xmin  = -350;
  int xmax  = 350;

  int nbiny = 13;
  int ymin  = -650;
  int ymax  = 650;

  int nbinz = 8;
  int zmin  = 650;
  int zmax  = 1450;

  bool CheckEachVoxel = false; // for debug purpose only

  // Calculate bin size
  int binx = (xmax-xmin)/nbinx;
  int biny = (ymax-ymin)/nbiny;
  int binz = (zmax-zmin)/nbinz;

  // LY calibration:
  // each x-y-z voxel has an avg LY, push back event by event
  vector<vector<vector<vector<float>>>> LY_XYZ;
  // avged over z
  vector<vector<vector<float>>> LY_XY;

  // 1D histogram light yield for every voxel
  TH1D *LY[nbinx][nbiny][nbinz];

  // 4D vector for light yield-energy calibration
  // for each voxel (x, y, z) --> PEs
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

  TH2D *PEvsEnergy1=new TH2D("PEvsEnergy1", ";PE;Energy[MeV]", 500,0,5000, 100,0,100);
  TH2D *PEvsEnergy2=new TH2D("PEvsEnergy2", ";PE;Energy[MeV]", 500,0,5000, 100,0,100);

  TProfile2D *xy_pe=new TProfile2D("xy_pe", ";Y [cm];X [cm]", nbiny,ymin,ymax, nbinx,xmin,xmax);
  TProfile   *LY_x =new TProfile("LY_x", ";X [cm];LY [PE/MeV]", nbinx,xmin,xmax);
  TProfile   *LY_y =new TProfile("LY_y", ";Y [cm];LY [PE/MeV]", 5*nbiny,ymin,ymax);
  TProfile   *LY_z =new TProfile("LY_z", ";Z [cm];LY [PE/MeV]", 5*nbinz,zmin,zmax);

  TH3D *LY_XYZcal             =new TH3D("LY_XYZcal", "", nbinx,xmin,xmax, nbiny,ymin,ymax, nbinz,zmin,zmax);
  TH2D *LY_map                =new TH2D("LY_map", "", nbiny,ymin,ymax, nbinx,xmin,xmax);
  TH2D *X_vs_Purity           =new TH2D("X_vs_Purity", "", nbinx,xmin,xmax, 20,0,1);
  TH1D *FlashHitPurity        =new TH1D("FlashHitPurity", "", 20, 0, 1);
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
    // N reconstructed flashes, each flash contains some number of PE
    for(Size_t i=0; i<TotalPEVector->size(); i++){
      TotalPE += TotalPEVector->at(i);
      X_vs_Purity->Fill(TrueX, Purity->at(i));
    }

    TrueE = TrueE*1000; // unit MeV
    Edep  = Edep*1000;  // unit MeV
    if( TotalPE == 0 ) continue; // Skip event with no light yield
    if( TrueE > 40 ) continue;   // we only care low E events below 40MeV
    // Skip event with avg LY is below some cut-off
    if( TotalPE/TrueE < EvtLYCut ) continue;

    // VDTDR: at least 50cm from side wall, 25cm from anode and cathode.
    if(TrueX>-275 && TrueX<275 && TrueY>-650 && TrueY<650 && TrueZ>650 && TrueZ<1450){
      xy_pe->Fill(TrueY, TrueX, TotalPE/TrueE);

      for(Size_t i=0; i<TotalPEVector->size(); i++){
        FlashHitPurity->Fill(Purity->at(i)); // purity: how many reco hits match to true hits
      }

      // Locate event in defined voxels
      //
      // !!! There is some problem here:
      // the true position only represents the intereaction position
      // but the event can span several voxels, do we still acount all PE to a single voxel?
      //
      xval=(TrueX-xmin)/binx;
      yval=(TrueY-ymin)/biny;
      zval=(TrueZ-zmin)/binz;

      LY_x->Fill(TrueX, TotalPE/Edep);
      LY_y->Fill(TrueY, TotalPE/Edep);
      LY_z->Fill(TrueZ, TotalPE/Edep);

      if(xval>=0 && xval<nbinx && yval>=0 && yval<nbiny && zval>=0 && zval<nbinz){
        LY_XYZ[xval][yval][zval].push_back(TotalPE/TrueE); // event by event
        LY_XY[yval][xval].push_back(TotalPE/TrueE); // why would this be interesting
      }

      LY_vs_visenergyallbins->Fill(TrueE, TotalPE/TrueE);
      // what's doing here?
      if(TrueX>-50 && TrueX<50 && TrueY>-50 && TrueY<50 && TrueZ>900 && TrueZ<1100){
        LY_vs_visenergy->Fill(TrueE,TotalPE/TrueE);
        LY_vs_tenergy->Fill(TrueE,TotalPE/TrueE);
      }

      if(TrueX>-250 && TrueX<-150 && TrueY>-50 && TrueY<50 && TrueZ>900 && TrueZ<1100) PEvsEnergy1->Fill(TotalPE, TrueE);
      if(TrueX>-250 && TrueX<-150 && TrueY>-50 && TrueY<50 && TrueZ>700 && TrueZ<900) PEvsEnergy2->Fill(TotalPE, TrueE);
    }//Z pos
  } // end event loop

  std::cout<<"event loop end "<<std::endl;

  // Calibration uses the median LY among all events in the voxel
  for(int i=0;i<nbinx;i++){
    for(int j=0;j<nbiny;j++){
      for(int k=0;k<nbinz;k++){
        LY_XYZcal->SetBinContent(i+1,j+1,k+1,TMath::Median(LY_XYZ[i][j][k].size(),&LY_XYZ[i][j][k][0]));
      }
    }
  }

  //
  // From here the resolution can already be calculated, why bother another macro
  //



  for(int i=0;i<nbiny;i++){
    for(int j=0;j<nbinx;j++){
      LY_map->SetBinContent(i+1,j+1,TMath::Median(LY_XY[i][j].size(),&LY_XY[i][j][0]));
    }
  }

  int xcoor=0;
  int ycoor=0;
  int zcoor=0;

  if ( CheckEachVoxel ) {
    for(int i=0;i<nbinx;i++){
      for(int j=0;j<nbiny;j++){
        for(int k=0;k<nbinz;k++){
          LY[i][j][k]=new TH1D(Form("LY_%d_%d_%d",i,j,k),"",200,0,50);

          for(unsigned int l=0;l<LY_XYZ[i][j][k].size();l++) LY[i][j][k]->Fill(LY_XYZ[i][j][k][l]); // LY for each voxel, all events filled

          xcoor=xmin+i*binx+binx/2;
          ycoor=ymin+j*biny+biny/2;
          zcoor=zmin+k*binz+binz/2;

          LY[i][j][k]->Write(Form("LYdist_X_%d_Y_%d_Z_%d",xcoor,ycoor,zcoor));
        }
      }
    }
  } // end check each voxel

  xy_pe->Write();
  LY_map->Write();
  LY_XYZcal->Write();
  X_vs_Purity->Write();
  FlashHitPurity->Write();
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
