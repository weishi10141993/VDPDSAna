//#####################################################################
//###  BlipAnaMC analyzer module
//###
//###  Contains algorithms for reconstructing isolated, MeV-scale energy
//###  depositions in the TPC, called "blips." A TTree is made for offline
//###  analysis and plot-making.
//###
//###  Ported from code in MicroBooNE written in 2021-2022.
//###
//###  Author: Will Foreman
//###  Date:   Sept 2024
//#####################################################################
#ifndef BLIPANA_H
#define BLIPANA_H

// Framework includes
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// LArSoft includes
#include "lardataobj/RawData/RDTimeStamp.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/RawData/OpDetWaveform.h"
#include "larevt/SpaceChargeServices/SpaceChargeService.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/WireReadout.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/PhotonBackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "larsim/Utils/TruthMatchUtils.h"
// SBND-specific includes
#include "dunereco/BlipRecoDUNE/Alg/BlipRecoAlg.h"

// C++ includes
#include <cstring>
#include <utility>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <cmath>
#include <deque>
#include <limits>
#include <exception>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

// Helper templates for initializing arrays
namespace{
  template <typename ITER, typename TYPE>
    inline void FillWith(ITER from, ITER to, TYPE value)
    { std::fill(from, to, value); }
  template <typename ITER, typename TYPE>
    inline void FillWith(ITER from, size_t n, TYPE value)
    { std::fill(from, from + n, value); }
  template <typename CONT, typename V>
    inline void FillWith(CONT& data, const V& value)
    { FillWith(std::begin(data), std::end(data), value); }
}




/* ================= Morphological baseline ================= */
double median_func(std::vector<double> vec, double fraction) {
  if (vec.empty()) return 0;
  std::sort(vec.begin(), vec.end());
  size_t n = vec.size();
  size_t cutoff = static_cast<size_t>(n * fraction);
  if (cutoff == 0) cutoff = 1;
  std::vector<double> lowest(vec.begin(), vec.begin() + cutoff);
  TH1D* fhist = new TH1D("fhist","",100,-50,50);
  for(size_t k=0; k<lowest.size(); k++) fhist->Fill(lowest.at(k));
  double mode_center = fhist->GetBinCenter(fhist->GetMaximumBin());
  fhist->Delete();
  return mode_center;
}

double GetPDSAmpThreshold(int DAQch_ID)
{
  switch (DAQch_ID) {
    // Cathode channels C1-C8
    case 1010: return 0.6 * 55.0;   // C1_0
    case 1011: return 0.6 * 26.5;   // C1_1

    case 1020: return 0.6 * 31.1;   // C2_0
    case 1021: return 0.6*43.0;         // C2_1 (user-specified)

    case 1030: return 0.6 * 31.1;   // C3_0
    case 1031: return 0.6* 18.0;         // C3_1 (user-specified)

    case 1040: return 0.6 * 30.8;   // C4_0
    case 1041: return 0.6 * 35.6;   // C4_1

    case 1050: return 0.6 * 48.7;   // C5_0
    case 1051: return 0.6 * 47.2;   // C5_1

    case 1060: return 0.6 * 29.8;   // C6_0
    case 1061: return 0.6 * 29.4;   // C6_1

    case 1070: return 0.6 * 55.0;   // C7_0
    case 1071: return 0.6 * 51.0;   // C7_1

    case 1080: return 0.6 * 23.9;   // C8_0
    case 1081: return 0.6 * 21.4;   // C8_1

    // Membrane channels M1-M8
    // M1-M4 not in table -> default 50 ADC unless you want to add values later
    case 2010: return 50.0;         // M1_0
    case 2011: return 50.0;         // M1_1
    case 2020: return 50.0;         // M2_0
    case 2021: return 50.0;         // M2_1
    case 2030: return 50.0;         // M3_0
    case 2031: return 50.0;         // M3_1
    case 2040: return 50.0;         // M4_0
    case 2041: return 50.0;         // M4_1

    case 2050: return 0.6 * 41.2;   // M5_0
    case 2051: return 0.6 * 42.0;   // M5_1

    case 2060: return 0.6 * 42.8;   // M6_0
    case 2061: return 0.6 * 42.4;   // M6_1

    case 2070: return 0.6 * 46.8;   // M7_0
    case 2071: return 0.6 * 44.8;   // M7_1

    case 2080: return 0.6 * 42.3;   // M8_0
    case 2081: return 0.6 * 41.6;   // M8_1

    default:   return 50.0;         // fallback
  }
}

inline std::vector<double> trailing_min(const std::vector<double>& x, int L) {
  const int N = static_cast<int>(x.size());
  if (N == 0) return {};
  if (L <= 1) return x;
  L = std::min(L, N);
  std::vector<double> y(N);
  std::deque<int> dq;
  for (int i = 0; i < N; ++i) {
    while (!dq.empty() && x[i] <= x[dq.back()]) dq.pop_back();
    dq.push_back(i);
    const int oldest = i - L + 1;
    while (!dq.empty() && dq.front() < oldest) dq.pop_front();
    y[i] = x[dq.front()];
  }
  return y;
}

inline std::vector<double> trailing_max(const std::vector<double>& x, int L) {
  const int N = static_cast<int>(x.size());
  if (N == 0) return {};
  if (L <= 1) return x;
  L = std::min(L, N);
  std::vector<double> y(N);
  std::deque<int> dq;
  for (int i = 0; i < N; ++i) {
    while (!dq.empty() && x[i] >= x[dq.back()]) dq.pop_back();
    dq.push_back(i);
    const int oldest = i - L + 1;
    while (!dq.empty() && dq.front() < oldest) dq.pop_front();
    y[i] = x[dq.front()];
  }
  return y;
}

inline std::vector<double> centered_min(const std::vector<double>& x, int L) {
  const int N = static_cast<int>(x.size());
  if (N == 0) return {};
  if (L <= 1) return x;
  L = std::min(L, N);
  const int r = (L - 1) / 2;
  auto tmin = trailing_min(x, L);
  std::vector<double> y(N);
  for (int i = 0; i < N; ++i) {
    int j = i + r;
    if (j >= N) j = N - 1;
    y[i] = tmin[j];
  }
  return y;
}

inline std::vector<double> centered_max(const std::vector<double>& x, int L) {
  const int N = static_cast<int>(x.size());
  if (N == 0) return {};
  if (L <= 1) return x;
  L = std::min(L, N);
  const int r = (L - 1) / 2;
  auto tmax = trailing_max(x, L);
  std::vector<double> y(N);
  for (int i = 0; i < N; ++i) {
    int j = i + r;
    if (j >= N) j = N - 1;
    y[i] = tmax[j];
  }
  return y;
}

inline std::vector<double> EstimateBaselineOpeningCentered(const std::vector<double>& wave, int L, int smooth_ma = 4) {
  const int N = static_cast<int>(wave.size());
  if (N == 0) return {};
  L = std::max(3, std::min(L, N));
  auto eroded = centered_min(wave, L);
  auto opened = centered_max(eroded, L);
  if (smooth_ma > 1) {
    std::vector<double> sm(N);
    double run = 0.0;
    int w = 0;
    for (int i = 0; i < N; ++i) {
      run += opened[i];
      ++w;
      if (w > smooth_ma) { run -= opened[i - smooth_ma]; --w; }
      sm[i] = run / w;
    }
    return sm;
  }
  return opened;
}

void findPeaks(const std::vector<double>& waveform,
               double threshold,
               int peak_distance,
               std::vector<double>& peakIndices,
               std::vector<double>& peakValues,
               std::vector<double>& widths,
               std::vector<double>& integrals,
               std::vector<int>& leftFound,
               std::vector<int>& rightFound)
{
  for (size_t i = peak_distance; i < waveform.size() - peak_distance; ++i) {
    if (waveform[i] > threshold) {
      bool isPeak = true;
      for (size_t j = i - peak_distance; j <= i + peak_distance; ++j) {
        if (j != i && waveform[j] > waveform[i]) {
          isPeak = false;
          break;
        }
      }
      if (isPeak) {
        peakIndices.push_back(i);
        peakValues.push_back(waveform[i]);
        double peakHeight = waveform[i];
        double fivePercent = 0.05 * peakHeight;
        int foundLeft = 0;
        int foundRight = 0;
        size_t leftb = i;
        for (int l = (int)i; l >= 0; --l) {
          if (waveform[l] < fivePercent) {
            leftb = (size_t)l;
            foundLeft = 1;
            break;
          }
        }
        size_t rightb = i;
        for (size_t l = i; l < waveform.size(); ++l) {
          if (waveform[l] < fivePercent) {
            rightb = l;
            foundRight = 1;
            break;
          }
        }
        widths.push_back(rightb - leftb);
        double integral = 0.0;
        for (size_t k = leftb; k <= rightb; ++k) integral += waveform[k];
        integrals.push_back(integral);
        leftFound.push_back(foundLeft);
        rightFound.push_back(foundRight);
        i = TMath::Max(rightb, i + peak_distance);
      }
    }
  }
}

// Set global constants and max array sizes
const int kMaxHits    =  300000;
const int kMaxOpHits  =  300000;
const int kMaxClusts  =  300000;
const int kMaxTrks    =   10000;
const int kMaxBlips   =   50000;
const int kMaxG4      =   30000;
const int kMaxEDeps   =   30000;
const int kMaxTrkPts  =   5000;

class BlipAnaMC;

//###################################################
//  Data storage structure
//###################################################
class BlipAnaMCTreeDataStruct
{
  public:

  // --- TTrees
  TTree* evtTree;

  // --- Configurations and switches ---
  std::string treeName      = "anatree";
  bool  saveTruthInfo       = true;
  bool  saveTrkInfo         = true;
  bool  saveHitInfo         = true;
  bool  saveClustInfo       = true;

  // --- Event information ---
  int           event;                // event number
  int           run;                  // run number
  int           subrun;               // subrun number
  unsigned int  timestamp;            // unix time of event
  float         lifetime;             // electron lifetime
  int           badchans;             // #bad chans according to wirecell
  int           longtrks;             // tracks > 5 cm
  int           trigcode;
  float         collcharge;           // total collection plane charge

  // --- G4 information ---
  int   nparticles;                   // number of G4 particles
  bool  part_isPrimary[kMaxG4];       // is primary particle
  int   part_trackID[kMaxG4];         // G4 track ID
  int   part_pdg[kMaxG4];             // PDG
  int   part_nDaughters[kMaxG4];      // number of daughters
  int   part_mother[kMaxG4];          // mother particle
  float part_E[kMaxG4];               // initial energy (MeV)
  float part_KE[kMaxG4];              // initial kinetic energy (MeV)
  float part_endE[kMaxG4];            // final energy (MeV)
  float part_endKE[kMaxG4];           // final energy (MeV)
  float part_mass[kMaxG4];            // mass (MeV)
  float part_P[kMaxG4];               // momentum (MeV)
  float part_Px[kMaxG4];              // momentum x (MeV)
  float part_Py[kMaxG4];              // momentum y (MeV)
  float part_Pz[kMaxG4];              // momentum z (MeV)
  float part_startPointx[kMaxG4];     // starting x (cm)
  float part_startPointy[kMaxG4];     // starting y (cm)
  float part_startPointz[kMaxG4];     // starting y (cm)
  float part_endPointx[kMaxG4];       // ending x (cm)
  float part_endPointy[kMaxG4];       // ending y (cm)
  float part_endPointz[kMaxG4];       // ending y (cm)
  float part_startT[kMaxG4];          // starting time (us)
  float part_endT[kMaxG4];            // ending time (us)
  float part_pathlen[kMaxG4];         // path length (cm)
  int   part_numTrajPts[kMaxG4];      // number traj points
  float part_depEnergy[kMaxG4];       // energy deposited in AV (MeV)
  int   part_depElectrons[kMaxG4];    // electrons deposited
  std::vector<std::string> part_process;// process name

  // --- True energy deposit info (derived from SimChannels and SimEnergyDeposits) ---
  int   nedeps;                   // number of true localized energy depositions
  int   edep_tpc[kMaxEDeps];      // TPC
  int   edep_partid[kMaxEDeps];   // leading particle index 'i' ("part_variable[i]")
  int   edep_g4trkid[kMaxEDeps];  // leading G4 track ID ("part_trackID")
  float edep_g4qfrac[kMaxEDeps];  // fraction of total charge from lead particle
  bool  edep_isPrimary[kMaxEDeps];// matched to a primary generated particle?
  int   edep_pdg[kMaxEDeps];      // leading G4 track PDG
  int   edep_blipid[kMaxEDeps];   // reconstructed 3D blip ID
  float edep_energy[kMaxEDeps];   // total energy deposited [MeV]
  int   edep_electrons[kMaxEDeps];// total ionization electrons deposited (e-)
  int   edep_charge[kMaxEDeps];   // total electrons reaching anode wires (e-)
  int   edep_tdrift[kMaxEDeps];   // drift time for this energy dep (us)
  float edep_x[kMaxEDeps];        // x (cm)
  float edep_y[kMaxEDeps];        // y (cm)
  float edep_z[kMaxEDeps];        // z (cm)
  float edep_dx[kMaxEDeps];       // dx (cm)
  float edep_dz[kMaxEDeps];       // dz (cm)
  int   edep_proc[kMaxEDeps];     // encodes particle process
                                  //  0 = primary
                                  //  1 = compton scatter ("compt")
                                  //  2 = photoelectric effect ("phot")
                                  //  3 = e+e- pair production ("conv")
                                  //  4 = other

  // --- Hit information ---
  int	  nhits;                    // number of hits
  int   hit_cryo[kMaxHits];       // cryostat
  int	  hit_tpc[kMaxHits];        // tpc number
  int	  hit_plane[kMaxHits];      // plane number
  int	  hit_wire[kMaxHits];       // wire number
  int	  hit_channel[kMaxHits];    // channel ID
  float	hit_peakT[kMaxHits];      // raw peak time (tick)
  float	hit_time[kMaxHits];       // corrected peak time (tick)
  float hit_rms[kMaxHits];        // shape RMS
  float	hit_amp[kMaxHits];        // amplitude
  float	hit_area[kMaxHits];       // charge (area) in ADC units
  float hit_sumadc[kMaxHits];     // summed ADC
  float hit_charge[kMaxHits];     // reconstructed number of electrons
  int   hit_mult[kMaxHits];       // multiplicity
  int	  hit_trkid[kMaxHits];      // is this hit associated with a reco track?
  int   hit_ismatch[kMaxHits];    // does hit have time match on another plane?
  int	  hit_g4trkid[kMaxHits];    // G4 TrackID of leading particle
  float hit_g4frac[kMaxHits];     // fraction of hit charge from leading MCParticle
  float hit_g4energy[kMaxHits];   // true energy
  float hit_g4charge[kMaxHits];   // true number of electrons at wire
  int   hit_clustid[kMaxHits];    // key of HitClust in which hit was included
  int   hit_blipid[kMaxHits];     // key of Blip in which hit was included
  float hit_gof[kMaxHits];        // goodness of fit (default -1)

  // --- Track information ---
  int   ntrks;                    // number tracks
  int   trk_id[kMaxTrks];         // trackID
  int   trk_npts[kMaxTrks];       // number 3D trajectory points
  float trk_length[kMaxTrks];     // track length [cm]
  float trk_extent[kMaxTrks];     // extent of track (start-end) [cm]
  float trk_startx[kMaxTrks];     // starting X coordinate
  float trk_starty[kMaxTrks];     // starting Y coordinate
  float trk_startz[kMaxTrks];     // starting Z coordinate
  float trk_startd[kMaxTrks];     // starting distance to boundary
  float trk_endx[kMaxTrks];       // ending X coordinate
  float trk_endy[kMaxTrks];       // ending Y coordinate
  float trk_endz[kMaxTrks];       // ending Z coordinate
  float trk_endd[kMaxTrks];       // ending distance to boundary
  float track_time[kMaxTrks];       // first/minimum hit time recorded by track

  // --- Photon detector information ---
  std::vector<short> DAQch_ID;
  std::vector<std::vector<float>> waveformsADC;
  std::vector<double> PeakPDS, YPDS, ZPDS, TPDS, Maxfraction, WidthPDS;
  std::vector<std::vector<double>> peakPDS, tPDS, intPDS, widthPDS;
  std::vector<std::vector<int>> leftPDS, rightPDS;
  std::vector<float> channelraw;
  std::vector<std::vector<float>> adcraw;
  std::vector<float> channeldec;
  std::vector<std::vector<float>> adcdec;
  std::vector<ULong64_t> pds_timestamp;      // shifted so earliest selected waveform timestamp is zero
  std::vector<ULong64_t> pds_timestamp_raw;  // original raw waveform timestamp
  ULong64_t pds_timestamp_min;
  ULong64_t pds_timestamp_max;
  int t_offset;

