#define vbox_cxx
#include "vbox_sep24.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void vbox::Loop()
{
//   In a ROOT session, you can do:
//      root> .L vbox_sep24.C
//      root> vbox t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fheader == 0) return;
   TRandom *myran       = new TRandom();
   Double_t rr=0.0, rr2=0.0, offset1=0.0, offset2=0.0;
   Long64_t nentries = fheader->GetEntriesFast();
   int nmaxima=0;
   ofstream myfile;
   int myevent=-1;
   int nnmax=100;
   Long64_t nbytes = 0, nb = 0, nbh=0;
   Double_t response[115];
   Double_t thr1=10.0; //total phe on 1 tile to get above threshold
   //  Double_t thr2=14.5*10.;
   Double_t thr2=14.5*5.; // 5 phe
   Double_t effi=0.3; // Efficiency. Factor 0.10 already accounted in sim.
   Double_t p2pe=1.2; // 1 Photon corresponds to p2pe * PhE  based on the over-voltage and SiPMs
   TFile* rfile= TFile::Open("sample.root");
   //            TCanvas *c2 =new TCanvas("c2","c2",1500,1500);
   //gROOT->ForceStyle(kTRUE);
   //gStyle->SetHistLineWidth(5);
gStyle->SetOptStat(11);
   TCanvas* h = nullptr;
rfile->GetObject("cpers_s",h);
 TH1D *avg = (TH1D*)h->GetPrimitive("haverage_s_Ch0");
	 for (int i=0; i<115;  i++){
	   response[i]=avg->GetBinContent(i+388);
	 }
	   Int_t imax=avg->GetMaximumBin();
	   cout << imax << " max " << response[imax-388] << endl;

   rfile->Close();
   int ngoodevts=0;
   int ngoodevts_noconv=0;
   int ntotevts=0;
   int ncapevt=0;
   int doconv=1;
   int capC1counter=0;
   int capC2counter=0;
   int capC3counter=0;
   int capC4counter=0;
   //   c2->cd();
   //   			  myfile.open("dump.dat");

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fhits->GetEntry(jentry);   nbytes += nb;
      nbh = fheader->GetEntry(jentry); //  nbytes += nb;
      if (tc != nullptr ) {delete tc;}

	  tc= new TH1F("tc","convoluted",75000,0.,1200.);
	  tc->GetXaxis()->SetRange(1,75000);
	  int nnvtx =0;
	  int tnvtx=0;
	  ntotevts++;
	  // first all plots without threshold on phe
	  int intlar=0;   //how many in lar before  and includingcapture
	  int intlarac=0;  //how many in active lar before and includingcaprure
	  int intine=0;    //how many in total
	  int i1stlar=0; //which was the first int in act LAr, capture in act
	  int i1stlarall=0; //which was the first int in LAr, capture all

	  //fill energy
	  energy_all->Fill(Edeptot*1000.);
	  for (int ihit=0; ihit<NCalHits; ihit++)
		{
		  eall2d->Fill(PosCal[ihit][1],PosCal[ihit][2],EneCal[ihit]);
		}

    if (ncaptures>0){
      ncapevt++;
      energy_cap1->Fill(Edeptot*1000.);
      Float_t ee=0.0;
      for (int ihit=0; ihit<NCHitNCap; ihit++)
      {
        ecap2d->Fill(PosCNCap[ihit][1],PosCNCap[ihit][2],EneCNCap[ihit]);
			  ee+=EneCNCap[ihit];
		  }
		  /*			if (ee < 0.1*Edeptot) {
					cout << RunNum << " " << EveNum << " has low e"  << ee*1000. << " hits" << NCHitNCap<<" etot " << Edeptot*1000. << endl;}*/
		  energy_cap2->Fill(ee*1000.);
      for (int ihit=0; ihit<NIncHits; ihit++) {
        if ((RegInc[ihit]==-4 | RegInc[ihit]==-5) && IdInc[ihit]==2112 ) {
          ene1st->Fill(log10(PInc[ihit][4]*1000.));
          ene1stall->Fill(log10(PInc[ihit][4]*1000.));
				  break;
        }
      }
		  for (int ihit=NIncHits-1; ihit>-1; ihit--){
        if ((RegInc[ihit]==-4 | RegInc[ihit]==-5) && IdInc[ihit]==2112 )
        {
          enelstcryo->Fill(log10(PInc[ihit][4]*1000.));
				  break;
			  }
		  }
		  for (int ihit=NIncHits-1; ihit>-1; ihit--)
      {
        if (IdInc[ihit]==2112 ){
          ene1stin->Fill(log10(PInc[ihit][4]*1000.));
          nintlar->Fill(-(float)RegInc[ihit]);
				  break;
			  }
		  }

	  } // end if on ncaptures>0

	  // fill photons/electrons
	  for (int ihit=0; ihit<NIneHits; ihit++)
		{
		  if (TypeIne[ihit]==306 && RegIne[ihit]==14) // capture at active LAr
			{

        // count captures on top of each tile
        if (ncaptures>0){
          // tile boundaries in yz from Paola
          // fill drift position
          captureDriftX->Fill(PosIne[ihit][0]);
          // only care captures at least 5cm away from cathode
          if ( PosIne[ihit][0] > -38.5 && PosIne[ihit][0] < -22.5 ){
            if (PosIne[ihit][1] > -75  && PosIne[ihit][1] < -15   && PosIne[ihit][2] > 86.5   && PosIne[ihit][2] < 146.5) {
              capC2counter++;
              petotcapontileC2->Fill(TotPhe[0]*effi);
            }
            if (PosIne[ihit][1] >  90  && PosIne[ihit][1] < 150   && PosIne[ihit][2] > 1      && PosIne[ihit][2] < 61){
              capC3counter++;
              petotcapontileC3->Fill(TotPhe[1]*effi);
            }
            if (PosIne[ihit][1] > -150 && PosIne[ihit][1] < -90   && PosIne[ihit][2] > -61    && PosIne[ihit][2] < -1){
              capC1counter++;
              petotcapontileC1->Fill(TotPhe[2]*effi);
            }
            if (PosIne[ihit][1] > 15   && PosIne[ihit][1] < 75    && PosIne[ihit][2] > -146.5 && PosIne[ihit][2] < -86.5) {
              capC4counter++;
              petotcapontileC4->Fill(TotPhe[3]*effi);
            }
          }
        }

			  for (int ns=FirstSec[ihit]; ns<FirstSec[ihit]+NSecIne[ihit]; ns++)
				{
				  if ( IdSecIne[ns] == 22 ) gamcap->Fill(PSec[ns][4]*1000.);
				}
        //	  ene1stall->Fill(log10(PIne[i1stlarall][4]*1000.),PIne[i1stlarall][4]);



			} // capture at active LAr
		  else if (TypeIne[ihit]==306 && RegIne[ihit]==13)
			{
			  for (int ih=0; ih<NIncHits ; ih++)
				{
				  if ((RegInc[ih]==-4 | RegInc[ihit]==-5)&& IdInc[ih]==2112 )
					{
					  ene1stall->Fill(log10(PInc[ih][4]*1000.));
					  break;
					}

				}
			}
		else if (TypeIne[ihit]>300 && RegIne[ihit]==13) {

		}
		else if (TypeIne[ihit]>300 && RegIne[ihit]==14) {
		}

		else if (TypeIne[ihit]>299 && RegIne[ihit]!=14&& RegIne[ihit]!=13 && RegIne[ihit]!=19 && RegIne[ihit]!=25)
		  {
			for (int ns=FirstSec[ihit]; ns<FirstSec[ihit]+NSecIne[ihit]; ns++)
			  {
				if ( IdSecIne[ns] == 22 ) gamcapo->Fill(PSec[ns][4]*1000.);
			  }
		  }
		else if (IdIne[ihit] == 22 && RegIne[ihit]==14)
		  {
			float ee =0.0;
			int iwhere =0;
			int iwhat=0;
			for (int ns=FirstSec[ihit]; ns<FirstSec[ihit]+NSecIne[ihit]; ns++)
			  {
				if ( abs(IdSecIne[ns]) == 11 )
				  {
					ee+= PSec[ns][4] ;
					iwhere=NeuRegSec[ns];
					iwhat=NeuOldSec[ns];
				  }
			  }
			if (ee > 0.0)
			  {
				// accept captures from all LAr?
				if (iwhere == 14 | iwhere ==13 | iwhere == 25 | iwhere == 19)
				  {
					if (iwhat == 306 ) eelecap->Fill(ee*1000.);
					eelear->Fill(ee*1000.);
				  }
				else {
				  eeleout->Fill(ee*1000.);
				}
			  }
		  }
	  } //end fill photons/electrons

	  // now set threshold
	  //if (TotPhe[2]*effi>thr1) {
    ngoodevts_noconv++;

    for (int ihit=0; ihit<NIneHits; ihit++){
      if(TypeIne[ihit]>299 && RegIne[ihit]==14) {nnvtx=nnvtx+1;} //active Ar
    }

    if (nnvtx ==0){
      for (int isec=0; isec<NTIneSec; isec++){
        //			cout << isec << "num, hit" <<HitSecIne[isec] << endl;
			  //			cout << IdSecIne[isec] << " id, reg " <<RegIne[HitSecIne[isec]] << endl;
        if (abs(IdSecIne[isec])==11 && RegIne[HitSecIne[isec]] ==14 ) {
          regneu->Fill(NeuRegSec[isec]);
          intneu->Fill(NeuOldSec[isec]);
        }
      }
    }

    // get tot phe on each tile
    if (ncaptures==1) {// capture signal time
      petotcapC4->Fill(TotPhe[3]*effi*p2pe);
      petotcapC1->Fill(TotPhe[2]*effi*p2pe);
      petotcapC3->Fill(TotPhe[1]*effi*p2pe);
      petotcapC2->Fill(TotPhe[0]*effi*p2pe);
    } else if ( nnvtx >0 ) {// active Ar inelastic
      petotineC4->Fill(TotPhe[3]*effi*p2pe);
      petotineC1->Fill(TotPhe[2]*effi*p2pe);
      petotineC3->Fill(TotPhe[1]*effi*p2pe);
      petotineC2->Fill(TotPhe[0]*effi*p2pe);
    } else { // bkg outside active LAr
      petotothC4->Fill(TotPhe[3]*effi*p2pe);
      petotothC1->Fill(TotPhe[2]*effi*p2pe);
      petotothC3->Fill(TotPhe[1]*effi*p2pe);
      petotothC2->Fill(TotPhe[0]*effi*p2pe);
    }

		rr2=myran->Rndm();
		rr2=rr2*5;
		rr2=Int_t(rr2)*80.0; //-280.0;
		rr=myran->Rndm();
		offset2=rr*60.0+rr2;
		offset1=offset2-280.0;
		for (int iph=0; iph<NPheHits; iph++) {

      timeh2->Fill(PheTime[iph]/1000.+offset1);
      pehitstot->Fill(Phe[iph]);
      if (PheTile[iph] == 3 && Phe[iph]>0.5) {
        // Most active tile is #2, above 0.5PE theshold
        timeh3->Fill(PheTime[iph]/1000.+offset2);
        // 60 microseconds bunches
        timeh1->Fill(PheTime[iph]/1000.);
        pehitsC4->Fill(Phe[iph]);
      }
      if (PheTile[iph] == 2 && Phe[iph]>0.5) pehitsC1->Fill(Phe[iph]);
      if (PheTile[iph] == 1 && Phe[iph]>0.5) pehitsC3->Fill(Phe[iph]);
      if (PheTile[iph] == 0 && Phe[iph]>0.5) pehitsC2->Fill(Phe[iph]);

			if (ncaptures==1) {// capture signal time
        timhitscap->Fill(PheTime[iph]/1000.+offset1);
        pehitscap->Fill(Phe[iph]);
        if (PheTile[iph] == 3 && Phe[iph]>0.5) pehitscapC4->Fill(Phe[iph]);
        if (PheTile[iph] == 2 && Phe[iph]>0.5) pehitscapC1->Fill(Phe[iph]);
        if (PheTile[iph] == 1 && Phe[iph]>0.5) pehitscapC3->Fill(Phe[iph]);
        if (PheTile[iph] == 0 && Phe[iph]>0.5) pehitscapC2->Fill(Phe[iph]);
			} else if ( nnvtx >0 ) {// active Ar inelastic
        timhitsine->Fill(PheTime[iph]/1000.+offset1);
        pehitsine->Fill(Phe[iph]);
        if (PheTile[iph] == 3 && Phe[iph]>0.5) pehitsineC4->Fill(Phe[iph]);
        if (PheTile[iph] == 2 && Phe[iph]>0.5) pehitsineC1->Fill(Phe[iph]);
        if (PheTile[iph] == 1 && Phe[iph]>0.5) pehitsineC3->Fill(Phe[iph]);
        if (PheTile[iph] == 0 && Phe[iph]>0.5) pehitsineC2->Fill(Phe[iph]);
      } else { // bkg outside active LAr
        timhitsoth->Fill(PheTime[iph]/1000.+offset1);
        pehitsoth->Fill(Phe[iph]);
        if (PheTile[iph] == 3 && Phe[iph]>0.5) pehitsothC4->Fill(Phe[iph]);
        if (PheTile[iph] == 2 && Phe[iph]>0.5) pehitsothC1->Fill(Phe[iph]);
        if (PheTile[iph] == 1 && Phe[iph]>0.5) pehitsothC3->Fill(Phe[iph]);
        if (PheTile[iph] == 0 && Phe[iph]>0.5) pehitsothC2->Fill(Phe[iph]);
			}

      if (EveNum==myevent){ cout << Phe[iph] << " filling " << PheTime[iph]<<" moved to " << PheTime[iph]+offset1*1000.<< " offset1 "<< offset1<<endl;}

      for ( int i=0; i<115;  i++){
			  Float_t newtime=PheTime[iph]+offset1*1000. +i*16.0;
			  newtime=newtime/1000.;
			  Float_t cont =response[i]*Phe[iph]*effi;
			  tcglob->Fill(newtime,cont);
			  tc->Fill(newtime,cont);
        if (EveNum==myevent){ cout << i << " newtime " << newtime <<" content" << cont << endl;}
			}
		} //end loop on phe hits to generate  non-convoluted analisys
		  // and convoluted waveform. now find peaks
		if ( doconv ==1 ){
  		nmaxima=0;
		int dum=tc->GetMaximumBin();
		//		myfile << EveNum <<" event, totph " << TotPhe[2] <<  " all max " << tc->GetBinContent(dum) << " time  " << dum*16.0<<endl;
		if ( tc->GetBinContent(dum) > thr2) {
			ngoodevts++;
			//		  cout << ngoodevts <<" good event " << EveNum<<endl;

		  //			cout << tc->GetBinContent(dum) << " above " << thr2 <<" thresh " <<ngoodevts<<endl;

			if (EveNum== myevent ) {
			  myfile.open("dump.dat");
			  for (int jj=1; jj<75000; jj++){
				myfile<< jj<< " " << tc->GetBinContent(jj) <<endl;}
			  myfile.close();}
			int jmin=1;
			Float_t mymax=0.0;
			Float_t mymax_prev=0.0;
			int k=0;
			Float_t val=0.;
			while (jmin < 74000 ){
			  tc->GetXaxis()->SetRange(jmin,75000);
			  if (EveNum== myevent ) {cout << k <<" k, jmin " << jmin << endl;}
			  k=-1;
			  for (int jj=jmin; jj<75000; jj++){
				val=tc->GetBinContent(jj);
				if (EveNum== myevent ){cout<<jj<< " looking " << val<<endl;}
				if (val > thr2) {
				  k=jj;
				  break;
				}
			  }
			  if (EveNum== myevent ) {cout << k <<" kfound " << val << endl;}
			  if (k > 0) {
				Int_t kmax=k;
				Int_t kprev=kmax;
				mymax=tc->GetBinContent(k);
				while (mymax_prev < mymax && k<74900){
			 // step by 100 channels, find max in each step,
			 // stop when decreasing
				  mymax_prev = mymax;
				  kprev=kmax;
				  tc->GetXaxis()->SetRange(k+1,k+100);
				  kmax= tc->GetMaximumBin();
				  mymax=tc->GetBinContent(kmax);
				  k=k+100;
				  if (EveNum== myevent )	{
					cout << mymax_prev <<" maxima  " << mymax << endl;
					cout << kprev <<" kprev " << k << endl;
					cout << kmax <<" kmax, knew " << k << endl;}
				}
		   // keep this one
			  //			  cout << kprev << " found " <<endl;
				Double_t thistime=kprev*16.E-03;
				nmaxima++;
				if (nmaxima > nnmax) {break;}
			  //			  cout << thistime << " filling " <<nmaxima << " " << ngoodevts<< endl;
				// why zeroes? to be understood"
				if ( thistime > 16E-03)
				  {
					tall->Fill(thistime);
					tcompact->Fill(thistime);
				  }
		   // set to start next window: see when it rises again
				mymax_prev=mymax;

				while (mymax_prev >= mymax && k<74900){
				  mymax_prev=mymax;
				  tc->GetXaxis()->SetRange(k+1,k+100);
				  kmax= tc->GetMaximumBin();
				  mymax=tc->GetBinContent(kmax);
				  k=k+100;
				  if (EveNum== myevent ) {cout << k << " newk found " << mymax<< endl;}
				}
				jmin=k-99;
				if (EveNum== myevent )	{cout << jmin << " jmin found " <<endl;}
			  }
			  else {jmin=75000;}}
		} //end if on at least 1 bin above threshold
			  } // end if on perform convolution
	  //} // end if on totphe

	  if (nmaxima > nnmax ) {cout <<EveNum<<endl; break;}
	  //	  if (ngoodevts >1000) break;
   }//end loop on entries

   //cout << "number of events with signal above thr " << ngoodevts <<endl;
   //cout << "number of events with signal above thr, no convolution " << ngoodevts_noconv <<endl;
   cout << "total events: " << ntotevts <<endl;
   cout << "total cap events: " << ncapevt <<endl;
   cout << "cap events on C1: " << capC1counter <<endl;
   cout << "cap events on C2: " << capC2counter <<endl;
   cout << "cap events on C3: " << capC3counter <<endl;
   cout << "cap events on C4: " << capC4counter <<endl;


    TCanvas *c2 =new TCanvas("c2","c2",1500,1500);
     c2->cd();
     timeh2->Draw();
      c2->Print("vbox_sep24_timeh2.png");
     timeh3->Draw();
      c2->Print("vbox_sep24_timeh3.png");
     tcompact->Draw();
      c2->Print("vbox_sep24_tconv.png");
    TCanvas *c3 =new TCanvas("c3","c3",3000,1500);
     c3->cd();
     int nx=30;
 const char *regnames[30] = {"BLKBODY",
							 "VOID    ",
							 "Cryfoam ",
							 "CrySSinc",
							 "CrySSinh",
							 "CrySSou ",
							 "Airout  ",
							 "ShieldPb",
							 "ShieldPo",
							 "ShieldPoB ",
							 "Shole   ",
							 "LArGas  ",
							 "LArIna  ",
							 "LArAct  ",
							 "Anode1  ",
							 "Anode2  ",
							 "AnEpox  ",
							 "AnSS    ",
							 "AnLAr   ",
							 "PDSfram ",
							 "PDS0    ",
							 "PDS1    ",
							 "PDS2    ",
							 "PDS3    ",
							 "PDSLAr","DDtube","","","","" };
