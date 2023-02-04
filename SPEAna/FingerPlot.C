// produce finger plot
//===================Function filter===============================================================
void TV1D_denoise(vector<float>& input, vector<float>& output, const int width, const float lambda){
  if (width>0) {                /*to avoid invalid memory access to input[0]*/
    int k=0, k0=0;            /*k: current sample location, k0: beginning of current segment*/
    float umin=lambda, umax=-lambda;    /*u is the dual variable*/
    float vmin=input[0]-lambda, vmax=input[0]+lambda;    /*bounds for the segment's value*/
    int kplus=0, kminus=0;     /*last positions where umax=-lambda, umin=lambda, respectively*/
    const float twolambda=2.0*lambda;    /*auxiliary variable*/
    const float minlambda=-lambda;        /*auxiliary variable*/
    for (;;) {                /*simple loop, the exit test is inside*/
      while (k==width-1) {    /*we use the right boundary condition*/
	if (umin<0.0) {            /*vmin is too high -> negative jump necessary*/
	  do output[k0++]=vmin; while (k0<=kminus);
	  umax=(vmin=input[kminus=k=k0])+(umin=lambda)-vmax;
	} else if (umax>0.0) {    /*vmax is too low -> positive jump necessary*/
	  do output[k0++]=vmax; while (k0<=kplus);
	  umin=(vmax=input[kplus=k=k0])+(umax=minlambda)-vmin;
	} else {
	  vmin+=umin/(k-k0+1);
	  do output[k0++]=vmin; while(k0<=k);
	  return;
	}
      }
      if ((umin+=input[k+1]-vmin)<minlambda) {        /*negative jump necessary*/
	do output[k0++]=vmin; while (k0<=kminus);
	vmax=(vmin=input[kplus=kminus=k=k0])+twolambda;
	umin=lambda; umax=minlambda;
      } else if ((umax+=input[k+1]-vmax)>lambda) {    /*positive jump necessary*/
	do output[k0++]=vmax; while (k0<=kplus);
	vmin=(vmax=input[kplus=kminus=k=k0])-twolambda;
	umin=lambda; umax=minlambda;
      } else {     /*no jump necessary, we continue*/
	k++;
	if (umin>=lambda) {        /*update of vmin*/
	  vmin+=(umin-lambda)/((kminus=k)-k0+1);
	  umin=lambda;
	}
	if (umax<=minlambda) {    /*update of vmax*/
	  vmax+=(umax+lambda)/((kplus=k)-k0+1);
	  umax=minlambda;
	}
      }
    }
  }
}
//==================================================================================================


// Before was used for simpx3 analysis