  // --- Optical hit information ---
  int   nophits;
  double ophit_min_time;
  double ophit_max_time;
  int   ophit_opchannel[kMaxOpHits];
  int   ophit_bt[kMaxOpHits];
  int   ophit_frame[kMaxOpHits];
  double ophit_peak_time[kMaxOpHits];
  double ophit_peak_time_abs[kMaxOpHits];
  double ophit_time[kMaxOpHits];          // PeakTime() shifted so earliest OpHit PeakTime is zero
  double ophit_start_time[kMaxOpHits];
  double ophit_start_time_shifted[kMaxOpHits];
  double ophit_rise_time[kMaxOpHits];
  double ophit_width[kMaxOpHits];
  double ophit_area[kMaxOpHits];
  double ophit_amplitude[kMaxOpHits];
  double ophit_pe[kMaxOpHits];
  double ophit_fast_to_total[kMaxOpHits];
  bool  ophit_has_start_time[kMaxOpHits];


  // --- Hit cluster information ---
  int   nclusts;                      // total clusters made
  int   clust_id[kMaxClusts];           // cluster ID (index)
  int   clust_cryo[kMaxClusts];         // cluster cryostat ID
  int   clust_tpc[kMaxClusts];          // cluster TPC ID
  int   clust_plane[kMaxClusts];        // cluster plane
  int   clust_wire[kMaxClusts];         // central-most wire of cluster
  int   clust_startwire[kMaxClusts];    // starting wire
  int   clust_endwire[kMaxClusts];      // ending wire
  int   clust_nwires[kMaxClusts];       // number of wires in this cluster
  //int   clust_deadwiresep[kMaxClusts];  // separation from nearest dead region (0=adjacent)
  bool  clust_bydeadwire[kMaxClusts];     // is cluster adjacent to a dead wire
  //int   clust_nticks[kMaxClusts];       // timespan in ticks
  int   clust_nhits[kMaxClusts];        // number of hits
  int   clust_charge[kMaxClusts];       // cluster charge at anode [e-]
  int   clust_chargeErr[kMaxClusts];    // cluster charge uncertainty
  float clust_amp[kMaxClusts];          // maximum hit amplitude [ADC]
  float clust_time[kMaxClusts];         // charge-weighted time [tick]
  float clust_timespan[kMaxClusts];     // timespan [tick]
  float clust_starttime[kMaxClusts];    // cluster start tick
  float clust_endtime[kMaxClusts];      // cluster end tick
  bool  clust_pulsetrain[kMaxClusts];   // does this cluster include pulse-trains?
  int   clust_blipid[kMaxClusts];       // blip ID for this nlusteer (if it was made into one)
  int   clust_edepid[kMaxClusts];       // true energy dep ID
  bool  clust_ismatch[kMaxClusts];      // was this cluster plane-matched?
  bool  clust_touchtrk[kMaxClusts];     // does this cluster connect to a trk (like a delta ray)?
  int   clust_touchtrkid[kMaxClusts];   // connected track ID


  // --- 3D Blip information ---
  int   nblips;                       // number of blips in event
  int   blip_id[kMaxBlips];           // blip ID / index
  int   blip_cryo[kMaxBlips];         // blip cryostat ID
  int   blip_tpc[kMaxBlips];          // blip TPC
  int   blip_nplanes[kMaxBlips];      // number of planes matched (2 or 3)
  int   blip_leadTrkID[kMaxBlips];    // lead G4 trk id
  int   blip_bt[kMaxBlips];           // blip backtrack
  float blip_time[kMaxBlips];         // drift time [us]
  float blip_x[kMaxBlips];            // X position [cm]
  float blip_y[kMaxBlips];            // Y position [cm]
  float blip_z[kMaxBlips];            // Z position [cm]
  float blip_sigmayz[kMaxBlips];      // difference in wire intersection points
  float blip_dx[kMaxBlips];           // Spatial extent along drift-X [cm]
  float blip_dw[kMaxBlips];           // Spatial extent in YZ-plane [cm]
  float blip_size[kMaxBlips];         // rough size estimation based on values above
  int   blip_charge[kMaxBlips];       // blip charge at anode [e-]
  float blip_energy[kMaxBlips];       // blip reco energy [MeV]
  float blip_energyCorr[kMaxBlips];   // blip reco energy (SCE+lifetime corr.) [MeV]
  float blip_energyTrue[kMaxBlips];   // blip truth energy [MeV]
  float blip_yzcorr[kMaxBlips];       // YZ uniformity correction factor (already applied)
  bool  blip_isMC[kMaxBlips];         // blip is matched to MC particle
  int   blip_edepid[kMaxBlips];       // true energy dep ID ("edep_variable[id]")
  float blip_proxtrkdist[kMaxBlips];  // distance to nearest track
  int   blip_proxtrkid[kMaxBlips];    // index of nearest trk
  bool  blip_touchtrk[kMaxBlips];     // is blip touching track?
  int   blip_touchtrkid[kMaxBlips];   // track ID of touched track
  bool  blip_incylinder[kMaxBlips];   // is blip within a cylinder near a track
  int   blip_clustid[kNplanes][kMaxBlips];// cluster ID per plane
  int   blip_nwires[kNplanes][kMaxBlips]; // nwires per plane
  float blip_maxamp[kNplanes][kMaxBlips]; // max amplitude among all hits in blip (plane-specific)
  float blip_minamp[kNplanes][kMaxBlips]; // min amplitude among all groups of hits per wire (plane-specific thresholding study)

  TTree*  calibTree;
  int     acptrk_npts;
  float   acptrk_theta_xz;
  float   acptrk_theta_yz;
  float   acptrk_qratio_median;
  float   acptrk_qratio_mean;


  // === Function for resetting data ===
  void Clear(){
    event                 = -999; // --- event-wide info ---
    run                   = -999;
    subrun                = -999;
    lifetime              = -999;
    badchans              = -99;
    longtrks              = -99;
    trigcode              = -9;
    timestamp             = -999;
    nparticles            = 0;    // --- G4 particles ---
    FillWith(part_isPrimary,   false);
    FillWith(part_trackID,     -999);
    FillWith(part_pdg,         -99999);
    FillWith(part_nDaughters,  -999);
    FillWith(part_mother,      -999);
    FillWith(part_E,           -999.);
    FillWith(part_endE,        -999.);
    FillWith(part_KE,           -999.);
    FillWith(part_endKE,        -999.);
    FillWith(part_mass,        -999.);
    FillWith(part_P,           -999.);
    FillWith(part_Px,          -999.);
    FillWith(part_Py,          -999.);
    FillWith(part_Pz,          -999.);
    FillWith(part_startPointx, -99999.);
    FillWith(part_startPointy, -99999.);
    FillWith(part_startPointz, -99999.);
    FillWith(part_endPointx,   -99999.);
    FillWith(part_endPointy,   -99999.);
    FillWith(part_endPointz,   -99999.);
    FillWith(part_startT,      -99999.);
    FillWith(part_endT,        -99999.);
    FillWith(part_pathlen,     -999.);
    FillWith(part_numTrajPts,   -9);
    FillWith(part_depElectrons,-999);
    FillWith(part_depEnergy,   -999.);
    FillWith(part_process,     "");
    nedeps                = 0;    // --- EDeps ---
    FillWith(edep_tpc,    -9);
    FillWith(edep_energy, -999);
    FillWith(edep_electrons,  -999);
    FillWith(edep_charge, -999);
    FillWith(edep_tdrift, -999);
    FillWith(edep_x,      -99999.);
    FillWith(edep_y,      -99999.);
    FillWith(edep_z,      -99999.);
    FillWith(edep_dx,      -99999.);
    FillWith(edep_dz,      -99999.);
    FillWith(edep_g4trkid,  -9);
    FillWith(edep_partid,     -9);
    FillWith(edep_g4qfrac,  -9);
    FillWith(edep_pdg,   -999);
    FillWith(edep_proc,   -9);
    FillWith(edep_isPrimary, false);
    FillWith(edep_blipid, -9);
    nhits                 = 0;    // --- TPC hits ---
    if( saveHitInfo ) {
      FillWith(hit_cryo,    -9);
      FillWith(hit_tpc,     -9);
      FillWith(hit_plane,   -9);
      FillWith(hit_wire,    -999);
      FillWith(hit_channel, -999);
      FillWith(hit_peakT,   -999);
      FillWith(hit_time,    -999);
      FillWith(hit_rms,     -999);
      FillWith(hit_amp,      -999);
      FillWith(hit_area,    -999);
      FillWith(hit_sumadc,  -999);
      FillWith(hit_mult,    -999);
      FillWith(hit_charge,  -999);
      FillWith(hit_ismatch, -9);
      FillWith(hit_trkid,   -9);
      FillWith(hit_g4trkid, -999);
      FillWith(hit_g4frac,  -9);
      FillWith(hit_g4energy,-999);
      FillWith(hit_g4charge,-999);
      FillWith(hit_clustid, -9);
      FillWith(hit_blipid,  -9);
      FillWith(hit_gof,     -9);
    }
    ntrks                 = 0;    // --- Tracks ---
    if( saveTrkInfo ) {
      FillWith(trk_id,      -999);
      FillWith(trk_npts,    -999);
      FillWith(trk_length,  -999);
      FillWith(trk_extent,  -999);
      FillWith(trk_startx,  -999);
      FillWith(trk_starty,  -999);
      FillWith(trk_startz,  -999);
      FillWith(trk_startd,  -999);
      FillWith(trk_endx,    -999);
      FillWith(trk_endy,    -999);
      FillWith(trk_endz,    -999);
      FillWith(trk_endd,    -999);
      FillWith(track_time,    -999);
    }
    DAQch_ID.clear();
    waveformsADC.clear();
    PeakPDS.clear(); YPDS.clear(); ZPDS.clear(); TPDS.clear(); Maxfraction.clear(); WidthPDS.clear();
    peakPDS.clear(); tPDS.clear(); intPDS.clear(); widthPDS.clear();
    leftPDS.clear(); rightPDS.clear();
    channelraw.clear(); adcraw.clear();
    channeldec.clear(); adcdec.clear();
    pds_timestamp.clear();
    pds_timestamp_raw.clear();
    pds_timestamp_min = 0;
    pds_timestamp_max = 0;
    t_offset = -9999;
    nophits = 0;
    ophit_min_time = -999.;
    ophit_max_time = -999.;

    FillWith(ophit_opchannel,      -999);
    FillWith(ophit_bt,             -999);
    FillWith(ophit_frame,          -999);
    FillWith(ophit_peak_time,      -999.);
    FillWith(ophit_peak_time_abs,  -999.);
    FillWith(ophit_time,           -999.);
    FillWith(ophit_start_time,     -999.);
    FillWith(ophit_start_time_shifted, -999.);
    FillWith(ophit_rise_time,      -999.);
    FillWith(ophit_width,          -999.);
    FillWith(ophit_area,           -999.);
    FillWith(ophit_amplitude,      -999.);
    FillWith(ophit_pe,             -999.);
    FillWith(ophit_fast_to_total,  -999.);
    FillWith(ophit_has_start_time, false);
    nclusts                   = 0;    // --- Hit Clusters ---
    FillWith(clust_id,        -9);
    FillWith(clust_cryo,       -9);
    FillWith(clust_tpc,       -9);
    FillWith(clust_plane,     -9);
    FillWith(clust_nwires,    -9);
    FillWith(clust_bydeadwire, false);
    FillWith(clust_nhits,     -9);
    FillWith(clust_wire,      -9);
    FillWith(clust_startwire, -9);
    FillWith(clust_endwire,   -9);
    FillWith(clust_charge,    -999);
    FillWith(clust_chargeErr,    -999);
    FillWith(clust_time,      -999);
    FillWith(clust_timespan,  -9);
    FillWith(clust_starttime, -999);
    FillWith(clust_endtime,   -999);
    FillWith(clust_amp,       -9);
    FillWith(clust_pulsetrain,  false);
    FillWith(clust_edepid,    -9);
    FillWith(clust_blipid,    -9);
    FillWith(clust_ismatch,   false);
    FillWith(clust_touchtrk,   false);
    FillWith(clust_touchtrkid,  -9);
    nblips                    = 0;
    FillWith(blip_id,         -9);
    FillWith(blip_cryo,        -9);
    FillWith(blip_tpc,        -9);
    FillWith(blip_nplanes,    -9);
    FillWith(blip_leadTrkID,  -9);
    FillWith(blip_bt,         -999);
    FillWith(blip_time,       -99);
    FillWith(blip_x,          -9999);
    FillWith(blip_y,          -9999);
    FillWith(blip_z,          -9999);
    FillWith(blip_sigmayz,    -9);
    FillWith(blip_dx,         -9);
    FillWith(blip_dw,        -9);
    FillWith(blip_size,       -9);
    FillWith(blip_charge,     -999);
    FillWith(blip_energy,     -999);
    FillWith(blip_energyCorr, -999);
    FillWith(blip_energyTrue, -999);
    FillWith(blip_yzcorr,     -9);
    FillWith(blip_proxtrkdist,-99);
    FillWith(blip_proxtrkid,  -9);
    FillWith(blip_touchtrk,   false);
    FillWith(blip_touchtrkid,  -9);
    FillWith(blip_incylinder, false);
    FillWith(blip_isMC,       false);
    FillWith(blip_edepid,     -9);
    for(int i=0; i<kNplanes; i++){
      FillWith(blip_clustid[i], -9);
      FillWith(blip_nwires[i],  -9);
      FillWith(blip_maxamp[i],  -9);
      FillWith(blip_minamp[i],  -9);
    }
  }

  // === Function for resizing vectors (if necessary) ===
  // To be called after numbers of hits/tracks/particles
  // in the event has been determined
  void Resize() {
    if(nparticles) part_process.assign(nparticles,"");
  }