for (int ix=1;ix<=nx;ix++) regneu->GetXaxis()->SetBinLabel(ix,regnames[ix-1]);

 regneu->Draw();
  c3->Print("vbox_sep24_regneu.png");

  timeh2->SetLineWidth(5); // time smear
  timeh2->Draw("PLC");
  timhitsoth->SetLineWidth(5); // non LAr interactions
  timhitsoth->Draw("SAME PLC");
  timhitscap->SetLineWidth(5); // capture signal
  timhitscap->Draw("SAME PLC");
  timhitsine->SetLineWidth(5); // inelastic
  timhitsine->Draw("SAME PLC");
  gPad->BuildLegend();
  c3->Print("vbox_sep24_allXA_times.png"); // time distribution of signals and bkgs

  pehitstot->SetLineWidth(5); // time smear
  pehitstot->Draw("PLC");
  pehitsoth->SetLineWidth(5); // non LAr interactions
  pehitsoth->Draw("SAME PLC");
  pehitscap->SetLineWidth(5); // capture signal
  pehitscap->Draw("SAME PLC");
  pehitsine->SetLineWidth(5); // inelastic
  pehitsine->Draw("SAME PLC");
  gPad->BuildLegend();
  c3->Print("vbox_sep24_allXA_pe.png");

  c3->SetLogy();
  energy_all->GetXaxis()->SetTitle("MeV");
  energy_all->SetLineWidth(5);
  energy_all->Draw("PLC");
  energy_cap1->SetLineWidth(5);
 energy_cap1->Draw("SAME PLC");
  energy_cap2->SetLineWidth(5);
 energy_cap2->Draw("SAME PLC");
