{
//check the raw waveform on computer.
  TFile *f=new TFile("raw_wfm.root", "RECREATE");
  ifstream file("/pnfs/dune/persistent/users/weishi/SPEAna/VDDCEMv1p2Differential/default-jan31-48v-led7p5-higherfreq.txt");
  int line=0;
  vector<double> waveform, time;
  double y, x;
  while (!file.eof() && line<2000000){
    file>>y>>x;
    waveform.push_back(y);
    time.push_back(line);
    line++;
  }
  TGraph *gr=new TGraph(time.size(),&time[0],&waveform[0]);
  gr->Write();
  gr->Draw("ALP");
}