  // === Function for initializing tree branches ===
  void MakeTree(){

    art::ServiceHandle<art::TFileService> tfs;
    evtTree = tfs->make<TTree>(treeName.c_str(),"analysis tree");
    evtTree->Branch("event",&event,"event/I");
    evtTree->Branch("run",&run,"run/I");
    evtTree->Branch("subrun",&subrun,"subrun/I");
    evtTree->Branch("timestamp",&timestamp,"timestamp/i");
    evtTree->Branch("lifetime",&lifetime,"lifetime/F");
    evtTree->Branch("trigcode",&trigcode,"trigcode/I");
    evtTree->Branch("longtrks",&longtrks,"longtrks/I");

    if( saveHitInfo ) {
      evtTree->Branch("nhits",&nhits,"nhits/I");
      evtTree->Branch("hit_cryo",hit_cryo,"hit_cryo[nhits]/I");
      evtTree->Branch("hit_tpc",hit_tpc,"hit_tpc[nhits]/I");
      evtTree->Branch("hit_plane",hit_plane,"hit_plane[nhits]/I");
      evtTree->Branch("hit_wire",hit_wire,"hit_wire[nhits]/I");
      evtTree->Branch("hit_peakT",hit_peakT,"hit_peakT[nhits]/F");
      evtTree->Branch("hit_time",hit_time,"hit_time[nhits]/F");
      evtTree->Branch("hit_rms",hit_rms,"hit_rms[nhits]/F");
      evtTree->Branch("hit_amp",hit_amp,"hit_amp[nhits]/F");
      evtTree->Branch("hit_area",hit_area,"hit_area[nhits]/F");
      evtTree->Branch("hit_sumadc",hit_sumadc,"hit_sumadc[nhits]/F");
      evtTree->Branch("hit_mult",hit_mult,"hit_mult[nhits]/I");
      evtTree->Branch("hit_charge",hit_charge,"hit_charge[nhits]/F");
      evtTree->Branch("hit_ismatch",hit_ismatch,"hit_ismatch[nhits]/I");
      evtTree->Branch("hit_trkid",hit_trkid,"hit_trkid[nhits]/I");
      if( saveTruthInfo ) {
      evtTree->Branch("hit_g4trkid",hit_g4trkid,"hit_g4trkid[nhits]/I");
      evtTree->Branch("hit_g4frac",hit_g4frac,"hit_g4frac[nhits]/F");
      evtTree->Branch("hit_g4energy",hit_g4energy,"hit_g4energy[nhits]/F");
      evtTree->Branch("hit_g4charge",hit_g4charge,"hit_g4charge[nhits]/F");
      }
      evtTree->Branch("hit_clustid",hit_clustid,"hit_clustid[nhits]/I");
      evtTree->Branch("hit_blipid",hit_blipid,"hit_blipid[nhits]/I");
      evtTree->Branch("hit_gof",hit_gof,"hit_gof[nhits]/F");
    }

    if( saveTrkInfo ) {
      evtTree->Branch("ntrks",&ntrks,"ntrks/I");
      evtTree->Branch("trk_id",trk_id,"trk_id[ntrks]/I");
      evtTree->Branch("trk_length",trk_length,"trk_length[ntrks]/F");
      evtTree->Branch("trk_extent",trk_extent,"trk_extent[ntrks]/F");
      evtTree->Branch("trk_startx",trk_startx,"trk_startx[ntrks]/F");
      evtTree->Branch("trk_starty",trk_starty,"trk_starty[ntrks]/F");
      evtTree->Branch("trk_startz",trk_startz,"trk_startz[ntrks]/F");
      evtTree->Branch("trk_endx",trk_endx,"trk_endx[ntrks]/F");
      evtTree->Branch("trk_endy",trk_endy,"trk_endy[ntrks]/F");
      evtTree->Branch("trk_endz",trk_endz,"trk_endz[ntrks]/F");
      evtTree->Branch("track_time",track_time,"track_time[ntrks]/F");
    }

    evtTree->Branch("DAQch_ID", &DAQch_ID);
    evtTree->Branch("waveformsADC", &waveformsADC);
    evtTree->Branch("PeakPDS", &PeakPDS);
    evtTree->Branch("TPDS", &TPDS);
    evtTree->Branch("YPDS", &YPDS);
    evtTree->Branch("ZPDS", &ZPDS);
    evtTree->Branch("Maxfraction", &Maxfraction);
    evtTree->Branch("WidthPDS", &WidthPDS);
    evtTree->Branch("peakPDS", &peakPDS);
    evtTree->Branch("tPDS", &tPDS);
    evtTree->Branch("intPDS", &intPDS);
    evtTree->Branch("widthPDS", &widthPDS);
    evtTree->Branch("leftPDS", &leftPDS);
    evtTree->Branch("rightPDS", &rightPDS);
    evtTree->Branch("channelraw", &channelraw);
    evtTree->Branch("adcraw", &adcraw);
    evtTree->Branch("channeldec", &channeldec);
    evtTree->Branch("adcdec", &adcdec);
    evtTree->Branch("pds_timestamp", &pds_timestamp);
    evtTree->Branch("pds_timestamp_raw", &pds_timestamp_raw);
    evtTree->Branch("pds_timestamp_min", &pds_timestamp_min, "pds_timestamp_min/l");
    evtTree->Branch("pds_timestamp_max", &pds_timestamp_max, "pds_timestamp_max/l");
    evtTree->Branch("t_offset", &t_offset, "t_offset/I");

    evtTree->Branch("nophits", &nophits, "nophits/I");
    evtTree->Branch("ophit_min_time", &ophit_min_time, "ophit_min_time/D");
    evtTree->Branch("ophit_max_time", &ophit_max_time, "ophit_max_time/D");
    evtTree->Branch("ophit_opchannel", ophit_opchannel, "ophit_opchannel[nophits]/I");
    evtTree->Branch("ophit_bt", ophit_bt, "ophit_bt[nophits]/I");
    evtTree->Branch("ophit_frame", ophit_frame, "ophit_frame[nophits]/I");
    evtTree->Branch("ophit_peak_time", ophit_peak_time, "ophit_peak_time[nophits]/D");
    evtTree->Branch("ophit_peak_time_abs", ophit_peak_time_abs, "ophit_peak_time_abs[nophits]/D");
    evtTree->Branch("ophit_time", ophit_time, "ophit_time[nophits]/D");
    evtTree->Branch("ophit_start_time", ophit_start_time, "ophit_start_time[nophits]/D");
    evtTree->Branch("ophit_start_time_shifted", ophit_start_time_shifted, "ophit_start_time_shifted[nophits]/D");
    evtTree->Branch("ophit_rise_time", ophit_rise_time, "ophit_rise_time[nophits]/D");
    evtTree->Branch("ophit_width", ophit_width, "ophit_width[nophits]/D");
    evtTree->Branch("ophit_area", ophit_area, "ophit_area[nophits]/D");
    evtTree->Branch("ophit_amplitude", ophit_amplitude, "ophit_amplitude[nophits]/D");
    evtTree->Branch("ophit_pe", ophit_pe, "ophit_pe[nophits]/D");
    evtTree->Branch("ophit_fast_to_total", ophit_fast_to_total, "ophit_fast_to_total[nophits]/D");
    evtTree->Branch("ophit_has_start_time", ophit_has_start_time, "ophit_has_start_time[nophits]/O");

    if( saveClustInfo ) {
      evtTree->Branch("nclusts",        &nclusts,       "nclusts/I");
      evtTree->Branch("clust_cryo",    clust_cryo,    "clust_cryo[nclusts]/I");
      evtTree->Branch("clust_tpc",    clust_tpc,    "clust_tpc[nclusts]/I");
      evtTree->Branch("clust_plane",    clust_plane,    "clust_plane[nclusts]/I");
      evtTree->Branch("clust_nhits",    clust_nhits,    "clust_nhits[nclusts]/I");
      evtTree->Branch("clust_nwires",   clust_nwires,   "clust_nwires[nclusts]/I");
      evtTree->Branch("clust_startwire",clust_startwire,"clust_startwire[nclusts]/I");
      evtTree->Branch("clust_endwire",  clust_endwire,  "clust_endwire[nclusts]/I");
      evtTree->Branch("clust_bydeadwire",   clust_bydeadwire,   "clust_bydeadwire[nclusts]/O");
      evtTree->Branch("clust_time",     clust_time,     "clust_time[nclusts]/F");
      evtTree->Branch("clust_timespan", clust_timespan, "clust_timespan[nclusts]/F");
      evtTree->Branch("clust_charge",   clust_charge,   "clust_charge[nclusts]/I");
      evtTree->Branch("clust_amp",      clust_amp,      "clust_amp[nclusts]/F");
      evtTree->Branch("clust_ismatch",  clust_ismatch,  "clust_ismatch[nclusts]/O");
      evtTree->Branch("clust_touchtrk",  clust_touchtrk,  "clust_touchtrk[nclusts]/O");
      if( saveTrkInfo ) evtTree->Branch("clust_touchtrkid",  clust_touchtrkid,  "clust_touchtrkid[nclusts]/I");
      evtTree->Branch("clust_blipid",   clust_blipid,   "clust_blipid[nclusts]/I");
      if( saveTruthInfo ) evtTree->Branch("clust_edepid",   clust_edepid,   "clust_edepid[nclusts]/I");
    }

    evtTree->Branch("nblips",&nblips,"nblips/I");
    evtTree->Branch("blip_cryo",blip_cryo,"blip_cryo[nblips]/I");
    evtTree->Branch("blip_tpc",blip_tpc,"blip_tpc[nblips]/I");
    evtTree->Branch("blip_nplanes",blip_nplanes,"blip_nplanes[nblips]/I");
    evtTree->Branch("blip_leadTrkID",blip_leadTrkID,"blip_leadTrkID[nblips]/I");
    evtTree->Branch("blip_bt",blip_bt,"blip_bt[nblips]/I");
    evtTree->Branch("blip_time",blip_time,"blip_time[nblips]/F");
    evtTree->Branch("blip_x",blip_x,"blip_x[nblips]/F");
    evtTree->Branch("blip_y",blip_y,"blip_y[nblips]/F");
    evtTree->Branch("blip_z",blip_z,"blip_z[nblips]/F");
    evtTree->Branch("blip_dx",blip_dx,"blip_dx[nblips]/F");
    evtTree->Branch("blip_dw",blip_dw,"blip_dw[nblips]/F");
    //evtTree->Branch("blip_size",blip_size,"blip_size[nblips]/F");
    evtTree->Branch("blip_charge",blip_charge,"blip_charge[nblips]/I");
    evtTree->Branch("blip_energy",blip_energy,"blip_energy[nblips]/F");
    evtTree->Branch("blip_energyCorr",blip_energyCorr,"blip_energyCorr[nblips]/F");
    evtTree->Branch("blip_isMC",blip_isMC,"blip_isMC[nblips]/O");
    //evtTree->Branch("blip_yzcorr",blip_yzcorr,"blip_yzcorr[nblips]/F");
    //evtTree->Branch("blip_incylinder",blip_incylinder,"blip_incylinder[nblips]/O");
    evtTree->Branch("blip_proxtrkdist",blip_proxtrkdist,"blip_proxtrkdist[nblips]/F");
    if( saveTrkInfo ) evtTree->Branch("blip_proxtrkid",blip_proxtrkid,"blip_proxtrkid[nblips]/I");
    evtTree->Branch("blip_touchtrk",blip_touchtrk,"blip_touchtrk[nblips]/O");
    if( saveTrkInfo ) evtTree->Branch("blip_touchtrkid",blip_touchtrkid,"blip_touchtrkid[nblips]/I");
    if( saveTruthInfo ) evtTree->Branch("blip_edepid",blip_edepid,"blip_edepid[nblips]/I");
    if( saveClustInfo ) {
    for(int i=0;i<kNplanes;i++) evtTree->Branch(Form("blip_pl%i_clustid",i),blip_clustid[i],Form("blip_pl%i_clustid[nblips]/I",i));
    }
    for(int i=0;i<kNplanes;i++) evtTree->Branch(Form("blip_pl%i_nwires",i),blip_nwires[i],Form("blip_pl%i_nwires[nblips]/I",i));
    for(int i=0;i<kNplanes;i++) evtTree->Branch(Form("blip_pl%i_maxamp",i),blip_maxamp[i],Form("blip_pl%i_maxamp[nblips]/F",i));
    //for(int i=0;i<kNplanes;i++) evtTree->Branch(Form("blip_pl%i_minamp",i),blip_minamp[i],Form("blip_pl%i_minamp[nblips]/F",i));


    if( saveTruthInfo ) {
      evtTree->Branch("nparticles",&nparticles,"nparticles/I");
      evtTree->Branch("part_isPrimary",part_isPrimary,"part_isPrimary[nparticles]/O");
      evtTree->Branch("part_trackID",part_trackID,"part_trackID[nparticles]/I");
      evtTree->Branch("part_pdg",part_pdg,"part_pdg[nparticles]/I");
      evtTree->Branch("part_nDaughters",part_nDaughters,"part_nDaughters[nparticles]/I");
      evtTree->Branch("part_mother",part_mother,"part_mother[nparticles]/I");
      evtTree->Branch("part_KE",part_KE,"part_KE[nparticles]/F");
      //evtTree->Branch("part_endKE",part_endKE,"part_endKE[nparticles]/F");
      //evtTree->Branch("part_mass",part_mass,"part_mass[nparticles]/F");
      //evtTree->Branch("part_P",part_P,"part_P[nparticles]/F");
      evtTree->Branch("part_Px",part_Px,"part_Px[nparticles]/F");
      evtTree->Branch("part_Py",part_Py,"part_Py[nparticles]/F");
      evtTree->Branch("part_Pz",part_Pz,"part_Pz[nparticles]/F");
      evtTree->Branch("part_startPointx",part_startPointx,"part_startPointx[nparticles]/F");
      evtTree->Branch("part_startPointy",part_startPointy,"part_startPointy[nparticles]/F");
      evtTree->Branch("part_startPointz",part_startPointz,"part_startPointz[nparticles]/F");
      //evtTree->Branch("part_endPointx",part_endPointx,"part_endPointx[nparticles]/F");
      //evtTree->Branch("part_endPointy",part_endPointy,"part_endPointy[nparticles]/F");
      //evtTree->Branch("part_endPointz",part_endPointz,"part_endPointz[nparticles]/F");
      evtTree->Branch("part_startT",part_startT,"part_startT[nparticles]/F");
      //evtTree->Branch("part_endT",part_endT,"part_endT[nparticles]/F");
      evtTree->Branch("part_pathlen",part_pathlen,"part_pathlen[nparticles]/F");
      evtTree->Branch("part_depEnergy",part_depEnergy,"part_depEnergy[nparticles]/F");
      //evtTree->Branch("part_depElectrons",part_depElectrons,"part_depElectrons[nparticles]/I");
      //evtTree->Branch("part_numElectrons",part_numElectrons,"part_numElectrons[nparticles]/F");
      evtTree->Branch("part_process",&part_process);

      evtTree->Branch("nedeps",&nedeps,"nedeps/I");
      evtTree->Branch("edep_partid",edep_partid,"edep_partid[nedeps]/I");
      evtTree->Branch("edep_g4trkid",edep_g4trkid,"edep_g4trkid[nedeps]/I");
      evtTree->Branch("edep_g4qfrac",edep_g4qfrac,"edep_g4qfrac[nedeps]/F");
      evtTree->Branch("edep_isPrimary",edep_isPrimary,"edep_isPrimary[nedeps]/O");
      evtTree->Branch("edep_pdg",edep_pdg,"edep_pdg[nedeps]/I");
      evtTree->Branch("edep_proc",edep_proc,"edep_proc[nedeps]/I");
      evtTree->Branch("edep_blipid",edep_blipid,"edep_blipid[nedeps]/I");
      evtTree->Branch("edep_energy",edep_energy,"edep_energy[nedeps]/F");
      evtTree->Branch("edep_electrons",edep_electrons,"edep_electrons[nedeps]/I");
      evtTree->Branch("edep_charge",edep_charge,"edep_charge[nedeps]/I");
      evtTree->Branch("edep_tdrift",edep_tdrift,"edep_tdrift[nedeps]/I");
      evtTree->Branch("edep_x",edep_x,"edep_x[nedeps]/F");
      evtTree->Branch("edep_y",edep_y,"edep_y[nedeps]/F");
      evtTree->Branch("edep_z",edep_z,"edep_z[nedeps]/F");
      evtTree->Branch("edep_dx",edep_dx,"edep_dx[nedeps]/F");
      evtTree->Branch("edep_dz",edep_dz,"edep_dz[nedeps]/F");
    }
  }

  void MakeCalibTree(){
    art::ServiceHandle<art::TFileService> tfs;
    calibTree = tfs->make<TTree>("calibtree","ACPT calibration tree");
    calibTree->Branch("run",&run,"run/I");
    calibTree->Branch("subrun",&subrun,"subrun/I");
    calibTree->Branch("timestamp",&timestamp,"timestamp/i");
    //calibTree->Branch("timestamp_hr",&timestamp_hr,"timestamp_hr/i");
    calibTree->Branch("acptrk_theta_xz",&acptrk_theta_xz,"acptrk_theta_xz/F");
    calibTree->Branch("acptrk_theta_yz",&acptrk_theta_yz,"acptrk_theta_yz/F");
    calibTree->Branch("acptrk_qratio_median",&acptrk_qratio_median,"acptrk_qratio_median/F");
    calibTree->Branch("acptrk_qratio_mean",&acptrk_qratio_mean,"acptrk_qratio_mean/F");
    calibTree->Branch("acptrk_npts",&acptrk_npts,"acptrk_npts/I");
    //calibTree->Branch("acptrk_dEdx",&acptrk_dEdx,"acptrk_dEdx[acptrk_npts]/F");
    //calibTree->Branch("acptrk_tdrift",&acptrk_tdrift,"acptrk_tdrift[acptrk_npts]/F");
  }



};//BlipAnaMCTreeDataStruct class



//###################################################
//  BlipAnaMC class definition
//###################################################
class BlipAnaMC : public art::EDAnalyzer
{
  public:
  explicit BlipAnaMC(fhicl::ParameterSet const& pset);
  virtual ~BlipAnaMC();

  //void beginJob();                      // called once, at start of job
  void endJob();                        // called once, at end of job
  void analyze(const art::Event& evt);  // called per event

  private:
  void    PrintParticleInfo(size_t);
  void    PrintTrueBlipInfo(const blipobj::TrueBlip&);
  void    PrintClusterInfo(const blipobj::HitClust&);
  void    PrintHitInfo(const blipobj::HitInfo&);
  void    PrintBlipInfo(const blipobj::Blip&);
  float   Truncate(float, double = 0.1);