gPad->BuildLegend();
  c3->Print("vbox_sep24_energies.png");
  eelecap->Draw("plc");
  eelear->Draw("plc same");
  eeleout->Draw("plc same");
 gPad->BuildLegend();
  c3->Print("vbox_sep24_electrons.png");

   c3->SetLogy(0);
  gamcap->GetXaxis()->SetTitle("MeV");
  gamcap->Draw();
  //gPad->BuildLegend();
  c3->Print("vbox_sep24_gamcap.png");

  gamcapo->GetXaxis()->SetTitle("MeV");
  gamcapo->Draw();
  //gPad->BuildLegend();
  c3->Print("vbox_sep24_gamcapo.png");

   ene1st->SetLineColor(1);
   ene1st->SetLineWidth(5);
   ene1stin->SetLineWidth(5);
   ene1stin->SetLineColor(3);
   ene1stall->SetLineColor(2);
   ene1stall->SetLineWidth(5);
   enelstcryo->SetLineColor(4);
   enelstcryo->SetLineWidth(5);
 ene1st->GetXaxis()->SetTitle("log10(E) (MeV)");
 //   ene1stall->Draw("hist");
    ene1stin->Draw(" hist");
   enelstcryo->Draw("SAME hist");
    ene1st->Draw("SAME hist");
	gPad->BuildLegend(.6,.7,.9,.9 );
  c3->Print("vbox_sep24_ene1st.png");