void FingerPlot(){
  // TFile *f=new TFile("Simp3_plots_36VLED6p8.root","RECREATE");
  // TFile *f=new TFile("Simp3_plots_36VLED6p8_july8.root","RECREATE");
  // TFile *f=new TFile("july8_led6p8_ch2_36V_1.root","RECREATE");
  // TFile *f=new TFile("test_save_led10p0-35v.root","RECREATE");
  TFile *f=new TFile("Fingerplot.root","RECREATE");

  TH1D *Total_charge=new TH1D("Total_charge","Integrated charge;Charge [V*ns];entries",10000,-2.0,40.0);
  TH2D *time_vs_signal=new TH2D("time_vs_signal",";time [ticks];signal[V]",1000,0,1000,5000,-0.5,0.5);
  TH1D *amplitude =new TH1D("amplitude","",500,-0.5,0.5);


  std::vector<float> ondama(1000);
  std::vector<float> ondaden(1000);
  std::vector<float> input(1000);
  std::vector<float> output(1000);
   Float_t mobileAVG,avg,c0,c1,c2;
   double mu,sigma,sigma2,sv1,sv2,base,baseCC,mediaPre,sum,sum2,base2;
   int index;
   int inum=0;
   double ampl=0;//amplitude



  TGraph *gr[500];
  TGraph *gr_denoised[500];
  double times[1000]={0.0};
  double waveform[1000]={0.0};
  int entry=0;
  std::vector<float> timev(1000);
  for(int i=0;i<1000;i++){
    times[i]=i;
    timev[i]=i;
  }

  //  ifstream file("test_save_led6p8.txt");
  // ifstream file("july8_led6p8_ch3_36V.txt");
  // ifstream file("july8_led6p8_ch2_36V_1.txt");
  //ifstream file("test_save_led10p0-35v.txt");
  ifstream file("/pnfs/dune/persistent/users/weishi/SPEAna/VDDCEMv1p2Differential/default-jan31-48v-led7p5-higherfreq.txt");
  int line=0;
  vector<double> waveformy, time;
  double y, x, sizev;
  int entries=0;
  while (!file.eof()){
    file>>y>>x;
    line++;
    waveformy.push_back(y);
    if(line<500) continue;
    if(x>1.0){
      sizev=waveformy.size();
      for(int i=0;i<500;i++){
	waveform[499-i]=waveformy[sizev-1-i];
      }
      for(int i=500;i<1000;i++){
	file>>y>>x;
	waveform[i]=y;
      }
      waveformy.clear();
      entry++;


      /////////////////Charge analysis///////////////////

      //////////////////////////////////////////////////
      //========= Denoising ============================================================
      mobileAVG=4.0;  avg=0.;  c0=0.;  c1=0.;  c2=0.;
      for(Int_t n=0; n<1000 ; n++){if(n>=mobileAVG && n<1000-mobileAVG){
	  for(Int_t i1=n-mobileAVG; i1<=n+mobileAVG; i1++){avg=avg+waveform[i1]; c0=c0+1;}
	  avg=avg/c0; ondama[n]=avg; avg=0; c0=0;}
	else{if(n<mobileAVG){
	    for(Int_t i1=0; i1<=n+mobileAVG; i1++){avg=avg+waveform[i1]; c1=c1+1;}
	    avg=avg/c1; ondama[n]=avg; avg=0; c1=0;}
	  else if(n>=1000-mobileAVG){
	    for(Int_t i1=n-mobileAVG; i1<1000; i1++){avg=avg+waveform[i1]; c2=c2+1;}
	    avg=avg/c2; ondama[n]=avg; avg=0; c2=0;}}}

      for(Int_t i1=0; i1<1000; i1++){input[i1]=ondama[i1]; output[i1]=input[i1];}

      TV1D_denoise(input,output,1000,0.00);
      for(Int_t i1=0; i1<1000; i1++){
	ondaden[i1]=output[i1];
      }

      //===========================BASELINE Histo========================================

      //===========================BASELINE Histo========================================
      base=0;
      base2=0;

      TH1F *basehelp= new TH1F("basehelp","basehelp",500,-0.01,0.01);//channel 4
      for(int ib=0; ib<800; ib++){basehelp->Fill(ondaden[ib]);}
      /* basebinmax = basehelp->GetMaximumBin();
	 base = basehelp->GetXaxis()->GetBinCenter(basebinmax);*/
      base=basehelp->GetMean();//try using mean for baseline

      basehelp->Delete();
      for(int i=0;i<1000;i++){
	waveform[i]=waveform[i]-base;
	time_vs_signal->Fill(i,waveform[i]);
      }

       for(int i=0; i<1000; i++){
	ondaden[i]=ondaden[i]-base;
      }

      if(entry<500){
	gr[entry]=new TGraph(1000,times,waveform);
	gr[entry]->Write(Form("gr_raw_%d",entry));
	gr_denoised[entry]=new TGraph(timev.size(),&timev[0],&ondaden[0]);
	gr_denoised[entry]->Write(Form("gr_denoised_%d",entry));
      }



      //=================== Integral ====================================================
      sum=0.; sum2=-1000.; sigma2=-10;ampl=-100;index=0;
      for(int i=0;i<1000;i++){
	if(i>520 && i<560){//noise
	  sum+=ondaden[i];
	  if(ondaden[i]>ampl) ampl=ondaden[i];
	}

      }
      if(entry<500){
	if(sum>0.35 && sum<0.45) std::cout<<"SPE "<<entry<<std::endl;
	if(sum>0.65 && sum<0.75) std::cout<<"2SPE "<<entry<<std::endl;
      }
      //=================================================================================
      //=================================================================================




      ////////////////////////////////////////////////
      ///////////////////////////////////////////////






      Total_charge->Fill(sum);
      amplitude->Fill(ampl);
    }
  }
  Total_charge->Write();
  amplitude->Write();
  time_vs_signal->Write();
  Total_charge->Draw();
  f->Close();


}