  // --- Data and calo objects ---
  BlipAnaMCTreeDataStruct*  fData;
  blip::BlipRecoAlg       fBlipAlg;

  // --- FCL configs ---
  bool                fDebugMode;
  std::string         fHitProducer;
  std::string         fTrkProducer;
  std::string         fGeantProducer;
  std::string         fSimDepProducer;
  std::string              fSignalLabel;
  std::vector<std::string> fBackgroundLabels;
  int                 fCaloPlane;
  std::vector<bool>   fSavePlaneInfo;
  bool                fDoACPTrkCalib;

  bool                fGetRDTimestamp;
  int                 fProtoDUNETrigger;

  std::vector<int>    fPlotBlipMult;
  std::vector<float>  fPlotBlipYBounds;
  std::vector<float>  fPlotBlipZBounds;

  // --- Counters and such ---
  bool  fIsRealData         = false;
  bool  fIsMC               = false;
  int   fNumEvents          = 0;
  int   fNumHits[3]         = {};
  int   fNumHitsUntracked[3]= {};
  int   fNumHitsMatched[3]  = {};
  int   fNumHitsTrue[3]     = {};
  int   fNumHitsMatchedTrue[3]  = {};
  int   fNum3DBlips         = 0;
  int   fNum3DBlips3Plane   = 0;
  int   fNum3DBlipsPicky    = 0;
  int   fNum3DBlipsTrue     = 0;
  int   fNum3DBlipsTrue3P   = 0;

  // --- Histograms ---
  TH1D*   h_triggercodes;

  TH2D*   h_blipE_vs_hitamp[kNplanes];

  TH1D*   h_part_process;
  TH1D*   h_nhits[kNplanes];
  //TH1D*   h_nhits_noise[kNplanes];
  TH1D*   h_nclusts[kNplanes];
  TH1D*   h_nclusts_pm[kNplanes];

  TH1D*   h_hitamp[kNplanes];
  TH1D*   h_hitamp_true[kNplanes];
  TH1D*   h_hitamp_fake[kNplanes];
  TH1D*   h_hitamp_mip[kNplanes];
  TH1D*   h_hitrms[kNplanes];
  TH1D*   h_hitrms_true[kNplanes];
  TH1D*   h_hitrms_fake[kNplanes];
  TH1D*   h_hitrms_mip[kNplanes];
  TH1D*   h_hitratio[kNplanes];
  TH1D*   h_hitgof[kNplanes];
  TH1D*   h_hitgof_true[kNplanes];
  TH1D*   h_hitgof_fake[kNplanes];
  TH1D*   h_hitgof_mip[kNplanes];
  TH1D*   h_hitmult[kNplanes];
  TH1D*   h_hitmult_true[kNplanes];
  TH1D*   h_hitmult_fake[kNplanes];
  TH1D*   h_hitmult_mip[kNplanes];

  TH1D*   h_hitadcdiff[kNplanes];
  TH1D*   h_hitq[kNplanes];
  TH1D*   h_hitqerr[kNplanes];
  TH1D*   h_hitqres[kNplanes];
  TH2D*   h_hitqres_scatter[kNplanes];
  TH2D*   h_hitqres_vs_q[kNplanes];

  TH1D*   h_hitpur[kNplanes];
  TH1D*   h_chargecomp[kNplanes];

  TH1D*   h_trk_length;
  TH1D*   h_trk_xspan;
  TH1D*   h_trk_yspan;
  TH1D*   h_trk_zspan;
  TH2D*   h_trk_length_vs_calopts;

  TH1D*   h_nblips;
  TH1D*   h_nblips_picky;
  TH2D*   h_blip_zy;
  TH2D*   h_blip_zy_picky;
  TH1D*   h_nblips_tm;
  TH1D*   h_blip_nplanes;
  TH1D*   h_blip_qcomp;
  TH2D*   h_blip_reszy;
  TH1D*   h_blip_resx;
  TH1D*   h_blip_resE;
  TH2D*   h_blip_E_vs_resE;
  TH1D*   h_blip_charge;
  TH1D*   h_blip_charge_picky;
  TH2D*   h_blip_charge_YU;
  TH2D*   h_blip_charge_YV;
  TH2D*   h_blip_charge_UV;
  TH2D*   h_blip_charge_YU_picky;
  TH2D*   h_blip_charge_YV_picky;
  TH2D*   h_blip_charge_UV_picky;
  TH1D*   h_clust_qres_anode;
  TH1D*   h_clust_qres_dep;
  TH2D*   h_clust_qres_vs_q;
  TH2D*   h_qratio_vs_time_sim;

  TH1D*   h_ACPtrk_theta_xz;
  TH1D*   h_ACPtrk_theta_yz;
  TH1D*   h_ACPtrk_dEdx;
  TH1D*   h_ACPtrk_dEdx_near;
  TH1D*   h_ACPtrk_dEdx_far;
  TH1D*   h_ACPtrk_qratio;
  TH2D*   h_ACPtrk_yz;

  // === Truth Track ID Sets for Backtracking ===
  std::set<int> ngenG4trkID;
  std::set<int> ar39G4trkID;
  std::set<int> ar42G4trkID;
  std::set<int> kr85G4trkID;
  std::set<int> k42fromar42G4trkID;
  std::set<int> k40cathodeG4trkID;
  std::set<int> th232cathodeG4trkID;
  std::set<int> u238cathodeG4trkID;
  std::set<int> k40anodeG4trkID;
  std::set<int> th232anodeG4trkID;
  std::set<int> u238anodeG4trkID;
  std::set<int> cryostatfoamgammaG4trkID;
  std::set<int> cosmicgenG4trkID;

  art::ServiceHandle<cheat::BackTrackerService> bt_serv;
  art::ServiceHandle<cheat::PhotonBackTrackerService> pbt;


