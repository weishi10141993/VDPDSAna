{
//check the raw waveform on computer.
  TFile *f=new TFile("raw_wfm.root", "RECREATE");
  ifstream file("/pnfs/dune/persistent/users/weishi/SPEAna/Hamamatsu20ArrayVbd36ColdSimpX3LED/led-pulse-4p6-feb27-2023-hamamatsu-36Vbd-LN2.txt");
  int line=0;
  vector<double> time, pulse, waveform;
  double x, y;
  while (!file.eof() && line<10000){
    file>>x>>y;
    time.push_back(line);
    pulse.push_back(x);
    waveform.push_back(y);
    line++;
  }

  TGraph *trigger=new TGraph(time.size(),&time[0],&pulse[0]);
  trigger->SetLineColor(3);
  trigger->Write();
  trigger->Draw("ALP");
  TGraph *signal=new TGraph(time.size(),&time[0],&waveform[0]);
  signal->SetLineColor(2);
  signal->Write();
  signal->Draw("LP SAME");

}
