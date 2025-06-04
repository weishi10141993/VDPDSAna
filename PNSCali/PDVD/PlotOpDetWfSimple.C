R__ADD_INCLUDE_PATH("lardataobj/RawData/OpDetWaveform.h")
R__ADD_INCLUDE_PATH("gallery/Event.h")

//#include "tools.h"

using namespace art;

void PlotOpDetWf(){

  vector<string> filenames;
  filenames.push_back("gen_protodunevd_allradio_g4_stage1_g4PLAr_stage2_detsim.root");

  string optag = "opdigi";
  //InputTag rawop_tag(optag);

  float dt      = 16.*1e-3; // us - tick time step
  const int Nch = 12;        // Number of optical channels
  Color_t colors[Nch] = {kAzure+10, kAzure+1, kBlue, kBlue+2, kMagenta, kMagenta+2, kRed, kRed-7, kOrange, kOrange+7, kGreen+2, kTeal+10};

  vector<TGraph*> graw;

  size_t nrawops, ndecops, nophits;
  double fst_timestamp = -1;
  vector<int> chanmap;
  int countevts = 0;

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
      cout << "raw::OpDetWaveform"<< endl;

      auto const& rawops = *ev.getValidHandle<vector<raw::OpDetWaveform>>(optag);

      nrawops = rawops.size();

      graw.resize(nrawops);
      chanmap.resize(nrawops);

      for (size_t ichan=0;ichan<nrawops; ++ichan){

       	std::vector<short> rawwf = rawops[ichan].Waveform();
       	double timestamp         = rawops[ichan].TimeStamp();
        int    opwf_channel      = rawops[ichan].ChannelNumber();

        if(fst_timestamp < 0) fst_timestamp = timestamp;

        cout << ichan << " " << opwf_channel << " " << rawwf.size()  << " starting at " << timestamp*dt << " us and " << (timestamp-fst_timestamp)*dt << " us from first channel" << endl;

        chanmap[ichan] = opwf_channel;

        graw[ichan] = new TGraph();

        for (size_t j=0;j<rawwf.size(); ++j){
          graw[ichan]->SetPoint(graw[ichan]->GetN(),j*dt,rawwf[j]);
        }

        //break;

        //float t0 = -1;
      }
      cout << "end evt: " <<countevts<< endl;
      countevts ++;
      //break;
  }

  TFile *f = new TFile("RawOpDetWfm_allradionofoam_pDvd_ch16.root", "RECREATE");
  TCanvas* c1 = new TCanvas("c1","PDS raw waverforms map",700,500);
  for (size_t ichan=0;ichan<nrawops; ++ichan){

    if (chanmap[ichan] == 16) {
      c1->cd(ichan+1);
      graw[ichan]->SetName(Form("graw_%zu",ichan));
      graw[ichan]->SetTitle(Form("Optical channel %d",chanmap[ichan]));
      graw[ichan]->SetMarkerColor(colors[ichan%12]); graw[ichan]->SetLineColor(colors[ichan%12]);
      graw[ichan]->Draw("AL");
      graw[ichan]->GetYaxis()->SetRangeUser(400,600);
      gPad->Update();
      c1->Write();
    }

    //break;
  }
}