  // Initialize histograms
  void InitializeHistograms(){

    art::ServiceHandle<art::TFileService> tfs;
    art::TFileDirectory dir_diag  = tfs->mkdir("Diagnostics");
    art::TFileDirectory dir_truth = dir_diag.mkdir("Truth");
    art::TFileDirectory dir_hits  = dir_diag.mkdir("HitMetrics");

    h_triggercodes = tfs->make<TH1D>("trig","trig codes",31,-1,30);


    int   blipBins  = fPlotBlipMult[0];
    float blipMax   = fPlotBlipMult[2];
    int   zBins     = (int)fPlotBlipZBounds[0];
    float zMin      = fPlotBlipZBounds[1];
    float zMax      = fPlotBlipZBounds[2];
    int   yBins     = (int)fPlotBlipYBounds[0];
    float yMin      = fPlotBlipYBounds[1];
    float yMax      = fPlotBlipYBounds[2];
    h_nblips        = tfs->make<TH1D>("nblips","Reconstructed 3D blips per event",blipBins,0,blipMax);
    h_nblips_picky  = tfs->make<TH1D>("nblips_picky","Reconstructed 3D blips per event (3-plane match, intersect #Delta < 1 cm)",blipBins,0,blipMax);
    h_blip_zy       = tfs->make<TH2D>("blip_zy","3D blip location;Z [cm];Y [cm]",zBins,zMin,zMax,yBins,yMin,yMax);
    h_blip_zy       ->SetOption("COLZ");
    h_blip_zy_picky = tfs->make<TH2D>("blip_zy_picky","3D blip location;Z [cm];Y [cm]",zBins,zMin,zMax,yBins,yMin,yMax);
    h_blip_zy_picky ->SetOption("COLZ");

    h_trk_length    = dir_diag.make<TH1D>("trk_length",";Track length [cm]",1000,0,500);
    h_trk_xspan     = dir_diag.make<TH1D>("trk_xspan",";Track dx [cm]",200,0,1000);
    h_trk_yspan     = dir_diag.make<TH1D>("trk_yspan",";Track dy [cm]",200,0,1000);
    h_trk_zspan     = dir_diag.make<TH1D>("trk_zspan",";Track dz [cm]",200,0,1000);
    h_trk_length_vs_calopts     = dir_diag.make<TH2D>("trk_len_vs_calopts",";Track length [cm];Calo points",200,0,1000,200,0,2000);
    h_trk_length_vs_calopts->SetOption("colz");

    h_blip_nplanes    = dir_diag.make<TH1D>("blip_nplanes","Matched planes per blip",3,1,4);
    h_blip_charge     = dir_diag.make<TH1D>("blip_charge","3D blips;Charge [e-]",                             200,0,100e3);
    h_blip_charge_picky  = dir_diag.make<TH1D>("blip_charge_picky","3D blips (3-plane match, intersect #Delta < 1 cm);Charge [e-]",200,0,100e3);

    if( fDoACPTrkCalib ) {
      h_ACPtrk_theta_xz   = dir_diag.make<TH1D>("trk_theta_xz","theta_xz",90,0,90);
      h_ACPtrk_theta_yz   = dir_diag.make<TH1D>("trk_theta_yz","theta_yz",90,0,90);
      h_ACPtrk_dEdx       = dir_diag.make<TH1D>("trk_acp_dEdx","Anode-cathode piercing tracks;Trajectory point dE/dx [MeV/cm]",80,0,8);
      h_ACPtrk_dEdx_near  = dir_diag.make<TH1D>("trk_acp_dEdx_near","20-40 cm from anode;dE/dx at x=20-40cm [MeV/cm]",80,0,8);
      h_ACPtrk_dEdx_far   = dir_diag.make<TH1D>("trk_acp_dEdx_far","220-240 cm from anode;dE/dx at x=220-240cm [MeV/cm]",80,0,8);
      h_ACPtrk_qratio     = dir_diag.make<TH1D>("trk_acp_qratio","Attenuation over 2m (using median dE/dx)",200,0,2.0);
      h_ACPtrk_yz         = dir_diag.make<TH2D>("trk_acp_yz",";Z [cm];Y [cm]",1037,0,1037,234,-117,117);
      h_ACPtrk_yz         ->SetOption("colz");
    }


    float qmax = 100;
    int   qbins = 200;
    h_blip_charge_YU = dir_diag.make<TH2D>("blip_charge_YU","3D blips (2-3 planes);Y Charge [#times 10^{3} e-];U Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_YU ->SetOption("COLZ");
    h_blip_charge_YV = dir_diag.make<TH2D>("blip_charge_YV","3D blips (2-3 planes);Y Charge [#times 10^{3} e-];V Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_YV ->SetOption("COLZ");
    h_blip_charge_UV = dir_diag.make<TH2D>("blip_charge_UV","3D blips (2-3 planes);U Charge [#times 10^{3} e-];V Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_UV ->SetOption("COLZ");
    h_blip_charge_YU_picky = dir_diag.make<TH2D>("blip_charge_YU_picky","3D blips (3 planes, #Delta < 1 cm);Y Charge [#times 10^{3} e-];U Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_YU_picky ->SetOption("COLZ");
    h_blip_charge_YV_picky = dir_diag.make<TH2D>("blip_charge_YV_picky","3D blips (3 planes, #Delta < 1 cm);Y Charge [#times 10^{3} e-];V Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_YV_picky ->SetOption("COLZ");
    h_blip_charge_UV_picky = dir_diag.make<TH2D>("blip_charge_UV_picky","3D blips (3 planes, #Delta < 1 cm);U Charge [#times 10^{3} e-];V Charge [#times 10^{3} e-]",qbins,0,qmax,qbins,0,qmax);
    h_blip_charge_UV_picky ->SetOption("COLZ");


    // MC histograms related to truth

    h_part_process    = dir_truth.make<TH1D>("part_process","MCParticle->Process()",5,0,5);
    auto xa = h_part_process->GetXaxis();
      xa->SetBinLabel(1,"primary");
      xa->SetBinLabel(2,"compt");
      xa->SetBinLabel(3,"phot");
      xa->SetBinLabel(4,"conv");
      xa->SetBinLabel(5,"other");

    h_nblips_tm    = dir_truth.make<TH1D>("nblips_tm","Truth-matched 3D blips per event",blipBins,0,blipMax);
    h_blip_qcomp   = dir_truth.make<TH1D>("blip_qcomp","Fraction of true charge (at anode) reconstructed into 3D blips",202,0,1.01);
    h_blip_reszy   = dir_truth.make<TH2D>("blip_res_zy","Blip position resolution;Z_{reco} - Z_{true} [cm];Y_{reco} - Y_{true} [cm]",150,-15,15,150,-15,15);
      h_blip_reszy ->SetOption("colz");
    h_blip_resx    = dir_truth.make<TH1D>("blip_res_x","Blip position resolution;X_{reco} - X_{true} [cm]",200,-10,10);
    h_blip_resE   = dir_truth.make<TH1D>("blip_res_E","Blip energy resolution;(E_{reco} - E_{true})/E_{true} [cm]",200,-1.,1.);
    h_blip_E_vs_resE    = dir_truth.make<TH2D>("blip_res_energy","Energy resolution of 3D blips;Energy [MeV];#deltaE/E_{true}",100,0,5,200,-1.,1.);
        h_blip_E_vs_resE ->SetOption("colz");
    h_clust_qres_vs_q       = dir_truth.make<TH2D>("qres_vs_q","Clusters on collection plane;True charge deposited [ #times 10^{3} e- ];Reco resolution",160,0,80,200,-1,1);
      h_clust_qres_vs_q     ->SetOption("colz");
    h_clust_qres_anode      = dir_truth.make<TH1D>("qres_anode","Reco charge vs true charge collected;( reco-true ) / true;Area-normalized entries",200,-1.,1.);
    h_clust_qres_dep        = dir_truth.make<TH1D>("qres_dep","Reco charge vs true charge deposited;( reco-true ) / true;Area-normalized entries",200,-1.,1.);
    h_qratio_vs_time_sim  = dir_truth.make<TH2D>("qratio_vs_time_sim",";Drift time [#mus]; Q_{anode} / Q_{dep}",58,100,3000, 500,0.50,1.50);
    h_qratio_vs_time_sim  ->SetOption("colz");


    float hitMax  = 30000;  int hitBins = 300;
    float ampMax  = 40;    int ampBins = 200;
    float rmsMax  = 10;     int rmsBins = 200;
    float qMax    = 100e3; int qBins   = 200;
    float multMax = 15;    int multBins = 15;
    float gofMin  = -10;  float gofMax = 10; int gofBins = 200;
    for(int i=kNplanes-1; i >= 0; i--) {

      h_blipE_vs_hitamp[i] = dir_diag.make<TH2D>(
        Form("pl%i_blipE_vs_hitamp",i),
        Form("Plane %i;Blip reco energy [MeVee];Hit amplitude [ADC]",i),
        100,0,10,
        100,0,20
      );
      h_blipE_vs_hitamp[i]->SetOption("colz");

      h_nhits[i]        = dir_hits.make<TH1D>(Form("pl%i_nhits",i),  Form("Plane %i;total number of hits",i),hitBins,0,hitMax);
      //h_nhits_noise[i]  = dir_hits.make<TH1D>(Form("pl%i_nhits_noise",i),  Form("Plane %i (non-truth-matched);total number of hits",i),hitBins,0,hitMax);

      h_hitamp[i]       = dir_hits.make<TH1D>(Form("pl%i_hit_amp",i),       Form("Plane %i untracked hits;hit amplitude [ADC]",i),ampBins,0,ampMax);
      h_hitamp_true[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_amp_true",i),  Form("Plane %i untracked hits, truth-matched;hit amplitude [ADC]",i),ampBins,0,ampMax);
      h_hitamp_fake[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_amp_fake",i),  Form("Plane %i untracked hits, non-truth-matched (noise);hit amplitude [ADC]",i),ampBins,0,ampMax);
      h_hitamp_mip[i]   = dir_hits.make<TH1D>(Form("pl%i_hit_amp_mip",i),   Form("Plane %i tracked hits (L>20cm);hit amplitude [ADC]",i),ampBins,0,ampMax);
      h_hitrms[i]       = dir_hits.make<TH1D>(Form("pl%i_hit_rms",i),       Form("Plane %i untracked hits;RMS [ADC time-tick]",i),rmsBins,0,rmsMax);
      h_hitrms_true[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_rms_true",i),  Form("Plane %i untracked hits, truth-matched;RMS [ADC time-tick]",i),rmsBins,0,rmsMax);
      h_hitrms_fake[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_rms_fake",i),  Form("Plane %i untracked hits, non-truth-matched (noise);RMS [ADC time-tick]",i),rmsBins,0,rmsMax);
      h_hitrms_mip[i]   = dir_hits.make<TH1D>(Form("pl%i_hit_rms_mip",i),   Form("Plane %i tracked hits (L>20cm);RMS [ADC time-tick]",i),rmsBins,0,rmsMax);

      h_hitratio[i]    = dir_hits.make<TH1D>(Form("pl%i_hit_ratio",i),   Form("Plane %i untracked hits;RMS/amplitude",i),200,0,10);

      h_hitgof[i]       = dir_hits.make<TH1D>(Form("pl%i_hit_gof",i),       Form("Plane %i untracked hits;log_{10}(GOF/ndf)",i),gofBins,gofMin,gofMax);
      h_hitgof_true[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_gof_true",i),  Form("Plane %i untracked hits, truth-matched;log_{10}(GOF/ndf)",i),gofBins,gofMin,gofMax);
      h_hitgof_fake[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_gof_fake",i),  Form("Plane %i untracked hits, non-truth-matched (noise);log_{10}(GOF/ndf)",i),gofBins,gofMin,gofMax);
      h_hitgof_mip[i]   = dir_hits.make<TH1D>(Form("pl%i_hit_gof_mip",i),   Form("Plane %i tracked hits (L>20cm);log_{10}(GOF/ndf)",i),gofBins,gofMin,gofMax);
      h_hitmult[i]      = dir_hits.make<TH1D>(Form("pl%i_hit_mult",i),       Form("Plane %i untracked hits;fit multiplicity",i),multBins,0,multMax);
      h_hitmult_true[i] = dir_hits.make<TH1D>(Form("pl%i_hit_mult_true",i),  Form("Plane %i untracked hits, truth-matched;fit multiplicity",i),multBins,0,multMax);
      h_hitmult_fake[i] = dir_hits.make<TH1D>(Form("pl%i_hit_mult_fake",i),  Form("Plane %i untracked hits, non-truth-matched (noise);fit multiplicity",i),multBins,0,multMax);
      h_hitmult_mip[i]  = dir_hits.make<TH1D>(Form("pl%i_hit_mult_mip",i),   Form("Plane %i tracked hits (L>20cm);fit multiplicity",i),multBins,0,multMax);

      h_hitadcdiff[i]   = dir_hits.make<TH1D>(Form("pl%i_hit_adcdiff",i),   Form("Plane %i hits;(Integral - SumADC) / SumADC",i), 400, -2, 2);
      h_hitq[i]         = dir_hits.make<TH1D>(Form("pl%i_hit_q",i),         Form("Plane %i hits;hit charge [#times 10^{3} e-]",i), qBins,0,qMax/1e3);

      h_hitqerr[i]      = dir_truth.make<TH1D>(Form("pl%i_hit_q_err",i),     Form("Plane %i hits;charge uncertainty from fit: (#sigma/q)",i),400,-1,1);
      h_hitqres[i]      = dir_truth.make<TH1D>(Form("pl%i_hit_q_res",i),     Form("Plane %i hits;charge resolution: (reco-true)/true",i),    400,-1,1);
      h_hitqres_scatter[i] = dir_truth.make<TH2D>( Form("pl%i_hit_qres_scatter",i),
        Form("Plane %i;true hit charge [#times 10^{3} e-];Reconstructed hit charge [#times 10^{3} e-]",i),qBins,0,qMax/1e3,qBins,0,qMax/1e3);
        h_hitqres_scatter[i]  ->SetOption("colz");
      h_hitqres_vs_q[i] = dir_truth.make<TH2D>( Form("pl%i_hit_qres_vs_q",i),
        Form("Plane %i;true hit charge [#times 10^{3} e-];hit charge resolution: (reco-true)/true",i),qBins,0,qMax/1e3, 400,-2,2);
        h_hitqres_vs_q[i]     ->SetOption("colz");

      h_chargecomp[i] = dir_truth.make<TH1D>(Form("pl%i_hit_charge_completeness",i),Form("charge completness, plane %i",i),101,0,1.01);
      h_hitpur[i]     = dir_truth.make<TH1D>(Form("pl%i_hit_purity",i),Form("hit purity, plane %i",i),101,0,1.01);

      h_nclusts[i]    = dir_diag.make<TH1D>(Form("pl%i_nclusts",i),Form("nclusts, plane %i",i),1000,0,5000);
      h_nclusts_pm[i] = dir_diag.make<TH1D>(Form("pl%i_nclusts_planematched",i),Form("nclusts plane matched, plane %i",i),1000,0,2000);


    }//endloop over planes

 }

};//class BlipAnaMC


//###################################################
//  BlipAnaMC constructor and destructor
//###################################################
BlipAnaMC::BlipAnaMC(fhicl::ParameterSet const& pset) :
  EDAnalyzer(pset)
  ,fData  (nullptr)
  ,fBlipAlg ( pset.get<fhicl::ParameterSet>("BlipAlg") )
{
  // blip reconstruction algorithm class
  fhicl::ParameterSet pset_blipalg = pset.get<fhicl::ParameterSet>("BlipAlg");
  fHitProducer    = pset_blipalg.get<std::string>   ("HitProducer",     "gaushit");
  fTrkProducer    = pset_blipalg.get<std::string>   ("TrkProducer",     "pandora");
  fGeantProducer  = pset_blipalg.get<std::string>   ("GeantProducer",   "simplemerge");
  fSimDepProducer = pset_blipalg.get<std::string>   ("SimEDepProducer", "ionization");
  fCaloPlane      = pset_blipalg.get<int>           ("CaloPlane",       2);
  fSavePlaneInfo  = pset.get<std::vector<bool>>     ("SavePlaneInfo",   {true,true,true});
  fDebugMode      = pset.get<bool>                  ("DebugMode",       false);
  fDoACPTrkCalib  = pset.get<bool>                  ("DoACPTrkCalib",   true);

  fGetRDTimestamp   = pset.get<bool>                ("GetRDTimestamp",   false);
  fProtoDUNETrigger = pset.get<int>                 ("ProtoDUNETrigger", 1);

  fPlotBlipMult     = pset.get<std::vector<int>>    ("PlotBlipMult",    {150,0,300});
  fPlotBlipYBounds  = pset.get<std::vector<float>>  ("PlotBlipYBounds", {160,-400,400});
  fPlotBlipZBounds  = pset.get<std::vector<float>>  ("PlotBlipZBounds", {100,-100,400});

  fSignalLabel = pset.get<std::string>("SignalLabel");
  fBackgroundLabels = pset.get<std::vector<std::string>>("BackgroundLabelVector");

  //fSignalLabel = pset.get<std::string>("SignalLabel", "ngen");
  // Explicitly configuring the background labels
  /*fBackgroundLabels = pset.get<std::vector<std::string>>("BackgroundLabels", {
  "ar39", "ar42", "kr85", "k42fromar42",
    "k40cathode", "th232cathode", "u238cathode",
    "k40anode", "th232anode", "u238anode",
    "cryostatfoamgamma", "cosmicgen"
  });*/

  // data tree object
  fData = new BlipAnaMCTreeDataStruct();
  fData ->treeName        = pset.get<std::string> ("EventTreeName", "anatree");
  fData ->saveTruthInfo   = pset.get<bool>        ("SaveTruthInfo", true);
  fData ->saveTrkInfo     = pset.get<bool>        ("SaveTrkInfo",   true);
  fData ->saveHitInfo     = pset.get<bool>        ("SaveHitInfo",   true);
  fData ->saveClustInfo   = pset.get<bool>        ("SaveClustInfo", true);
  fData ->Clear();
  fData ->MakeTree();

  if( fDoACPTrkCalib ) fData->MakeCalibTree();

  // initialize histograms
  InitializeHistograms();

}
BlipAnaMC::~BlipAnaMC(){}



//###################################################
//  Main event-by-event analysis
//###################################################
void BlipAnaMC::analyze(const art::Event& evt)
{
  // New event!
  //============================================
  fData            ->Clear();
  fData->event      = evt.id().event();
  fData->run        = evt.id().run();
  fData->subrun     = evt.id().subRun();
  fIsRealData       = evt.isRealData();
  fNumEvents++;

  // Clear previous event sets
  ngenG4trkID.clear();
  ar39G4trkID.clear();          ar42G4trkID.clear();          kr85G4trkID.clear();
  k42fromar42G4trkID.clear();   k40cathodeG4trkID.clear();    th232cathodeG4trkID.clear();
  u238cathodeG4trkID.clear();   k40anodeG4trkID.clear();      th232anodeG4trkID.clear();
  u238anodeG4trkID.clear();     cryostatfoamgammaG4trkID.clear(); cosmicgenG4trkID.clear();

  // Get timestamp
  unsigned long long int tsval = evt.time().value();
  const unsigned long int mask32 = 0xFFFFFFFFUL;
  fData->timestamp = ( tsval >> 32 ) & mask32;

  // Retrieve lifetime
  //const lariov::UBElectronLifetimeProvider& elifetime_provider
  //  = art::ServiceHandle<lariov::UBElectronLifetimeService>()->GetProvider();
  //float electronLifetime = elifetime_provider.Lifetime() * /*convert ms->mus*/ 1e3;

  //auto const detProp    = art::ServiceHandle<detinfo::DetectorPropertiesService const>()->DataForJob();
  auto const detProp    = art::ServiceHandle<detinfo::DetectorPropertiesService>()->DataFor(evt);

  fData->lifetime = detProp.ElectronLifetime(); // [us]

  // Tell us what's going on!
  //if( fNumEvents < 200 || (fNumEvents % 100) == 0 ) {
  std::cout<<"\n"
  <<"=========== BlipAnaMC =========================\n"
  <<"Event "<<evt.id().event()<<" / run "<<evt.id().run()<<"; total events processed: "<<fNumEvents<<"\n";
  std::cout<<"Lifetime "<<fData->lifetime<<"\n";
  //}


  /*
  art::ServiceHandle<calib::LifetimeCalibService> lifetimecalibHandler;
  calib::LifetimeCalibService & lifetimecalibService = *lifetimecalibHandler;
  calib::LifetimeCalib *lifetimecalib = lifetimecalibService.provider();
  double lifetime;
  if (evt.isRealData()) {
    lifetime = lifetimecalib->GetLifetime()*1000.0; // [ms]*1000.0 -> [us]
    std::cout << "\nuse lifetime from database(run " << evt.id().run()<< ")   " << lifetime<< "\n";
  } else {
    lifetime= detProp.ElectronLifetime();
    std::cout << "\nuse lifetime from DetectorProperties(run " << evt.id().run()<< ")   " << lifetime << "\n\n";
  }
  fData->lifetime = lifetime;
  */

  //============================================
  // Run blip reconstruction:
  //============================================
  fBlipAlg.RunBlipReco(evt);
  if( !fBlipAlg.hitinfo.size() ) return;

  //
  //  In the above step, we pass the entire art::Event to the algorithm,
  //  and it creates a single collection of blip 'objects', a special data
  //  struct in the 'blip' namespace defined in BlipUtils.h.
  //
  //  We can then retrieve these blips and incorporate them into
  //  our analysis however we like:
  //
  //    std::vector<blipobj::Blip> blipVec = fBlipAlg.blips;
  //
  //  The alg also creates collections of 'HitInfo' and 'HitClust'
  //  structs used in the blip reconstruction process, which can be
  //  accessed in the same way as blips.
  //
  //    * HitInfo simply saves some calculations for each hit that aren't
  //      present in the native recob::Hit object, like drift time, associated
  //      G4 particle IDs, etc.
  //
  //    * HitClust is just a cluster of hits on a specific plane; these are
  //      used to create 3D blips by plane-matching.
  //


  //=======================================
  // Get ProtoDUNE trigger
  //=======================================
  if( fGetRDTimestamp ) {
    try {
      art::ValidHandle<std::vector<raw::RDTimeStamp>> timeStamps
        = evt.getValidHandle<std::vector<raw::RDTimeStamp>>("timingrawdecoder:daq");
      // Check that we have good information
      if(timeStamps.isValid() && timeStamps->size() == 1){
        // Access the trigger information. Beam trigger flag = 0xc
        const raw::RDTimeStamp& timeStamp = timeStamps->at(0);
        fData->trigcode = timeStamp.GetFlags();
        h_triggercodes->Fill(fData->trigcode);
      }
      std::cout<<"Trigger  "<<fData->trigcode<<"\n";
    }
    catch (...) {
    }
  }



  //=======================================
  // Get data products for this event
  //========================================

  // -- G4 particles
  art::Handle< std::vector<simb::MCParticle> > pHandle;
  std::vector<art::Ptr<simb::MCParticle> > plist;
  if (evt.getByLabel(fGeantProducer,pHandle)) // here need to be consistent to what supplies to blipreco: i.e., fGeantProducer configured in fcl, not largeant
    art::fill_ptr_vector(plist, pHandle);

  // -- hits (from input module)
  art::Handle< std::vector<recob::Hit> > hitHandle;
  std::vector<art::Ptr<recob::Hit> > hitlist;
  if (evt.getByLabel(fHitProducer,hitHandle))
    art::fill_ptr_vector(hitlist, hitHandle);

  // -- optical hits
  art::Handle< std::vector<recob::OpHit> > opHitHandle;
  std::vector<art::Ptr<recob::OpHit> > ophitlist;
  if (evt.getByLabel("ophit", opHitHandle))
    art::fill_ptr_vector(ophitlist, opHitHandle);

  // -- tracks
  auto tracklistHandle = evt.getHandle<std::vector<recob::Track>>(fTrkProducer);
  std::vector<art::Ptr<recob::Track> > tracklist;
  if (tracklistHandle) art::fill_ptr_vector(tracklist, tracklistHandle);
  art::FindManyP<recob::Hit> fmtht(tracklistHandle, evt, fTrkProducer);

  // Resize data struct objects
  fData->nhits      = (int)hitlist.size();
  fData->nophits    = std::min((int)ophitlist.size(), kMaxOpHits);
  fData->nparticles = (int)plist.size();
  fData->ntrks      = (int)tracklist.size();
  fData->badchans   = fBlipAlg.EvtBadChanCount;
  fData->Resize();



  // flag this data as MC
  fIsMC = ( plist.size()>0 );

  std::cout<<" - found "<<hitlist.size()<<" hits from "<<fHitProducer<<"\n";
  std::cout<<" - found "<<ophitlist.size()<<" OpHits from ophit\n";
  std::cout<<" - found "<<tracklist.size()<<" tracks from "<<fTrkProducer<<"\n";
  std::cout<<" - found "<<plist.size()<<" MCParticles\n";
  std::cout<<" - reconstructed "<<fBlipAlg.blips.size()<<" 3D blips\n";

  //====================================
  // Keep tabs on total energy, charge,
  // and electrons arriving at anode
  //====================================
  float total_depEnergy         = 0;
  float total_depElectrons      = 0;
  float total_numElectrons      = 0;

  //====================================
  // Save MCParticle information
  //====================================
  std::map<int,int> map_g4trkid_index;
  if( plist.size() ) {

    std::vector<blipobj::ParticleInfo>& pinfo = fBlipAlg.pinfo;

    // Loop through the MCParticles
    if( fDebugMode ) std::cout<<"\nLooping over G4 MCParticles: \n";
    for(size_t i = 0; i<plist.size(); i++){
      auto& pPart = plist[i];
      map_g4trkid_index[pPart->TrackId()] = i;
      total_depEnergy       += pinfo[i].depEnergy;
      total_depElectrons    += pinfo[i].depElectrons;

      // Save to TTree object
      if(i<kMaxG4){
        fData->part_trackID[i]         = pPart->TrackId();
        fData->part_pdg[i]             = pPart->PdgCode();
        fData->part_nDaughters[i]      = pPart->NumberDaughters();
        fData->part_mother[i]          = pPart->Mother();
        fData->part_E[i]               = pinfo[i].E;
        fData->part_endE[i]            = pinfo[i].endE;
        fData->part_mass[i]            = pinfo[i].mass;
        fData->part_KE[i]              = pinfo[i].KE;
        fData->part_endKE[i]           = pinfo[i].endKE;
        fData->part_P[i]               = pinfo[i].P;
        fData->part_Px[i]              = pinfo[i].Px;
        fData->part_Py[i]              = pinfo[i].Py;
        fData->part_Pz[i]              = pinfo[i].Pz;
        fData->part_startPointx[i]     = pPart->Vx();
        fData->part_startPointy[i]     = pPart->Vy();
        fData->part_startPointz[i]     = pPart->Vz();
        fData->part_endPointx[i]       = pPart->EndPosition()[0];
        fData->part_endPointy[i]       = pPart->EndPosition()[1];
        fData->part_endPointz[i]       = pPart->EndPosition()[2];
        fData->part_startT[i]          = pinfo[i].time;
        fData->part_endT[i]            = pinfo[i].endtime;
        fData->part_pathlen[i]         = pinfo[i].pathLength;
        fData->part_numTrajPts[i]      = pinfo[i].numTrajPts;
        fData->part_process[i]         = pPart->Process();
        fData->part_depEnergy[i]       = pinfo[i].depEnergy;
        fData->part_depElectrons[i]    = pinfo[i].depElectrons;
        fData->part_isPrimary[i]       = pinfo[i].isPrimary;
        if( fDebugMode ) PrintParticleInfo(i);
      }
    } // endloop over G4 particles

    if( fDebugMode ) std::cout<<"True total energy deposited: "<<total_depEnergy<<" MeV \n";

  }//endif particles found in event

  // === UPDATED: Populate Truth Track ID Sets for Backtracking ===

  // Fill Signal IDs from ngen
  art::Handle<std::vector<simb::MCTruth>> SignalHandle;
  if (evt.getByLabel(fSignalLabel, SignalHandle) && SignalHandle.isValid()) { // fSignalLabel configured as "ngen"
    art::FindManyP<simb::MCParticle> SignalAssn(SignalHandle, evt, "largeant"); // here only largeant is available for assn, it is the primary collection with secondary particles dropped
    for (size_t i = 0; i < SignalAssn.size(); i++) {
      auto SignalParticles = SignalAssn.at(i);
      for (auto const& part : SignalParticles) {
        ngenG4trkID.emplace(part->TrackId()); // largeant drops low-energy EM shower particles or neutral particles from the final output list to keep file sizes small, that specific ID doesn't exist in the event's MCParticle list anymore.
      }
    }
  }

  // Fill Consolidated Background IDs matching your new specific tracking sets
  for (auto const& label : fBackgroundLabels) {
    art::Handle<std::vector<simb::MCTruth>> BkgHandle;
    if (!evt.getByLabel(label, BkgHandle) || !BkgHandle.isValid()) continue;

    art::FindManyP<simb::MCParticle> BkgAssn(BkgHandle, evt,  "largeant");
    for (size_t i = 0; i < BkgAssn.size(); i++) {
      auto BkgParticles = BkgAssn.at(i);
      for (auto const& part : BkgParticles) {
        int trkId = part->TrackId();
        if (label == "ar39")                 ar39G4trkID.emplace(trkId);
        else if (label == "ar42")            ar42G4trkID.emplace(trkId);
        else if (label == "kr85")            kr85G4trkID.emplace(trkId);
        else if (label == "k42fromar42")     k42fromar42G4trkID.emplace(trkId);
        else if (label == "k40cathode")      k40cathodeG4trkID.emplace(trkId);
        else if (label == "th232cathode")    th232cathodeG4trkID.emplace(trkId);
        else if (label == "u238cathode")     u238cathodeG4trkID.emplace(trkId);
        else if (label == "k40anode")        k40anodeG4trkID.emplace(trkId);
        else if (label == "th232anode")      th232anodeG4trkID.emplace(trkId);
        else if (label == "u238anode")       u238anodeG4trkID.emplace(trkId);
        else if (label == "cryostatfoamgamma") cryostatfoamgammaG4trkID.emplace(trkId);
        else if (label == "cosmicgen")       cosmicgenG4trkID.emplace(trkId);
      }
    }
  }

  //====================================
  // Save TrueBlip information
  //====================================
  std::vector<blipobj::TrueBlip>& trueblips = fBlipAlg.trueblips;
  fData->nedeps = (int)trueblips.size();
  if( trueblips.size() ) {
    if( fDebugMode ) std::cout<<"\nLooping over true blips:\n";
    for(auto& trueblip : trueblips ) {
      int i     = trueblip.ID;
      int ig4   = trueblip.LeadG4Index;
      auto& pPart = plist[ig4];
      fData->edep_tpc[i]      = trueblip.TPC;
      fData->edep_energy[i]   = trueblip.Energy;
      fData->edep_electrons[i]= trueblip.DepElectrons;
      fData->edep_charge[i]   = trueblip.NumElectrons;
      fData->edep_x[i]        = trueblip.Position.X();
      fData->edep_y[i]        = trueblip.Position.Y();
      fData->edep_z[i]        = trueblip.Position.Z();
      fData->edep_tdrift[i]   = trueblip.DriftTime;
      fData->edep_pdg[i]      = trueblip.LeadG4PDG;
      fData->edep_g4trkid[i]  = trueblip.LeadG4ID;
      fData->edep_partid[i]   = trueblip.LeadG4Index;
      fData->edep_g4qfrac[i]  = trueblip.G4ChargeMap[trueblip.LeadG4ID] / trueblip.DepElectrons;
      fData->edep_isPrimary[i]= (pPart->Process() == "primary");
      fData->edep_dz[i]       = fabs(pPart->EndPosition()[2]-pPart->Vz());
      fData->edep_dx[i]       = fabs(pPart->EndPosition()[0]-pPart->Vx());

      int proc_code = -9;
      std::string proc = fData->part_process[ig4];
      if(     proc == "primary") { h_part_process->Fill("primary",1); proc_code = 0; }
      else if(proc == "compt"  ) { h_part_process->Fill("compt",  1); proc_code = 1; }
      else if(proc == "phot"   ) { h_part_process->Fill("phot",   1); proc_code = 2; }
      else if(proc == "conv"   ) { h_part_process->Fill("conv",   1); proc_code = 3; }
      else                       { h_part_process->Fill("other",  1); proc_code = 4; }
      fData->edep_proc[i] = proc_code;
      float ne      = trueblip.NumElectrons;
      total_numElectrons += ne;
      if( fDebugMode ) PrintTrueBlipInfo(trueblip);
    }
  }//endif trueblips were made


  //====================================
  // Save track information
  //====================================
  //std::cout<<"Looping over tracks...\n";
  std::map<int,int> map_trkid_index;
  std::map<int,bool> map_trkid_isMIP;
  std::map<int,float> map_trkid_length;
  std::vector<int> flaggedtrks;
  fData->longtrks=0;
  for(size_t i=0; i<tracklist.size(); i++){
    auto& trk = tracklist[i];
    const auto& startPt = trk->Vertex();
    const auto& endPt   = trk->End();
    map_trkid_index[trk->ID()] = i;
    h_trk_length  ->Fill(trk->Length());

    float xspan = fabs( startPt.X()-endPt.X() );
    float yspan = fabs( startPt.Y()-endPt.Y() );
    float zspan = fabs( startPt.Z()-endPt.Z() );
    h_trk_xspan   ->Fill( xspan );
    h_trk_yspan   ->Fill( yspan );
    h_trk_zspan   ->Fill( zspan );



    // set the required 'dX' that would indicate a
    // track crossed the full drift distance
    float dx_min = 350;
    float dx_max = 370;
    if( xspan > dx_min && xspan < dx_max ) flaggedtrks.push_back(i);

    map_trkid_length[trk->ID()] = trk->Length();
    map_trkid_isMIP[trk->ID()]  = (trk->Length()>100) ? true : false;

    fData->trk_id[i]    = trk->ID();
    fData->trk_npts[i]  = trk->NumberTrajectoryPoints();
    fData->trk_length[i]= trk->Length();
    fData->trk_extent[i]= sqrt( pow(xspan,2) + pow(yspan,2) + pow(zspan,2) );
    fData->trk_startx[i]= startPt.X();
    fData->trk_starty[i]= startPt.Y();
    fData->trk_startz[i]= startPt.Z();
    fData->trk_endx[i]  = endPt.X();
    fData->trk_endy[i]  = endPt.Y();
    fData->trk_endz[i]  = endPt.Z();
    fData->trk_startd[i]= BlipUtils::DistToBoundary(startPt);
    fData->trk_endd[i]  = BlipUtils::DistToBoundary(endPt);
    float trk_time = -999.;
    if( fmtht.isValid() ) {
      std::vector<art::Ptr<recob::Hit>> allHits = fmtht.at(i);
      if( !allHits.empty() ) {
        trk_time = allHits.front()->PeakTime();
        for(const auto& hit : allHits) trk_time = std::min(trk_time, (float)hit->PeakTime());
      }
    }
    fData->track_time[i] = trk_time;
    // count the number of non-blippy tracks to use
    // as a metric for cosmic activity in event
    if( trk->Length() > 5 ) fData->longtrks++;

  }//endloop over trks

  //====================================
  // Save photon detector information
  //====================================

  //print photon detector locations
  /*
  auto const& geom = *art::ServiceHandle<geo::Geometry const>();

  std::cout << "\n===== OpDet centers only =====\n";
  for (unsigned int opdet = 0; opdet < geom.NOpDets(); ++opdet) {
    auto const& pdGeo = geom.OpDetGeoFromOpDet(opdet);
    auto const c = pdGeo.GetCenter();

    std::cout << "opDet = " << opdet
	      << "  center = ("
	      << c.X() << ", " << c.Y() << ", " << c.Z() << ") cm\n";
  }

  */


  ///////////////////////////////

  try {
    auto const& waveforms = *evt.getValidHandle<std::vector<raw::OpDetWaveform>>("opdigi");
    if( fGetRDTimestamp ) {
       try {
        auto timeStamps = evt.getValidHandle<std::vector<raw::RDTimeStamp>>("tpcrawdecoder:daq");
        if(timeStamps.isValid() && !timeStamps->empty()) {
          fData->t_offset = -9999;
        }
      } catch (...) {}
    }

    std::vector<double> wfsb1, basev, wfsb2, wfsb3;
    std::vector<double> peakIndices, peakValues, widths, integrals;
    std::vector<int> leftFound, rightFound;

    /* ULong64_t min_wf_time = std::numeric_limits<ULong64_t>::max();
    ULong64_t max_wf_time = 0;
    size_t n_selected_waveforms = 0;
    for (auto const& in_wave : waveforms) {
      if(in_wave.ChannelNumber() != 2020) continue;
      std::cout<<"timestamps "<<in_wave.TimeStamp()<<std::endl;
      const ULong64_t wf_time = in_wave.TimeStamp();
      min_wf_time = std::min(min_wf_time, wf_time);
      max_wf_time = std::max(max_wf_time, wf_time);
      ++n_selected_waveforms;
    }

    if(n_selected_waveforms > 0) {
      fData->pds_timestamp_min = min_wf_time;
      fData->pds_timestamp_max = max_wf_time;
      std::cout << " OpDetWaveform TimeStamp min = " << min_wf_time
                << ", max = " << max_wf_time
                << ", range = " << (max_wf_time - min_wf_time)
                << " ticks/counts =  " << n_selected_waveforms<<std::endl;
    }
    */
    for (auto const& in_wave : waveforms) {
      if(in_wave.ChannelNumber() < 1009 || in_wave.ChannelNumber() > 3000) continue;
      if(in_wave.ChannelNumber() == 1010) std::cout<<"channel size "<<in_wave.size()<<std::endl;
      double raw_wf_time = in_wave.TimeStamp();
      // const ULong64_t shifted_wf_time = raw_wf_time;
      double toff_set=raw_wf_time;
      // std::cout<<"toff_set "<<toff_set<<std::endl;
      //	if(toff_set>100000) continue;
      fData->DAQch_ID.push_back(in_wave.ChannelNumber());
      fData->waveformsADC.push_back(std::vector<float>());
      fData->pds_timestamp_raw.push_back(raw_wf_time);
      fData->pds_timestamp.push_back(raw_wf_time);

      std::vector<float> rawWave;
      std::vector<float> decWave;
      rawWave.reserve(in_wave.size());
      decWave.reserve(in_wave.size());
      wfsb1.reserve(in_wave.size());

      for(size_t is = 0; is < in_wave.size(); ++is) {
        rawWave.push_back((float)in_wave[is]);
        wfsb1.push_back((double)in_wave[is]);
      }

      basev = EstimateBaselineOpeningCentered(wfsb1, 900, 4);
      for(size_t is = 0; is < in_wave.size(); ++is) {
        wfsb2.push_back((double)in_wave[is] - basev.at(is));
      }
      double base = median_func(wfsb2, 0.1);
      for(size_t is = 0; is < in_wave.size(); ++is) {
        wfsb3.push_back(wfsb2[is] - base);
        decWave.push_back((float)wfsb3.back());
	// fData->waveformsADC.back().push_back((float)wfsb3.back());
      }
      double thres=GetPDSAmpThreshold(int(in_wave.ChannelNumber()));
      thres=10;
      findPeaks(wfsb3, thres, 10, peakIndices, peakValues, widths, integrals, leftFound, rightFound);

      fData->peakPDS.push_back(peakValues);
      std::vector<double> peakIndicesShifted(peakIndices.size());
      std::transform(peakIndices.begin(), peakIndices.end(),
		     peakIndicesShifted.begin(),
		     [toff_set](double x) { return 16.0/1000*x + toff_set; });

      fData->tPDS.push_back(peakIndicesShifted);

      // fData->tPDS.push_back(peakIndices);


      fData->intPDS.push_back(integrals);
      fData->widthPDS.push_back(widths);
      fData->leftPDS.push_back(leftFound);
      fData->rightPDS.push_back(rightFound);

      fData->channelraw.push_back((float)in_wave.ChannelNumber());
      // fData->adcraw.push_back(rawWave);
      // fData->channeldec.push_back((float)in_wave.ChannelNumber());
      // fData->adcdec.push_back(decWave);

      for(size_t ip = 0; ip < peakValues.size(); ++ip) {
        fData->PeakPDS.push_back(peakValues[ip]);
        fData->TPDS.push_back(peakIndicesShifted[ip]);
        fData->WidthPDS.push_back(widths[ip]);
        fData->YPDS.push_back(-999.);
        fData->ZPDS.push_back(-999.);
        fData->Maxfraction.push_back(-999.);
      }

      wfsb1.clear();
      basev.clear();
      wfsb2.clear();
      wfsb3.clear();
      peakIndices.clear();
      peakValues.clear();
      widths.clear();
      integrals.clear();
      leftFound.clear();
      rightFound.clear();
    }
  } catch (std::exception const& e) {
    std::cout << " Problem while reading OpDetWaveform collection opdigi: " << e.what() << std::endl;
  } catch (...) {
    std::cout << " Unknown problem while reading OpDetWaveform collection opdigi" << std::endl;
  }


  //====================================
  // Save optical hit information
  //====================================
  if( ophitlist.empty() ) {
    std::cout << " No OpHits found from label ophit" << std::endl;
  } else {
    double min_ophit_time = std::numeric_limits<double>::max();
    double max_ophit_time = -std::numeric_limits<double>::max();
    for(auto const& ophit : ophitlist) {
      const double t = ophit->PeakTime();
      min_ophit_time = std::min(min_ophit_time, t);
      max_ophit_time = std::max(max_ophit_time, t);
    }

    fData->ophit_min_time = min_ophit_time;
    fData->ophit_max_time = max_ophit_time;
    std::cout << " OpHit PeakTime min = " << min_ophit_time
              << ", max = " << max_ophit_time
              << ", range = " << (max_ophit_time - min_ophit_time)
              << std::endl;

    if( ophitlist.size() > kMaxOpHits ) {
      std::cout << " WARNING: OpHit count " << ophitlist.size()
                << " exceeds kMaxOpHits = " << kMaxOpHits
                << "; only the first " << kMaxOpHits
                << " OpHits will be written to the tree." << std::endl;
    }

    for(size_t i=0; i<ophitlist.size() && i<kMaxOpHits; ++i) {
      auto const& ophit = ophitlist[i];
      const bool has_start_time = ophit->HasStartTime();

      fData->ophit_opchannel[i]           = ophit->OpChannel();
      fData->ophit_frame[i]               = (int)ophit->Frame();
      fData->ophit_peak_time[i]           = ophit->PeakTime();
      fData->ophit_peak_time_abs[i]       = ophit->PeakTimeAbs();
      fData->ophit_time[i]                = ophit->PeakTime() - min_ophit_time;
      fData->ophit_has_start_time[i]      = has_start_time;
      fData->ophit_start_time[i]          = has_start_time ? ophit->StartTime() : -999.;
      fData->ophit_start_time_shifted[i]  = has_start_time ? ophit->StartTime() - min_ophit_time : -999.;
      fData->ophit_rise_time[i]           = has_start_time ? ophit->RiseTime() : -999.;
      fData->ophit_width[i]               = ophit->Width();
      fData->ophit_area[i]                = ophit->Area();
      fData->ophit_amplitude[i]           = ophit->Amplitude();
      fData->ophit_pe[i]                  = ophit->PE();
      fData->ophit_fast_to_total[i]       = ophit->FastToTotal();

      // Retrieve the SimPhotonsLite or TrackSDPs associated with this hit
      auto const& sdp_vector = pbt->OpHitToTrackSDPs(*ophit);

      if (!sdp_vector.empty()) {
        // Replicating QLMatchAna logic: check the track ID of the main contributing particle
        int trackID = sdp_vector[0].trackID;
        //std::cout << "DEBUG: ophit #" << i << " trackID = " << trackID << std::endl;
        // Question from Wei:
        // here should we ask for ancester trk ID because ngenG4trkID anf anybkgID are only obtained from largeant assn product, it misses all secodnary particles G4 decides not to track

        if (ngenG4trkID.count(trackID))                   fData->ophit_bt[i] = 0;  // ngen Signal
        else if (ar39G4trkID.count(trackID))              fData->ophit_bt[i] = 1;  // ar39
        else if (ar42G4trkID.count(trackID))              fData->ophit_bt[i] = 2;  // ar42
        else if (kr85G4trkID.count(trackID))              fData->ophit_bt[i] = 3;  // kr85
        else if (k42fromar42G4trkID.count(trackID))       fData->ophit_bt[i] = 4;  // k42fromar42
        else if (k40cathodeG4trkID.count(trackID))        fData->ophit_bt[i] = 5;  // k40cathode
        else if (th232cathodeG4trkID.count(trackID))      fData->ophit_bt[i] = 6;  // th232cathode
        else if (u238cathodeG4trkID.count(trackID))       fData->ophit_bt[i] = 7;  // u238cathode
        else if (k40anodeG4trkID.count(trackID))          fData->ophit_bt[i] = 8;  // k40anode
        else if (th232anodeG4trkID.count(trackID))        fData->ophit_bt[i] = 9;  // th232anode
        else if (u238anodeG4trkID.count(trackID))         fData->ophit_bt[i] = 10; // u238anode
        else if (cryostatfoamgammaG4trkID.count(trackID)) fData->ophit_bt[i] = 11; // cryostatfoamgamma
        else if (cosmicgenG4trkID.count(trackID))         fData->ophit_bt[i] = 12; // cosmicgen
        else                                              fData->ophit_bt[i] = -999; // Unmapped particle
      } else {
        fData->ophit_bt[i] = -999; // No track SDP matching found
      }


    } // end loop ophit
  } // end if else no ophit


  /*
  //================================================
  // In-house lifetime calibration using anode-to-
  // cathode-piercing (ACP) tracks.
  //================================================
  if( fDoACPTrkCalib ) {

    // retrieve track calo data product
    // - pandoracalo = no corrections;
    // - pandoracali = YZ transparency corrections only
    art::FindManyP<recob::Track> fmtrk(hitHandle, evt, fTrkProducer);
    art::FindManyP<anab::Calorimetry> fmcal(tracklistHandle, evt, "pandoracalo");
    if( fmcal.isValid() ) {


      // Map all hits to tracks
      if(


      //================================================
      // loop over the flagged tracks
      //===============================================
      for(auto i : flaggedtrks ) {
        auto& trk = tracklist[i];
        auto& startPt = trk->Vertex();
        auto& endPt   = trk->End();
        float dX      = fabs(startPt.X() - endPt.X());
        float dY      = fabs(startPt.Y() - endPt.Y());
        float dZ      = fabs(startPt.Z() - endPt.Z());
        std::cout<<"Found a ACP candidate: "<<dX<<" "<<dY<<"  "<<dZ<<"\n";


        // THETA/ANGLE cuts



        //==============================================
        // loop the calo objects and find the one for collection plane
        // also require at least 100 dE/dx points
        //==============================================
        std::vector<art::Ptr<anab::Calorimetry> > caloObjs = fmcal.at(i);
        for(auto& caloObj : caloObjs ) {
          if( caloObj->PlaneID().Plane != 2 )       continue;
          if( caloObj->dEdx().size() < 100 )        continue;
          size_t Npts = caloObj->dEdx().size();
          std::cout<<"Found "<<Npts<<" associated calo points for the trk\n";
          h_trk_length_vs_calopts->Fill(trk->Length(), Npts);

          // Calculate the X-coordinate offset that we'll use to shift
          // the track so that its start/end correspond to anode and cathode
          //  - in MicroBooNE, X starts at 0 and increases toward cathode,
          //    so we take the MIN of the two track endpoints as 0
          //  - in ProtoDUNE, X starts at at +/- 357cm and decreases in magnitude
          //    toward the cathode
          float X0 = std::min( startPt.X(), endPt.X() );



          // First, apply SCE spatial corrections at every
          // 3D point along the track...
          std::vector<geo::Point_t> vec_XYZ_orig;
          std::vector<geo::Point_t> vec_XYZ_corr;
          for(size_t j=0; j<Npts; j++){
            geo::Point_t point{ caloObj->XYZ().at(j).X() - X0, caloObj->XYZ().at(j).Y(), caloObj->XYZ().at(j).Z() };
            vec_XYZ_orig.push_back( point );
            geo::Vector_t loc_offset = SCE->GetCalPosOffsets( point );
            point.SetXYZ(point.X()-loc_offset.X(),point.Y()+loc_offset.Y(),point.Z()+loc_offset.Z());
            vec_XYZ_corr.push_back( point );
          }

          // record the "dx correction factor" applied at each
          // trajectory point along the track
          std::vector<float> dx_corr_factor( Npts, 1. );
          for(size_t j=1; j<Npts-1; j++){
            float ds_prev = sqrt( ( vec_XYZ_orig[j-1] - vec_XYZ_orig[j+1] ).Mag2() );
            float ds_new  = sqrt( ( vec_XYZ_corr[j-1] - vec_XYZ_corr[j+1] ).Mag2() );
            float corr_factor = ds_new/ds_prev;
            dx_corr_factor[j] = corr_factor;
          }



          }



      }//endloop over flagged trks
    }//endif fmcal is valid
  }//END ACP TRACK CALIBRATION
*/




  //====================================
  // Save hit information
  //====================================
  std::cout<<"Looping over the hits...\n";
  int   num_hits[kNplanes]            ={0};
  int   num_hits_untracked[kNplanes]  ={0};
  int   num_hits_true[kNplanes]       ={0};
  int   num_hits_pmatch[kNplanes]     ={0};
  float total_hit_charge[kNplanes]    ={0};

  for(size_t i=0; i<hitlist.size(); i++){
    auto const& hinfo = fBlipAlg.hitinfo[i];

    //PrintHitInfo(hinfo);
    int     plane   = hitlist[i]->WireID().Plane;
    int     ndf     = hitlist[i]->DegreesOfFreedom();
    double  gof     = (ndf>0) ? hitlist[i]->GoodnessOfFit()/ndf : -9;
    int     isFit   = int(gof>=0);
    int     mult    = (isFit) ? hitlist[i]->Multiplicity() : -9;
    float   logGOF  = (isFit && mult == 1) ? log10(gof) : 99;
    float   amp     = (isFit && mult == 1) ? hitlist[i]->PeakAmplitude() : -9;
    float   rms     = (isFit && mult == 1) ? hitlist[i]->RMS() : -9; //(gof>0) ? hitlist[i]->RMS() : -99;
    float   sumADC  = hitlist[i]->HitSummedADC();
    float   integral= hitlist[i]->Integral();
    float   qreco   = hinfo.charge;
    bool    isMC    = (hinfo.g4trkid >= 0 );
    bool    isTrked     = (hinfo.trkid >= 0 && map_trkid_length[hinfo.trkid] > 5 );
    bool    isMIP       = map_trkid_isMIP[hinfo.trkid];
    bool    isMatched   = hinfo.ismatch;

    fNumHits[plane]++;
    num_hits[plane]++;

    h_hitq[plane]->Fill(qreco/1e3);
    if( integral != 0 ) h_hitqerr[plane]    -> Fill( fabs(hitlist[i]->SigmaIntegral()/hitlist[i]->Integral()) );
    if( sumADC != 0 )   h_hitadcdiff[plane] ->Fill( (integral-sumADC)/sumADC );


    // calculate reco-true resolution
    if( isMC ) {
      float qtrue = hinfo.g4charge;
      if(qreco && qtrue){
        fNumHitsTrue[plane]++;
        if( isMatched ) fNumHitsMatchedTrue[plane]++;
        num_hits_true[plane]++;
        total_hit_charge[plane] += qtrue;
        float qres = (qreco-qtrue)/qtrue;
        h_hitqres[plane]->Fill(qres);
        h_hitqres_vs_q[plane]->Fill(qtrue/1e3,qres);
        h_hitqres_scatter[plane]->Fill(qtrue/1e3,qreco/1e3);
      }
    }

    // --- isolated hits --
    if( !isTrked ) {
      fNumHitsUntracked[plane]++;
      num_hits_untracked[hinfo.plane]++;
      h_hitamp[plane]   ->Fill(amp);
      h_hitrms[plane]   ->Fill(rms);
      h_hitgof[plane]   ->Fill(logGOF);
      h_hitmult[plane]  ->Fill(mult);
      if( amp > 0 )
      h_hitratio[plane] ->Fill(rms/amp);

      // -- isolated and plane-matched --
      if( isMatched ) {
        fNumHitsMatched[plane]++;
        num_hits_pmatch[hinfo.plane]++;
      }

      // -- isolated hits from an MC PARTICLE --
      if( isMC ) {
        h_hitamp_true[plane]->Fill(amp);
        h_hitrms_true[plane]->Fill(rms);
        h_hitgof_true[plane]->Fill(logGOF);
        h_hitmult_true[plane]->Fill(mult);
      } else {
        h_hitamp_fake[plane]->Fill(amp);
        h_hitrms_fake[plane]->Fill(rms);
        h_hitgof_fake[plane]->Fill(logGOF);
        h_hitmult_fake[plane]->Fill(mult);
      }

    }//endif hit is untracked


    // -- hits in long MIP-like tracks --
    //    (i.e., NON-isolated hits)
    if( isMIP ) {
      h_hitamp_mip[plane]   ->Fill(amp);
      h_hitrms_mip[plane]   ->Fill(rms);
      h_hitgof_mip[plane]   ->Fill(logGOF);
      h_hitmult_mip[plane]  ->Fill(mult);
    }


    // fill data to be saved to event tree
    if( i < kMaxHits && fData->saveHitInfo ){
      fData->hit_plane[i]     = hitlist[i]->WireID().Plane;
      fData->hit_wire[i]      = hitlist[i]->WireID().Wire;
      fData->hit_tpc[i]       = hitlist[i]->WireID().TPC;
      fData->hit_cryo[i]      = hitlist[i]->WireID().Cryostat;
      fData->hit_channel[i]   = hitlist[i]->Channel();
      fData->hit_peakT[i]     = hitlist[i]->PeakTime();
      fData->hit_rms[i]       = hitlist[i]->RMS();
      fData->hit_amp[i]	      = hitlist[i]->PeakAmplitude();
      fData->hit_area[i]      = hitlist[i]->Integral();
      fData->hit_sumadc[i]    = hitlist[i]->HitSummedADC();
      fData->hit_mult[i]      = hitlist[i]->Multiplicity();
      fData->hit_gof[i]       = gof;
      fData->hit_trkid[i]     = hinfo.trkid;
      fData->hit_time[i]      = hinfo.peakTick;
      fData->hit_charge[i]    = hinfo.charge;
      fData->hit_ismatch[i]   = hinfo.ismatch;
      fData->hit_g4trkid[i]   = hinfo.g4trkid;
      fData->hit_g4frac[i]    = hinfo.g4frac;
      fData->hit_g4energy[i]  = hinfo.g4energy;
      fData->hit_g4charge[i]  = hinfo.g4charge;
      fData->hit_blipid[i]    = hinfo.blipid;
      fData->hit_clustid[i]   = hinfo.clustid;
    }

  }//endloop over hits

  // Now that we've looped all the hits, calculate some
  // plane-specific variables and fill histograms
  for(size_t ip=0; ip<kNplanes; ip++){
    h_nhits[ip]   ->Fill(num_hits[ip]);
    //h_nhits_noise[ip] ->Fill(num_hits[ip]-num_hits_true[ip]);
    float qcomp     = -9;
    if( num_hits_true[ip] ) {
      if(total_depElectrons )  qcomp = total_hit_charge[ip]/total_depElectrons;
      h_chargecomp[ip]->Fill( qcomp );
      h_hitpur[ip]->Fill((float)num_hits_true[ip]/num_hits[ip]);
    }
  }//endloop over planes


  //=============================================
  // Save hit cluster info
  //=============================================
  fData->nclusts = (int)fBlipAlg.hitclust.size();
  int num_clusts[kNplanes]     ={0};
  int num_clusts_pm[kNplanes]   ={0};
  if( fDebugMode ) std::cout<<"\nLooping over "<<fBlipAlg.hitclust.size()<<" 2D hit clusters...\n";
  for(size_t i=0; i < fBlipAlg.hitclust.size(); i++){
    auto const& clust = fBlipAlg.hitclust[i];
    num_clusts[clust.Plane]++;
    if( clust.isMatched ) num_clusts_pm[clust.Plane]++;
    if( !fSavePlaneInfo[clust.Plane] ) continue;
    fData->clust_id[i]        = clust.ID;
    fData->clust_cryo[i]      = clust.Cryostat;
    fData->clust_tpc[i]       = clust.TPC;
    fData->clust_plane[i]     = clust.Plane;
    fData->clust_wire[i]        = clust.CenterWire;
    fData->clust_startwire[i]   = clust.StartWire;
    fData->clust_endwire[i]     = clust.EndWire;
    fData->clust_nwires[i]      = clust.NWires;
    fData->clust_bydeadwire[i]  = (clust.DeadWireSep==0);
    fData->clust_nhits[i]       = clust.NHits;
    fData->clust_pulsetrain[i] = (clust.NPulseTrainHits>0);
    // Truncate precision to reduce file size after ROOT compression
    // (we don't need to know these to the Nth decimal place)
    fData->clust_charge[i]    = Truncate(clust.Charge,    10);
    fData->clust_chargeErr[i] = Truncate(clust.SigmaCharge, 10);
    fData->clust_amp[i]       = Truncate(clust.Amplitude, 0.01);
    fData->clust_time[i]      = Truncate(clust.Tick,      0.1);
    fData->clust_timespan[i]  = Truncate(clust.Tickspan,  0.01);
    fData->clust_starttime[i] = Truncate(clust.StartTick, 0.1);
    fData->clust_endtime[i]   = Truncate(clust.EndTick,   0.1);
    fData->clust_ismatch[i]   = clust.isMatched;
    fData->clust_touchtrk[i]  = (clust.TouchTrkID >= 0 );
    fData->clust_touchtrkid[i]= clust.TouchTrkID;
    fData->clust_blipid[i]    = clust.BlipID;
    fData->clust_edepid[i]    = clust.EdepID;

    // if this clust has an associated "trueblip" ID, find it
    // and figure out the true G4 charge, energy, etc
    int tbi = clust.EdepID;
    if( tbi >= 0 && tbi < (int)fBlipAlg.trueblips.size() ) {
      auto const& trueBlip = fBlipAlg.trueblips[tbi];
      fData->clust_edepid[i]   = trueBlip.ID;
      float q_reco  = clust.Charge;
      float q_anode = trueBlip.NumElectrons;
      float q_dep   = trueBlip.DepElectrons;
      float tdrift  = trueBlip.DriftTime;

      // fill diagnostic histograms for energy deposits from electrons
      if( clust.Plane==fCaloPlane && q_dep > 2000 ) { //&& tdrift > 100 ) {
        h_clust_qres_anode   ->Fill( (q_reco-q_anode)/q_anode );
        h_clust_qres_dep     ->Fill( (q_reco-q_dep)/q_dep );
        h_clust_qres_vs_q    ->Fill( q_dep/1e3, (q_reco-q_dep)/q_dep );
        h_qratio_vs_time_sim ->Fill( tdrift, q_anode/q_dep );
      }

    }

    if( fDebugMode ) PrintClusterInfo(clust);

  }//endloop over 2D hit clusters

  for(size_t ip=0; ip<kNplanes; ip++){
    h_nclusts[ip]   ->Fill(num_clusts[ip]);
    h_nclusts_pm[ip]->Fill(num_clusts_pm[ip]);
  }//endloop over planes

  //====================================
  // Save blip info to tree
  //===================================
  fData->nblips             = fBlipAlg.blips.size();
  int nblips_matched        = 0;
  int nblips_total          = 0;
  int nblips_picky          = 0;
  float true_blip_charge    = 0;
  for(size_t i=0; i<fBlipAlg.blips.size(); i++){
    auto& blp = fBlipAlg.blips[i];

    nblips_total++;
    fNum3DBlips++;
    if( blp.NPlanes >= 3 ) fNum3DBlips3Plane++;
    fData->blip_id[i]         = i;
    fData->blip_cryo[i]       = blp.Cryostat;
    fData->blip_tpc[i]        = blp.TPC;
    fData->blip_nplanes[i]    = blp.NPlanes;
    fData->blip_time[i]       = blp.Time;
    fData->blip_x[i]          = blp.Position.X();
    fData->blip_y[i]          = blp.Position.Y();
    fData->blip_z[i]          = blp.Position.Z();
    fData->blip_sigmayz[i]    = blp.SigmaYZ;
    fData->blip_dx[i]         = blp.dX;
    fData->blip_dw[i]         = blp.dYZ;
    fData->blip_size[i]       = sqrt( pow(blp.dX,2) + pow(blp.dYZ,2) );
    fData->blip_proxtrkdist[i]= blp.ProxTrkDist;
    fData->blip_proxtrkid[i]  = blp.ProxTrkID;
    fData->blip_incylinder[i] = blp.inCylinder;
    fData->blip_touchtrk[i]   = (blp.TouchTrkID >= 0 );
    fData->blip_touchtrkid[i] = blp.TouchTrkID;
    fData->blip_charge[i]     = blp.Charge;
    fData->blip_energy[i]     = blp.Energy;
    fData->blip_energyCorr[i] = blp.EnergyCorr;
    //fData->blip_yzcorr[i]     = tpcCalib.YZdqdxCorrection(fCaloPlane,blp.Position.Y(),blp.Position.Z());

    // Extract the leading true particle Track ID
    int leadTrkID = blp.truth.LeadG4ID;
    fData->blip_leadTrkID[i] = leadTrkID;
    // Question from Wei:
    // here should we ask for ancester trk ID because ngenG4trkID anf anybkgID are only obtained from largeant assn product, it misses all secodnary particles G4 decides not to track

    std::cout << "DEBUG: Blip #" << i << " LeadG4ID = " << leadTrkID << std::endl;

    if (ngenG4trkID.count(leadTrkID))                   fData->blip_bt[i] = 0;   // ngen Signal
    else if (ar39G4trkID.count(leadTrkID))              fData->blip_bt[i] = 1;   // ar39
    else if (ar42G4trkID.count(leadTrkID))              fData->blip_bt[i] = 2;   // ar42
    else if (kr85G4trkID.count(leadTrkID))              fData->blip_bt[i] = 3;   // kr85
    else if (k42fromar42G4trkID.count(leadTrkID))       fData->blip_bt[i] = 4;   // k42fromar42
    else if (k40cathodeG4trkID.count(leadTrkID))        fData->blip_bt[i] = 5;   // k40cathode
    else if (th232cathodeG4trkID.count(leadTrkID))      fData->blip_bt[i] = 6;   // th232cathode
    else if (u238cathodeG4trkID.count(leadTrkID))       fData->blip_bt[i] = 7;   // u238cathode
    else if (k40anodeG4trkID.count(leadTrkID))          fData->blip_bt[i] = 8;   // k40anode
    else if (th232anodeG4trkID.count(leadTrkID))        fData->blip_bt[i] = 9;   // th232anode
    else if (u238anodeG4trkID.count(leadTrkID))         fData->blip_bt[i] = 10;  // u238anode
    else if (cryostatfoamgammaG4trkID.count(leadTrkID)) fData->blip_bt[i] = 11;  // cryostatfoamgamma
    else if (cosmicgenG4trkID.count(leadTrkID))         fData->blip_bt[i] = 12;  // cosmicgen
    else                                                fData->blip_bt[i] = -999; // Unknown background / noise

    // Fill cluster charge 2D histograms
    h_blip_charge   ->Fill(blp.Charge);
    h_blip_charge_YU->Fill( 0.001*blp.clusters[2].Charge, 0.001*blp.clusters[0].Charge );
    h_blip_charge_YV->Fill( 0.001*blp.clusters[2].Charge, 0.001*blp.clusters[1].Charge );
    h_blip_charge_UV->Fill( 0.001*blp.clusters[0].Charge, 0.001*blp.clusters[1].Charge );
    for(size_t ipl = 0; ipl<kNplanes; ipl++){
      if( blp.clusters[ipl].NHits <= 0 ) continue;
      fData->blip_clustid[ipl][i] = blp.clusters[ipl].ID;
      fData->blip_nwires[ipl][i]  = blp.clusters[ipl].NWires;

      // record all amps


      // max amp
      fData->blip_maxamp[ipl][i]  = blp.clusters[ipl].Amplitude;

      // min amp
      //fData->blip_minamp[ipl][i] =
      // find all hits in this blip per plane
      auto& hids = blp.clusters[ipl].HitIDs;
      float minamp = -9;
      int hitnum = (int)hids.size();
      for(auto const& id : hids ) {
        //std::cout<<"Blip "<<i<<" on plane "<<ipl<<"  HITID "<<id<<"\n";
        auto const& hinfo = fBlipAlg.hitinfo[id];
        int mult    = hitlist[id]->Multiplicity();
        float amp   = hinfo.amp;
        if( amp > 0 ) h_blipE_vs_hitamp[ipl]->Fill(blp.Energy,amp,1./hitnum);
        if( mult != 1 ) continue;
        if( minamp < 0 || amp < minamp ) minamp = amp;
      }

      fData->blip_minamp[ipl][i] = minamp;

    }

    // Select picky (high-quality) blips:
    if(blp.NPlanes >= 3 && blp.SigmaYZ < 1.) {
      nblips_picky++;
      fNum3DBlipsPicky++;
      h_blip_charge_picky ->Fill(blp.clusters[fCaloPlane].Charge);
       h_blip_zy_picky     ->Fill(blp.Position.Z(), blp.Position.Y());
      h_blip_charge_YU_picky->Fill( 0.001*blp.clusters[2].Charge, 0.001*blp.clusters[0].Charge );
      h_blip_charge_YV_picky->Fill( 0.001*blp.clusters[2].Charge, 0.001*blp.clusters[1].Charge );
      h_blip_charge_UV_picky->Fill( 0.001*blp.clusters[0].Charge, 0.001*blp.clusters[1].Charge );
    }

    h_blip_zy     ->Fill(blp.Position.Z(), blp.Position.Y());
    h_blip_nplanes->Fill(blp.NPlanes);

    // -----------------------------------------------
    // save the clustIDs and true energy deposits to the blip
    // (use the association between clust <--> edep)
    // -----------------------------------------------
    if( blp.truth.ID >= 0 && blp.truth.Energy > 0 ) {
      fData->blip_isMC[i]             = true;
      fData->blip_edepid[i]           = blp.truth.ID;
      fData->blip_energyTrue[i]       = blp.truth.Energy;
      fData->edep_blipid[blp.truth.ID]  = blp.ID;
      fNum3DBlipsTrue++;
      if( blp.NPlanes >= 3 ) fNum3DBlipsTrue3P++;
      nblips_matched++;
      true_blip_charge += blp.truth.NumElectrons;
      auto dl = (blp.Position - blp.truth.Position);
      float res = (blp.Energy - blp.truth.Energy) / blp.truth.Energy;
      h_blip_reszy->Fill(dl.Z(),dl.Y());
      h_blip_resx ->Fill(dl.X());
      h_blip_resE->Fill(res);
      h_blip_E_vs_resE->Fill( blp.truth.Energy, res);
    }


  }//endloop over 3D blips

  // Fill some more histograms...
  h_nblips->Fill(nblips_total);
  h_nblips_picky->Fill(nblips_picky);
  if( fIsMC ) {
    h_nblips_tm->Fill(nblips_matched);
    if( total_numElectrons        ) h_blip_qcomp      ->Fill(true_blip_charge      / total_numElectrons     );
    std::cout<<"   (truth-matched blips: "<<nblips_matched<<")\n";
  }

  if( fDebugMode ) {
    std::cout<<"\nLooping over "<<fBlipAlg.blips.size()<<" 3D blips:\n";
    for(auto const& b : fBlipAlg.blips ) PrintBlipInfo(b);
  }



  //====================================
  // Fill TTree
  //====================================
  fData->evtTree->Fill();

}


//###################################################
//  endJob: output useful info to screen
//###################################################
void BlipAnaMC::endJob(){

  fBlipAlg.h_recoWireEff_num->Divide(fBlipAlg.h_recoWireEff_denom);
  fBlipAlg.h_recoWireEff_num->SetOption("hist");
  fBlipAlg.h_recoWireEff_num->SetBit(TH1::kIsAverage);
  fBlipAlg.h_recoWireEffQ_num->Divide(fBlipAlg.h_recoWireEffQ_denom);
  fBlipAlg.h_recoWireEffQ_num->SetOption("hist");
  fBlipAlg.h_recoWireEffQ_num->SetBit(TH1::kIsAverage);
  float nEvents = float(fNumEvents);
  //h_trk_length->Scale(1./nEvents);
  BlipUtils::NormalizeHist(h_clust_qres_anode);
  BlipUtils::NormalizeHist(h_clust_qres_dep);
  for(size_t i=0; i<kNplanes; i++){
    BlipUtils::NormalizeHist(h_hitamp[i]);
    BlipUtils::NormalizeHist(h_hitamp_true[i]);
    BlipUtils::NormalizeHist(h_hitamp_fake[i]);
    BlipUtils::NormalizeHist(h_hitamp_mip[i]);
    BlipUtils::NormalizeHist(h_hitrms[i]);
    BlipUtils::NormalizeHist(h_hitrms_true[i]);
    BlipUtils::NormalizeHist(h_hitrms_fake[i]);
    BlipUtils::NormalizeHist(h_hitrms_mip[i]);
    BlipUtils::NormalizeHist(h_hitgof[i]);
    BlipUtils::NormalizeHist(h_hitgof_true[i]);
    BlipUtils::NormalizeHist(h_hitgof_fake[i]);
    BlipUtils::NormalizeHist(h_hitgof_mip[i]);
    BlipUtils::NormalizeHist(h_hitmult[i]);
    BlipUtils::NormalizeHist(h_hitmult_true[i]);
    BlipUtils::NormalizeHist(h_hitmult_fake[i]);
    BlipUtils::NormalizeHist(h_hitmult_mip[i]);
  }


  printf("\n***********************************************\n");
  fBlipAlg.PrintConfig();
  printf("BlipAnaMC Summary\n\n");
  printf("  Total events                : %i\n",        fNumEvents);
  printf("  Blips per evt, total        : %.3f\n",      fNum3DBlips/nEvents);
  printf("                 3 planes     : %.3f\n",      fNum3DBlips3Plane/nEvents);

  if(fIsMC){
  printf("  MC-matched blips per evt    : %.3f\n",       fNum3DBlipsTrue/nEvents);
  printf("  MC blip purity              : %.3f\n",       fNum3DBlipsTrue/float(fNum3DBlips));
  printf("  MC blip purity, 3 planes    : %.3f\n",      fNum3DBlipsTrue3P/float(fNum3DBlips3Plane));
  if( h_blip_qcomp->GetMean() > 0 )
  printf("  Charge completeness, total  : %.4f +/- %.4f\n", h_blip_qcomp->GetMean(), h_blip_qcomp->GetStdDev()/sqrt(fNumEvents));
  }
  printf("  Mean blip charge            : %.0f e-\n",      h_blip_charge->GetMean());
  printf("\n");
  for(size_t i=0; i<kNplanes; i++){
  printf("  Plane %lu -------------------------\n",i);
  printf("   * total hits/evt           : %.2f\n",fNumHits[i]/(float)fNumEvents);
  printf("   * untracked hits/evt       : %.2f (%.2f plane-matched)\n",fNumHitsUntracked[i]/(float)fNumEvents, fNumHitsMatched[i]/(float)fNumEvents);
  if(fIsMC) {
  printf("   * true-matched hits/evt    : %.2f (%.2f plane-matched)\n",fNumHitsTrue[i]/(float)fNumEvents, fNumHitsMatchedTrue[i]/(float)fNumEvents);
  if( h_chargecomp[i]->GetMean() > 0 )
  printf("   * charge completeness      : %.4f\n",h_chargecomp[i]->GetMean());
  printf("   * hit purity               : %.4f\n",h_hitpur[i]->GetMean());
  }
  }
  printf("\n***********************************************\n");

}




//###################################################
//  Printouts for debugging
//###################################################

void BlipAnaMC::PrintParticleInfo(size_t i){
  printf("  %5i  trkID: %-6i PDG: %-10i XYZ= %7.1f %7.1f %7.1f, dL=%7.2f, Npts=%4i, KE0=%8.3f, Edep=%8.3f, T=%10.2f, moth=%5i, %12s, ND=%i\n",
   (int)i,
   fData->part_trackID[i],
   fData->part_pdg[i],
   fData->part_startPointx[i],
   fData->part_startPointy[i],
   fData->part_startPointz[i],
   fData->part_pathlen[i],
   fData->part_numTrajPts[i],
   fData->part_KE[i],
   fData->part_depEnergy[i],
   fData->part_startT[i]/1e3,
   fData->part_mother[i],
   fData->part_process[i].c_str(),
   fData->part_nDaughters[i]
  );
}

void BlipAnaMC::PrintTrueBlipInfo(const blipobj::TrueBlip& tb){
  printf("  edepID: %5i  G4ID: %-6i PDG: %-10i XYZ: %7.2f, %7.2f, %7.2f, %8.3f MeV, %8i e- deposited, %8i e- @anode,  %12s\n",
   tb.ID,
   tb.LeadG4ID,
   tb.LeadG4PDG,
   tb.Position.X(),
   tb.Position.Y(),
   tb.Position.Z(),
   tb.Energy,
   tb.DepElectrons,
   tb.NumElectrons,
   fData->part_process[tb.LeadG4Index].c_str()
  );
}

void BlipAnaMC::PrintHitInfo(const blipobj::HitInfo& hi){
  printf("  hitID: %4i, TPC: %i, plane: %i, driftTicks: %7.2f, leadWire: %3i, G4ID: %4i, recoTrack: %4i\n",
    hi.hitid,
    hi.tpc,
    hi.plane,
    hi.driftTick,
    hi.wire,
    hi.g4trkid,
    hi.trkid
  );
}

void BlipAnaMC::PrintClusterInfo(const blipobj::HitClust& hc){
  printf("  clustID: %4i, TPC: %i, plane: %i, time range: %7.2f - %7.2f, timespan: %6.2f, leadWire: %3i, nwires: %3i, nhits: %3i, edepid: %i, isMatched: %i\n",
    hc.ID,
    hc.TPC,
    hc.Plane,
    hc.StartTick,
    hc.EndTick,
    hc.Tickspan,
    hc.CenterWire,
    hc.NWires,
    hc.NHits,
    hc.EdepID,
    hc.isMatched
  );
}

void BlipAnaMC::PrintBlipInfo(const blipobj::Blip& bl){
  printf("  blipID: %4i, TPC: %i, charge: %8.0i,  recoEnergy: %8.3f MeV, XYZ: %6.1f, %6.1f, %6.1f,   EdepID: %i\n",
  bl.ID,
  bl.TPC,
  (int)bl.clusters[2].Charge,
  bl.Energy,
  bl.Position.X(),bl.Position.Y(),bl.Position.Z(),
  bl.truth.ID
  );
}

float BlipAnaMC::Truncate(float input, double base){
  if( base > 0 )  return roundf( input / base ) * base;
  else            return input;
  return input;
}


DEFINE_ART_MODULE(BlipAnaMC)

#endif
