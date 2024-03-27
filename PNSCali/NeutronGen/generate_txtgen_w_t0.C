#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLeaf.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TH2.h>
#include <TPad.h>
#include <TVector3.h>
#include <TString.h>
#include <TPRegexp.h>
#include <TGraph.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TSystem.h>
#include <TList.h>
#include <TGraph.h>
#include <TStopwatch.h>

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

//------------------------------------------------------------------------------
// Main method
void generate_txtgen_w_t0(UInt_t ndecays, UInt_t nevents, string fileout) {
  // Prevent canvases from being drawn.
  //   gROOT->SetBatch(kTRUE);
  gROOT->Reset();

  TStopwatch* clock = new TStopwatch();
  clock->Start();

  // Random and Vertex parameters
  TRandom3* rdef = new TRandom3();//Random number generator
  rdef->SetSeed(0);

  // Open root file containing position distributions
  TFile f("coldbox_side_dist.root");

  //Extract necessary histograms from the file
  TH1F *r = (TH1F*)f.Get("r");
  TH1F *x = (TH1F*)f.Get("X");
  TH1F *y = (TH1F*)f.Get("Y");
  TH1F *z = (TH1F*)f.Get("Z");

  TH3F *dist = (TH3F*)f.Get("dist");

  //time histogram for verification
  TH1D *time = new TH1D("t_HEPEvt", "time", 115, 0, 5*1000000);


  // Choose an isotope (only Ar37, Ar41, 2H, 17O, 13C for now)
  TString isotope = "Ar41";

  if (isotope != "Ar41" && isotope != "Ar37" && isotope != "H2" && isotope != "C13" && isotope != "O17") {
    cout << "Isotope isn't supported******. Exiting!\n";
    return;
  }

  // HEPEvt file
  std::ofstream output_hepevt_g4(isotope + "_decays_HEPEvt_G4.dat");
  std::ofstream output_hepevt_larsoft(fileout + ".dat");
  std::ofstream output_dance(isotope + "_decays_DANCEinput.dat");


  // Histos showing all gammas and their probab (cross check)
  TH1D *Gamma_spectrum = new TH1D(isotope + "_Gamma_spectrum",isotope + "_Gamma_spectrum", 1000, 0, 10000);

  // Output file for histo
  TFile *g_out = new TFile(isotope + "_Gamma_spectrum.root","RECREATE");

  // Position in the simulation
  TVector3 Xg, Pg;
  Xg.SetXYZ(0.,0.,0.); Pg.SetXYZ(0.,0.,0.);


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////-----------------  Argon 41 decay part -----------------------////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (isotope == "Ar41") {
    // List of levels    0        1         2       3       4        5    6      7         8     9        10     11     12
    double Levels[13] = {6098.9, 4270.0, 3968.2, 3326.8, 3009.6, 2948.7, 2733.4, 2398.1, 1353.9, 1034.7, 516.1, 167.3, 0.0};
    double level = 0.0;


    std::vector<double> v_gammas; v_gammas.clear(); // gammas for this event
    Int_t nb_gammas = 0; // number of gammas for this decay
    Double_t probability = 0.; // the probability we'll use in the loop
    Double_t Etot = 0.;
    Int_t tot_gammas =0; //number of gammas for the entire event
    Double_t probMyGamma = 51.2;

    Int_t nb1 = 0, nb2 = 0, nb3 = 0, nb4 = 0, nb5 = 0, nb6 = 0, nb7 = 0, nb8 = 0, nb9 = 0, nb10 = 0, nb11 = 0, nb12 = 0;
    Int_t nb13 = 0, nb14 = 0, nb15 = 0, nb16 = 0, nb17 = 0, nb18 = 0, nb19 = 0, nb20 = 0, nb21 = 0;
    Int_t nb22 = 0, nb23 = 0, nb24 = 0, nb25 = 0, nb26 = 0, nb27 = 0, nb28 = 0, nb29 = 0;
    // 1 is 4.7+1.18+0.167
    // 2 is 5582+516
    // 3 is 4.7+0.8+0.5
    // 4 is 3.7+1+1.18+0.167
    // 5 is 2771.8 1972.6 837.7 348.7 167.3
    // 6 is 3.7MeV, 1.04MeV, 837keV, 516keV
    // 7 is 3.7MeV, 1.88MeV, 516keV
    // 8 is 3.7MeV, 2.23MeV, 167keV
    // 9 is 2.77MeV, 1.97MeV, 1.18MeV, 167keV
    // 10 is 2.77MeV, 1.97MeV, 837keV, 516keV
    // 11 is 2.77MeV, 2.29MeV, 867keV, 167keV
    // 12 is 2.77MeV, 2.81MeV, 516keV
    // 13 is 5.063MeV, 867keV, 167keV
    // 14 is 3.36MeV,  2.56MeV, 167keV
    // 15 is 3.15MeV, 2.78MeV, 167keV
    // 16 is 3.15MeV, 2.43MeV, 516keV
    // 17 is 3.089MeV, 2.8MeV, 167keV
    // 18 is 2.1MeV, 2.6MeV, 1.18MeV, 167keV
    // 19 is 2.1MeV, 2.6MeV, 837keV, 516keV
    // 20 is 2.1MeV, 3.45MeV, 516keV
    // 21 is 1.83MeV, 4.1MeV, 163MeV
    // 22 is 5.58MeV, 348keV, 167keV
    // 23 is 4.7MeV, 837keV, 348keV, 167keV
    // 24 is 3.7MeV, 1.04MeV, 837keV, 348keV, 167keV
    // 25 is 3.7MeV, 1.88MeV, 348keV, 167keV
    // 26 is 2.77MeV, 2.81MeV, 348keV, 167keV
    // 27 is 3.15MeV, 2.43MeV, 348keV, 167keV
    // 28 is 2.1MeV, 2.6MeV, 837keV, 348keV, 167keV
    // 29 is 2.1MeV, 3.45MeV, 348keV, 167keV

    for (UInt_t e = 0; e <nevents; e++){
      // Initialization
      tot_gammas = 0;
      std::ofstream output_temp; //temporary file to hole the gammas, will be coppied into the output file at the end
      output_temp.open("temp.txt", std::ofstream::out | std::ofstream::trunc); //open and delete contents

      for (UInt_t n = 0; n < ndecays; n++) {

	// Initialization
	v_gammas.clear();
	nb_gammas = 0;



	level = Levels[0];
	while (level != Levels[12]) {

	  if (level == Levels[0]) {
	    probability = rdef->Uniform(10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00+4.09+0.93);
	    if (probability <= 10.79){
	      level = Levels[10];
	      v_gammas.push_back(5582.0); nb_gammas++;
	      continue;
	    } else if (probability > 10.79 && probability <= 10.79+0.242) {
	      level = Levels[9];
	      v_gammas.push_back(5063.7); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242 && probability <= 10.79+0.242+probMyGamma) {
	      level = Levels[8];
	      v_gammas.push_back(4745.0); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma && probability <= 10.79+0.242+probMyGamma+9.11) {
	      level = Levels[7];
	      v_gammas.push_back(3700.4); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11 && probability <= 10.79+0.242+probMyGamma+9.11+3.91) {
	      level = Levels[6];
	      v_gammas.push_back(3365.5); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11+3.91 && probability <= 10.79+0.242+probMyGamma+9.11+3.91+3.72) {
	      level = Levels[5];
	      v_gammas.push_back(3150.2); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11+3.91+3.72 && probability <= 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02) {
	      level = Levels[4];
	      v_gammas.push_back(3089.4); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02 && probability <= 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00) {
	      level = Levels[3];
	      v_gammas.push_back(2771.8); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00 && probability <= 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00+4.09) {
	      level = Levels[2];
	      v_gammas.push_back(2130.7); nb_gammas++;
	      continue;
	    } else if (probability > 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00+4.09 && probability <= 10.79+0.242+probMyGamma+9.11+3.91+3.72+1.02+8.00+4.09+0.93) {
	      level = Levels[1];
	      v_gammas.push_back(1828.8); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[1]) {
	    probability = rdef->Uniform(0.93);
	    if (probability <= 0.93){
	      level = Levels[11];
	      v_gammas.push_back(4102.5); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[2]) {
	    probability = rdef->Uniform(1.86+2.7);
	    if (probability <= 1.86){
	      level = Levels[10];
	      v_gammas.push_back(3451.8); nb_gammas++;
	      continue;
	    } else if (probability > 1.86 && probability <= 1.86+2.7) {
	      level = Levels[8];
	      v_gammas.push_back(2614.3); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[3]) {
	    probability = rdef->Uniform(5.49+0.186+0.502);
	    if (probability <= 5.49){
	      level = Levels[10];
	      v_gammas.push_back(2810.5); nb_gammas++;
	      continue;
	    } else if (probability > 5.49 && probability <= 5.49+0.186) {
	      level = Levels[9];
	      v_gammas.push_back(2291.6); nb_gammas++;
	      continue;
	    } else if (probability >  5.49+0.186 && probability <=  5.49+0.186+ 0.502) {
	      level = Levels[8];
	      v_gammas.push_back(1972.6); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[4]) {
	    probability = rdef->Uniform(0.818);
	    if (probability <= 0.818) {
	      level = Levels[11];
	      v_gammas.push_back(2842.5); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[5]) {
	    probability = rdef->Uniform(1.58+0.781);
	    if (probability <= 1.58){
	      level = Levels[11];
	      v_gammas.push_back(2781.8); nb_gammas++;
	      continue;
	    } else if (probability > 1.58 && probability <= 1.58+0.781) {
	      level = Levels[10];
	      v_gammas.push_back(2432.5); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[6]) {
	    probability = rdef->Uniform(2.6);
	    if (probability <= 2.6) {
	      level = Levels[11];
	      v_gammas.push_back(2566.1); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[7]) {
	    probability = rdef->Uniform(0.27+1.3+5.58);
	    if (probability <= 0.27){
	      level = Levels[11];
	      v_gammas.push_back(2229.5); nb_gammas++;
	      continue;
	    } else if (probability > 0.27 && probability <= 0.27+1.3) {
	      level = Levels[10];
	      v_gammas.push_back(1881.5); nb_gammas++;
	      continue;
	    } else if (probability >  0.27+1.3 && probability <=  0.27+1.3+5.58) {
	      level = Levels[8];
	      v_gammas.push_back(1044.3); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[8]) {
	    probability = rdef->Uniform(2.14+48.5+8.93);
	    if (probability <= 2.14){
	      level = Levels[12];
	      v_gammas.push_back(1354.0); nb_gammas++;
	      continue;
	    } else if (probability > 2.14 && probability <= 2.14+48.5) {
	      level = Levels[11];
	      v_gammas.push_back(1186.8); nb_gammas++;
	      continue;
	    } else if (probability >  2.14+48.5 && probability <=  2.14+48.5+8.93) {
	      level = Levels[10];
	      v_gammas.push_back(837.7); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[9]) {
	    probability = rdef->Uniform(1.02);
	    if (probability <= 1.02) {
	      level = Levels[11];
	      v_gammas.push_back(867.3); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[10]) {
	    probability = rdef->Uniform(23.5+6.14);
	    if (probability <= 23.5){
	      level = Levels[12];
	      v_gammas.push_back(516.0); nb_gammas++;
	      continue;
	    } else if (probability > 23.5 && probability <= 23.5+6.14) {
	      level = Levels[11];
	      v_gammas.push_back(348.7); nb_gammas++;
	      continue;
	    }
	  }

	  if (level == Levels[11]) {
	    probability = rdef->Uniform(74.0);
	    if (probability <= 74.0) {
	      level = Levels[12];
	      v_gammas.push_back(167.3); nb_gammas++;
	      continue;
	    }
	  }

	}

	//       v_gammas.empty();
	//       v_gammas.push_back(4745.0);v_gammas.push_back(1186.8);v_gammas.push_back(167.3); nb_gammas = 3;

	// Check if sum of gammas is 6.1
	for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	  Etot += v_gammas.at(i);
	}
	//     cout<< Etot << endl;
	if (Etot < 6000 || Etot > 6200) {
	  cout << "Summed gamma energy of " << Etot << " is not around 6.1 +/- 0.1 MeV. Something must be wrong************\n";
	  Etot = 0.; continue;
	}
	Etot = 0.; // re-initialize Etot


	// Print and fill histograms
	//     cout << nb_gammas << endl;
	for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	  //       cout << v_gammas.at(i) << endl;
	  Gamma_spectrum->Fill(v_gammas.at(i));
	}
	//     cout << endl;

	// Generates position in protoDUNE from the histogram distributions
	Xg.SetXYZ(x->GetRandom(), y->GetRandom(), z->GetRandom());
	//Xg.SetXYZ(-100, 400, 100);

	//calculate cylindrical radial distance
	Double_t r = sqrt(pow(Xg[0],2) + pow(Xg[2]-146,2));

	//Get a random time for the specific r and y

	auto rAxis = dist->GetXaxis();
	auto yAxis = dist->GetYaxis();

	auto xbin = rAxis->FindBin(r);
	auto ybin = yAxis->FindBin(Xg[1]);

	rAxis->SetRange(xbin-1,xbin+1);
	yAxis->SetRange(ybin-1,ybin+1);

	auto timeDist = dist->Project3D("z");


	Double_t t = timeDist->GetRandom();


	time->Fill(t);

	//cout << time->GetEntries() << endl;



	// Now the part where the HEPEvt and DANCE input files are written
	output_dance << nb_gammas << endl;
	output_hepevt_g4 << nb_gammas << endl;

	for (Int_t i=0; i < nb_gammas; i++) {
	  // HEPEvt part
	  // generate energy, momentum and position
	  gRandom->Sphere(Pg[0],Pg[1],Pg[2], v_gammas.at(i)*1e-6); //random momentum over a sphere (in GeV)
	  // write in HEPEvt file
	  output_temp           << "1 22 0 0 0 0 "
				<< Pg[0] << " " << Pg[1] << " " << Pg[2] << " " << v_gammas.at(i)*1e-6 << " 0 "
				<< Xg[0] << " " << Xg[1] << " " << Xg[2] << " " << t << " "<< endl;

	  // output_hepevt_g4 << "1 22 0 0 "
	  //			   << Pg[0] << " " << Pg[1] << " " << Pg[2] << " 0\n";

	  // DANCE input part
	  //output_dance << v_gammas.at(i)*1e-3 << " "; // in MeV
	}
	output_dance << endl;


	if (v_gammas.size() == 3) {
	  if (v_gammas.at(0) == 4745.0 && v_gammas.at(1) == 1186.8 && v_gammas.at(2) == 167.3) {
	    nb1++;
	  }
	  if (v_gammas.at(0) == 4745.0 && v_gammas.at(1) == 837.7 && v_gammas.at(2) == 516.0) {
	    nb3++;
	  }
    if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 1881.5 && v_gammas.at(2) == 516.0) {
	    nb7++;
	  }
    if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 2229.5 && v_gammas.at(2) == 167.3) {
	    nb8++;
	  }
    if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 2810.5 && v_gammas.at(2) == 516.0) {
	    nb12++;
	  }
    if (v_gammas.at(0) == 5063.7 && v_gammas.at(1) == 867.3 && v_gammas.at(2) == 167.3) {
	    nb13++;
	  }
    if (v_gammas.at(0) == 3365.5 && v_gammas.at(1) == 2566.1 && v_gammas.at(2) == 167.3) {
	    nb14++;
	  }
    if (v_gammas.at(0) == 3150.2 && v_gammas.at(1) == 2781.8 && v_gammas.at(2) == 167.3) {
	    nb15++;
	  }
    if (v_gammas.at(0) == 3150.2 && v_gammas.at(1) == 2432.5 && v_gammas.at(2) == 516.0) {
	    nb16++;
	  }
    if (v_gammas.at(0) == 3089.4 && v_gammas.at(1) == 2842.5 && v_gammas.at(2) == 167.3) {
	    nb17++;
	  }
    if (v_gammas.at(0) == 2130.7 && v_gammas.at(1) == 3451.8 && v_gammas.at(2) == 516.0 ) {
	    nb20++;
	  }
    if (v_gammas.at(0) == 1828.8 && v_gammas.at(1) == 4102.5 && v_gammas.at(2) == 167.3 ) {
	    nb21++;
	  }
    if (v_gammas.at(0) == 5582.0 && v_gammas.at(1) == 348.7 && v_gammas.at(2) == 167.3 ) {
	    nb22++;
	  }
	}
	if (v_gammas.size() == 4) {
	  if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 1044.3 && v_gammas.at(2) == 1186.8 && v_gammas.at(3) == 167.3) {
	    nb4++;
	  }
    if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 1044.3 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 516.0) {
	    nb6++;
	  }
    if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 1972.6 && v_gammas.at(2) == 1186.8 && v_gammas.at(3) == 167.3) {
	    nb9++;
	  }
    if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 1972.6 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 516.0) {
	    nb10++;
	  }
    if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 2291.6 && v_gammas.at(2) == 867.3 && v_gammas.at(3) == 167.3) {
	    nb11++;
	  }
    if (v_gammas.at(0) == 2130.7 && v_gammas.at(1) == 2614.3 && v_gammas.at(2) == 1186.8 && v_gammas.at(3) == 167.3) {
	    nb18++;
	  }
    if (v_gammas.at(0) == 2130.7 && v_gammas.at(1) == 2614.3 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 516.0) {
	    nb19++;
	  }
    if (v_gammas.at(0) == 4745.0 && v_gammas.at(1) == 837.7 && v_gammas.at(2) == 348.7 && v_gammas.at(3) == 167.3) {
	    nb23++;
	  }
    if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 1881.5 && v_gammas.at(2) == 348.7 && v_gammas.at(3) == 167.3) {
	    nb25++;
	  }
    if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 2810.5 && v_gammas.at(2) == 348.7 && v_gammas.at(3) == 167.3) {
	    nb26++;
	  }
    if (v_gammas.at(0) == 3150.2 && v_gammas.at(1) == 2432.5 && v_gammas.at(2) == 348.7 && v_gammas.at(3) == 167.3) {
	    nb27++;
	  }
    if (v_gammas.at(0) == 2130.7 && v_gammas.at(1) == 3451.8 && v_gammas.at(2) == 348.7 && v_gammas.at(3) == 167.3) {
	    nb29++;
	  }
	}
	if (v_gammas.size() == 2) {
	  if (v_gammas.at(0) == 5582.0 && v_gammas.at(1) == 516.0) {
	    nb2++;
	  }
	}
	if (v_gammas.size() == 5) {
	  if (v_gammas.at(0) == 2771.8 && v_gammas.at(1) == 1972.6 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 348.7 && v_gammas.at(4) == 167.3) {
	    nb5++;
	  }
    if (v_gammas.at(0) == 3700.4 && v_gammas.at(1) == 1044.3 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 348.7 && v_gammas.at(4) == 167.3) {
	    nb24++;
	  }
    if (v_gammas.at(0) == 2130.7 && v_gammas.at(1) == 2614.3 && v_gammas.at(2) == 837.7 && v_gammas.at(3) == 348.7 && v_gammas.at(4) == 167.3) {
	    nb28++;
	  }
	}

	tot_gammas += nb_gammas;

      } //end of nb decays loop


      output_temp.close();
      std::ifstream infile("temp.txt"); //open stream to read temp file
      output_hepevt_larsoft << e << " " << tot_gammas << "\n"; //write the first line for each event to the output file
      string line;
      while(std::getline(infile, line)){
	//copy line from the temp file to the output file
	output_hepevt_larsoft << line << "\n";
      }


    } //end of nb events loop


    cout << "nb1: " << nb1 << ", nb2: " << nb2 << ", nb3: " << nb3 << ", nb4: " << nb4 << ", nb5: " << nb5 << ", nb6: " << nb6 << ", nb7: " << nb7 << ", nb8: " << nb8 << ", nb9: " << nb9 << ", nb10: " << nb10 << ", nb11: " << nb11 << ", nb12: " << nb12 << "\n";
    cout << "nb13: " << nb13 << ", nb14: " << nb14 << ", nb15: " << nb15 << ", nb16: " << nb16 << ", nb17: " << nb17 << ", nb18: " << nb18 << ", nb19: " << nb19 << ", nb20: " << nb20 << ", nb21: " << nb21 << "\n";
    cout << "nb22: " << nb22 << ", nb23: " << nb23 << ", nb24: " << nb24 << ", nb25: " << nb25 << ", nb26: " << nb26 << ", nb27: " << nb27 << ", nb28: " << nb28 << ", nb29: " << nb29 << "\n";
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////-----------------  End of Argon 41 decay part -----------------------////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////-----------------  Argon 37 decay part -----------------------////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (isotope == "Ar37") {
    // List of levels    0        1         2       3       4        5    6      7         8     9        10     11       12     13
    double Levels[14] = {8791.2, 6826.2, 6583.7, 5090.5, 4637.6, 4578.7, 4448.6, 3981.1, 3938.5, 3518.0, 2490.9, 1611.9, 1410.6, 0.0};
    double level = 0.0;


    std::vector<double> v_gammas; v_gammas.clear(); // gammas for this event
    Int_t nb_gammas = 0; // number of gammas for this decay
    Double_t probability = 0.; // the probability we'll use in the loop
    Double_t Etot = 0.;

    // Fills the macro file with ndecays
    for (UInt_t n = 0; n < ndecays; n++) {

      // Initialization
      v_gammas.clear();
      nb_gammas = 0;


      level = Levels[0];
      while (level != Levels[13]) {

	if (level == Levels[0]) {
	  probability = rdef->Uniform(10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4+2.7+2.4);
	  if (probability <= 10.9){
	    level = Levels[13];
	    v_gammas.push_back(8790.4); nb_gammas++;
	    continue;
	  } else if (probability > 10.9 && probability <= 10.9+37.5) {
	    level = Levels[10];
	    v_gammas.push_back(6299.7); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5 && probability <= 10.9+37.5+25.0) {
	    level = Levels[9];
	    v_gammas.push_back(5272.6); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0 && probability <= 10.9+37.5+25.0+0.8) {
	    level = Levels[8];
	    v_gammas.push_back(4851.8); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8 && probability <= 10.9+37.5+25.0+0.8+1.6) {
	    level = Levels[7];
	    v_gammas.push_back(4810.3); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2) {
	    level = Levels[6];
	    v_gammas.push_back(4342.3); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6+3.2 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2+0.8) {
	    level = Levels[5];
	    v_gammas.push_back(4211.6); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6+3.2+0.8 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8) {
	    level = Levels[4];
	    v_gammas.push_back(4153.0); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4) {
	    level = Levels[3];
	    v_gammas.push_back(3700.2); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4+2.7) {
	    level = Levels[2];
	    v_gammas.push_back(2207.6); nb_gammas++;
	    continue;
	  } else if (probability > 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4+2.7 && probability <= 10.9+37.5+25.0+0.8+1.6+3.2+0.8+1.8+13.4+2.7+2.4) {
	    level = Levels[1];
	    v_gammas.push_back(1966.7); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[1]) {
	  probability = rdef->Uniform(5.0); //it's ACTUALLY < 5.0
	  if (probability <= 5.0){
	    level = Levels[5];
	    v_gammas.push_back(2247.9); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[2]) {
	  probability = rdef->Uniform(0.2);
	  if (probability <= 0.2){
	    level = Levels[6];
	    v_gammas.push_back(2135.3); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[3]) {
	  probability = rdef->Uniform(7.9+3.1);
	  if (probability <= 7.9){
	    level = Levels[12];
	    v_gammas.push_back(3679.3); nb_gammas++;
	    continue;
	  } else if (probability > 7.9 && probability <= 7.9+3.1) {
	    level = Levels[10];
	    v_gammas.push_back(2599.6); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[4]) {
	  probability = rdef->Uniform(1.0+0.2);
	  if (probability <= 1.0){
	    level = Levels[12];
	    v_gammas.push_back(3226.9); nb_gammas++;
	    continue;
	  } else if (probability > 1.0 && probability <= 1.0+0.2) {
	    level = Levels[10];
	    v_gammas.push_back(2145.2); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[5]) {
	  probability = rdef->Uniform(0.3);
	  if (probability <= 0.3) {
	    level = Levels[10];
	    v_gammas.push_back(2087.3); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[6]) {
	  probability = rdef->Uniform(2.0);
	  if (probability <= 2.0) {
	    level = Levels[10];
	    v_gammas.push_back(1957.3); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[7]) {
	  probability = rdef->Uniform(1.3);
	  if (probability <= 1.3) {
	    level = Levels[13];
	    v_gammas.push_back(3981.4); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[8]) {
	  probability = rdef->Uniform(1.0);
	  if (probability <= 1.0) {
	    level = Levels[13];
	    v_gammas.push_back(3938.0); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[9]) {
	  probability = rdef->Uniform(23.7+2.7);
	  if (probability <= 23.7){
	    level = Levels[12];
	    v_gammas.push_back(2107.5); nb_gammas++;
	    continue;
	  } else if (probability > 23.7 && probability <= 23.7+2.7) {
	    level = Levels[10];
	    v_gammas.push_back(1026.7); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[10]) {
	  probability = rdef->Uniform(57.0+0.5);
	  if (probability <= 57.0){
	    level = Levels[13];
	    v_gammas.push_back(2490.6); nb_gammas++;
	    continue;
	  } else if (probability > 57.0 && probability <= 57.0+0.5) {
	    level = Levels[11];
	    v_gammas.push_back(878.5); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[11]) {
	  probability = rdef->Uniform(3.4);
	  if (probability <= 3.4) {
	    level = Levels[13];
	    v_gammas.push_back(1611.7); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[12]) {
	  probability = rdef->Uniform(33.0);
	  if (probability <= 33.0) {
	    level = Levels[13];
	    v_gammas.push_back(1410.3); nb_gammas++;
	    continue;
	  }
	}
      }

      // Check if sum of gammas is 6.1
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	Etot += v_gammas.at(i);
      }
      //     cout<< Etot << endl;
      if (Etot < 8700 || Etot > 8900) {
	cout << "Summed gamma energy of " << Etot << " is not around 8.8 +/- 0.1 MeV. Something must be wrong************\n";
	Etot = 0.; continue;
      }
      Etot = 0.; // re-initialize Etot


      // Print and fill histograms
      //     cout << nb_gammas << endl;
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	//       cout << v_gammas.at(i) << endl;
	Gamma_spectrum->Fill(v_gammas.at(i));
      }
      //     cout << endl;

      // Generates position in DUNE (for each event, not each gamma)
      //Xg.SetXYZ(rdef->Uniform(-4000.,4000),rdef->Uniform(0.,6000),rdef->Uniform(0.,7000));
      Xg.SetXYZ(-100, 400, 100);


      // Now the part where the HEPEvt and DANCE input files are written
      output_dance << nb_gammas << endl;
      output_hepevt_larsoft << n << " " << nb_gammas << endl;
      output_hepevt_g4 << nb_gammas << endl;

      for (Int_t i=0; i < nb_gammas; i++) {
	// HEPEvt part
	// generate energy, momentum and position
	gRandom->Sphere(Pg[0],Pg[1],Pg[2], v_gammas.at(i)*1e-6); //random momentum over a sphere (in GeV)
	// write in HEPEvt file
	output_hepevt_larsoft << "1 22 0 0 0 0 "
			      << Pg[0] << " " << Pg[1] << " " << Pg[2] << " " << v_gammas.at(i)*1e-6 << " 0 "
			      << Xg[0] << " " << Xg[1] << " " << Xg[2] << " 0 0 0 " << endl;

	output_hepevt_g4 << "1 22 0 0 "
			 << Pg[0] << " " << Pg[1] << " " << Pg[2] << " 0\n";

	// DANCE input part
	output_dance << v_gammas.at(i)*1e-3 << " "; // in MeV
      }
      output_dance << endl;

    } //end of nb decays loop

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////-----------------  End of Argon 37 decay part -----------------------////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////-----------------  H 2 decay part -----------------------////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (isotope == "H2") {
    // List of levels    0        1
    double Levels[2] = {2224.572, 0.0};
    double level = 0.0;


    std::vector<double> v_gammas; v_gammas.clear(); // gammas for this event
    Int_t nb_gammas = 0; // number of gammas for this decay
    Double_t probability = 0.; // the probability we'll use in the loop
    Double_t Etot = 0.;

    // Fills the macro file with ndecays
    for (UInt_t n = 0; n < ndecays; n++) {

      // Initialization
      v_gammas.clear();
      nb_gammas = 0;


      level = Levels[0];
      while (level != Levels[1]) {

	if (level == Levels[0]) {
	  probability = rdef->Uniform(100.0);
	  if (probability <= 100.){
	    level = Levels[1];
	    v_gammas.push_back(2223.245); nb_gammas++;
	    continue;
	  }
	}
      }

      // Check if sum of gammas is 6.1
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	Etot += v_gammas.at(i);
      }
      //     cout<< Etot << endl;
      if (Etot < 2100 || Etot > 2300) {
	cout << "Summed gamma energy of " << Etot << " is not around 2.2 +/- 0.1 MeV. Something must be wrong************\n";
	Etot = 0.; continue;
      }
      Etot = 0.; // re-initialize Etot


      // Print and fill histograms
      //     cout << nb_gammas << endl;
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	//       cout << v_gammas.at(i) << endl;
	Gamma_spectrum->Fill(v_gammas.at(i));
      }
      //     cout << endl;

      // Generates position in DUNE (for each event, not each gamma)
      //Xg.SetXYZ(rdef->Uniform(-4000.,4000),rdef->Uniform(0.,6000),rdef->Uniform(0.,7000));
      Xg.SetXYZ(-100, 400, 100);


      // Now the part where the HEPEvt and DANCE input files are written
      output_dance << nb_gammas << endl;
      output_hepevt_larsoft << n << " " << nb_gammas << endl;
      output_hepevt_g4 << nb_gammas << endl;

      for (Int_t i=0; i < nb_gammas; i++) {
	// HEPEvt part
	// generate energy, momentum and position
	gRandom->Sphere(Pg[0],Pg[1],Pg[2], v_gammas.at(i)*1e-6); //random momentum over a sphere (in GeV)
	// write in HEPEvt file
	output_hepevt_larsoft << "1 22 0 0 0 0 "
			      << Pg[0] << " " << Pg[1] << " " << Pg[2] << " " << v_gammas.at(i)*1e-6 << " 0 "
			      << Xg[0] << " " << Xg[1] << " " << Xg[2] << " 0 0 0 " << endl;

	output_hepevt_g4 << "1 22 0 0 "
			 << Pg[0] << " " << Pg[1] << " " << Pg[2] << " 0\n";

	// DANCE input part
	output_dance << v_gammas.at(i)*1e-3 << " "; // in MeV
      }
      output_dance << endl;

    } //end of nb decays loop

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////-----------------  End of H 2 decay part -----------------------////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////-----------------  Carbon 13 decay part -----------------------////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (isotope == "C13") {
    // List of levels    0        1         2       3
    double Levels[4] = {4946.3120, 3684.475, 3089.446, 0.0};
    double level = 0.0;


    std::vector<double> v_gammas; v_gammas.clear(); // gammas for this event
    Int_t nb_gammas = 0; // number of gammas for this decay
    Double_t probability = 0.; // the probability we'll use in the loop
    Double_t Etot = 0.;

    // Fills the macro file with ndecays
    for (UInt_t n = 0; n < ndecays; n++) {

      // Initialization
      v_gammas.clear();
      nb_gammas = 0;


      level = Levels[0];
      while (level != Levels[3]) {

	if (level == Levels[0]) {
	  probability = rdef->Uniform(100.0);
	  if (probability <= 67.5){
	    level = Levels[3];
	    v_gammas.push_back(4945.301); nb_gammas++;
	    continue;
	  } else if (probability > 67.5 && probability <= 67.5 + 0.16) {
	    level = Levels[2];
	    v_gammas.push_back(1859.716); nb_gammas++;
	    continue;
	  } else if (probability > 67.5 + 0.16 && probability <= 100.0 ) {
	    level = Levels[1];
	    v_gammas.push_back(1261.764); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[1]) {
	  probability = rdef->Uniform(32.1 + 0.24);
	  if (probability <= 32.1){
	    level = Levels[3];
	    v_gammas.push_back(3683.921); nb_gammas++;
	    continue;
	  } else if (probability > 32.1 && probability <= 32.1 + 0.24) {
	    level = Levels[2];
	    v_gammas.push_back(595.013); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[2]) {
	  probability = rdef->Uniform(0.43);
	  if (probability <= 0.43) {
	    level = Levels[3];
	    v_gammas.push_back(3089.049); nb_gammas++;
	    continue;
	  }
	}
      }

      // Check if sum of gammas is 6.1
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	Etot += v_gammas.at(i);
      }
      //     cout<< Etot << endl;
      if (Etot < 4800 || Etot > 5000) {
	cout << "Summed gamma energy of " << Etot << " is not around 4.9 +/- 0.1 MeV. Something must be wrong************\n";
	Etot = 0.; continue;
      }
      Etot = 0.; // re-initialize Etot


      // Print and fill histograms
      //     cout << nb_gammas << endl;
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	//       cout << v_gammas.at(i) << endl;
	Gamma_spectrum->Fill(v_gammas.at(i));
      }
      //     cout << endl;

      // Generates position in DUNE (for each event, not each gamma)
      //Xg.SetXYZ(rdef->Uniform(-4000.,4000),rdef->Uniform(0.,6000),rdef->Uniform(0.,7000));
      Xg.SetXYZ(-100, 400, 100);


      // Now the part where the HEPEvt and DANCE input files are written
      output_dance << nb_gammas << endl;
      output_hepevt_larsoft << n << " " << nb_gammas << endl;
      output_hepevt_g4 << nb_gammas << endl;

      for (Int_t i=0; i < nb_gammas; i++) {
	// HEPEvt part
	// generate energy, momentum and position
	gRandom->Sphere(Pg[0],Pg[1],Pg[2], v_gammas.at(i)*1e-6); //random momentum over a sphere (in GeV)
	// write in HEPEvt file
	output_hepevt_larsoft << "1 22 0 0 0 0 "
			      << Pg[0] << " " << Pg[1] << " " << Pg[2] << " " << v_gammas.at(i)*1e-6 << " 0 "
			      << Xg[0] << " " << Xg[1] << " " << Xg[2] << " 0 0 0 " << endl;

	output_hepevt_g4 << "1 22 0 0 "
			 << Pg[0] << " " << Pg[1] << " " << Pg[2] << " 0\n";

	// DANCE input part
	output_dance << v_gammas.at(i)*1e-3 << " "; // in MeV
      }
      output_dance << endl;

    } //end of nb decays loop

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////-----------------  End of Carbon 13 decay part -----------------------////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////-----------------  Oxygen 17 decay part -----------------------////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (isotope == "O17") {
    // List of levels    0        1         2       3
    double Levels[4] = {4143.33, 3055.36, 870.73, 0.0};
    double level = 0.0;


    std::vector<double> v_gammas; v_gammas.clear(); // gammas for this event
    Int_t nb_gammas = 0; // number of gammas for this decay
    Double_t probability = 0.; // the probability we'll use in the loop
    Double_t Etot = 0.;

    // Fills the macro file with ndecays
    for (UInt_t n = 0; n < ndecays; n++) {

      // Initialization
      v_gammas.clear();
      nb_gammas = 0;


      level = Levels[0];
      while (level != Levels[3]) {

	if (level == Levels[0]) {
	  probability = rdef->Uniform(18.0 + 82.0);
	  if (probability <= 18.0){
	    level = Levels[2];
	    v_gammas.push_back(3272.26); nb_gammas++;
	    continue;
	  } else if (probability > 18.0 && probability <= 18.0 + 82.0) {
	    level = Levels[1];
	    v_gammas.push_back(1087.93); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[1]) {
	  probability = rdef->Uniform(82.0);
	  if (probability <= 82.0){
	    level = Levels[2];
	    v_gammas.push_back(2184.48); nb_gammas++;
	    continue;
	  }
	}

	if (level == Levels[2]) {
	  probability = rdef->Uniform(100.0);
	  if (probability <= 100.0) {
	    level = Levels[3];
	    v_gammas.push_back(870.71); nb_gammas++;
	    continue;
	  }
	}
      }

      // Check if sum of gammas is 6.1
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	Etot += v_gammas.at(i);
      }
      //     cout<< Etot << endl;
      if (Etot < 4000 || Etot > 4200) {
	cout << "Summed gamma energy of " << Etot << " is not around 4.1 +/- 0.1 MeV. Something must be wrong************\n";
	Etot = 0.; continue;
      }
      Etot = 0.; // re-initialize Etot


      // Print and fill histograms
      //     cout << nb_gammas << endl;
      for( UInt_t i = 0; i < v_gammas.size(); ++i) {
	//       cout << v_gammas.at(i) << endl;
	Gamma_spectrum->Fill(v_gammas.at(i));
      }
      //     cout << endl;

      // Generates position in DUNE (for each event, not each gamma)
      //Xg.SetXYZ(rdef->Uniform(-4000.,4000),rdef->Uniform(0.,6000),rdef->Uniform(0.,7000));
      Xg.SetXYZ(-100, 400, 100);


      // Now the part where the HEPEvt and DANCE input files are written
      output_dance << nb_gammas << endl;
      output_hepevt_larsoft << n << " " << nb_gammas << endl;
      output_hepevt_g4 << nb_gammas << endl;

      for (Int_t i=0; i < nb_gammas; i++) {
	// HEPEvt part
	// generate energy, momentum and position
	gRandom->Sphere(Pg[0],Pg[1],Pg[2], v_gammas.at(i)*1e-6); //random momentum over a sphere (in GeV)
	// write in HEPEvt file
	output_hepevt_larsoft << "1 22 0 0 0 0 "
			      << Pg[0] << " " << Pg[1] << " " << Pg[2] << " " << v_gammas.at(i)*1e-6 << " 0 "
			      << Xg[0] << " " << Xg[1] << " " << Xg[2] << " 0 0 0 " << endl;

	output_hepevt_g4 << "1 22 0 0 "
			 << Pg[0] << " " << Pg[1] << " " << Pg[2] << " 0\n";

	// DANCE input part
	output_dance << v_gammas.at(i)*1e-3 << " "; // in MeV
      }
      output_dance << endl;

    } //end of nb decays loop

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////-----------------  End of Oxygen 17 decay part -----------------------////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Edit and plot histo
  Gamma_spectrum->Scale(100./ndecays);
  Gamma_spectrum->GetXaxis()->SetTitle("Energy [keV]");
  Gamma_spectrum->GetYaxis()->SetTitle("Probability [%]");
  TCanvas *c1 = new TCanvas("c1","c1",0,0, 1200, 1000);
  c1->cd();
  Gamma_spectrum->DrawCopy();
  auto c2 = new TCanvas("c2", "c2");
  c2->cd();
  time->DrawCopy("colz");

  time->Write();


  delete Gamma_spectrum;
  output_hepevt_g4.close();
  output_hepevt_larsoft.close();
  output_dance.close();
  g_out->Close();

  cout << "Done! And in " << clock->RealTime() << " s." << endl;
}