for (int ix=1;ix<=nx;ix++) nintlar->GetXaxis()->SetBinLabel(ix,regnames[ix-1]);
 nintlar->Draw();
  c3->Print("vbox_sep24_nintlar.png");

  TFile *f = new TFile("vbox_sep24_pe_minimumcut_p2pe1p2.root", "RECREATE");
  TCanvas *c01 =new TCanvas("c01", "c01", 3000, 1500);
  c01->cd();
  petotothC1->SetLineWidth(1); // non LAr interactions
  petotothC1->Draw("hist");
  petotineC1->SetLineWidth(1); // inelastic
  petotineC1->Draw("SAME hist");
  petotcapC1->SetLineWidth(1);
  petotcapC1->Draw("SAME hist");
  petotcapontileC1->SetLineWidth(1); // capture signal
  petotcapontileC1->Draw("SAME hist");
  gPad->BuildLegend();
  c01->Write();
  TCanvas *c02 =new TCanvas("c02","c02",3000,1500);
  c02->cd();
  petotothC2->SetLineWidth(1); // non LAr interactions
  petotothC2->Draw("hist");
  petotineC2->SetLineWidth(1); // inelastic
  petotineC2->Draw("SAME hist");
  petotcapC2->SetLineWidth(1);
  petotcapC2->Draw("SAME hist");
  petotcapontileC2->SetLineWidth(1); // capture signal
  petotcapontileC2->Draw("SAME hist");
  gPad->BuildLegend();
  c02->Write();
  TCanvas *c03 =new TCanvas("c03","c03",3000,1500);
  c03->cd();
  petotothC3->SetLineWidth(1); // non LAr interactions
  petotothC3->Draw("hist");
  petotineC3->SetLineWidth(1); // inelastic
  petotineC3->Draw("SAME hist");
  petotcapC3->SetLineWidth(1);
  petotcapC3->Draw("SAME hist");
  petotcapontileC3->SetLineWidth(1); // capture signal
  petotcapontileC3->Draw("SAME hist");
  gPad->BuildLegend();
  c03->Write();
  TCanvas *c04 =new TCanvas("c04","c04",3000,1500);
  c04->cd();
  petotothC4->SetLineWidth(1); // non LAr interactions
  petotothC4->Draw("hist");
  petotineC4->SetLineWidth(1); // inelastic
  petotineC4->Draw("SAME hist");
  petotcapC4->SetLineWidth(1);
  petotcapC4->Draw("SAME hist");
  petotcapontileC4->SetLineWidth(1); // capture signal
  petotcapontileC4->Draw("SAME hist");
  gPad->BuildLegend();
  c04->Write();
  TCanvas *c05 =new TCanvas("c05","c05",3000,1500);
  c05->cd();
  captureDriftX->SetLineWidth(1);
  captureDriftX->Draw("hist");
  c05->Write();

}
