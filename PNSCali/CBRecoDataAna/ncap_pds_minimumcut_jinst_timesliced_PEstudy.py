import ROOT
#from ROOT import TFile, TTree
from ROOT import gROOT, TFile, TTree, TCanvas, TGraph, TH1D
from array import array
import tables as tab
import numpy as np
import math
import os
import os.path
import sys
import lar_param as lar
import projection_2d as proj2d
import histograms as hist
import matplotlib
import matplotlib.pyplot as plt

#run_number = 25004 # cosmic run
#run_number = 25066 # cosmic run
#run_number = 25078 # cosmic run
#run_number = 25084 # cosmic run
run_number = 25086 # cosmic run

#run_number = 25036 # PNS+TPC+PDS run
#run_number = 25050 # PNS+TPC+PDS run? --> do not use
#run_number = 25068 # PNS+TPC+PDS run
#run_number = 25071 # PNS+TPC+PDS run

""" get the general infos """
with tab.open_file(sys.argv[1],"r") as f:
    infos = f.get_node('/',name='infos',classname='Table')
    n_channels = infos.read(0,1,field='n_channels')[0].astype(int)
    n_view = infos.read(0,1,field='n_view')[0].astype(int)
    e_drift = infos.read(0,1,field='e_drift')[0].astype(float)
    view_nchan  = infos.read(0,1,field='view_nchan')[0].astype(int)

v_drift = lar.drift_velocity(e_drift)
print("v_drift = ", v_drift)

# Here we only take the PDE variation calibrated based on near perpendicular cosmics passing through the center:
# C1: 125.5/130.8 [if combined for the two channels: 256.3]
# C2: 68.2/62.97 [131.2]
# C3: 69.14/69.4 [138.5] - use this as ref
# C4: 57.42/48.4 [105.8]
pescaleC3 = 1.0
pescaleC1 = 1.85
pescaleC2 = 0.947
pescaleC4 = 0.764

# PD channel map for runs on April 18-19
if run_number == 25068 or run_number == 25071 or run_number == 25078 or run_number == 25084 or run_number == 25086:
    C1_ch1 = 0 # 2nd closest to PNS
    C1_ch2 = 7
    C2_ch1 = 10
    C2_ch2 = 17
    C3_ch1 = 30
    C3_ch2 = 37
    C4_ch1 = 40 # closest to PNS
    C4_ch2 = 47
if run_number == 25004 or run_number == 25034 or run_number == 25036 or run_number == 25066:
    C1_ch1 = 0 # 2nd closest to PNS
    C1_ch2 = 7
    C2_ch1 = 10
    C2_ch2 = 17
    C3_ch1 = 20
    C3_ch2 = 27
    C4_ch1 = 40 # closest to PNS
    C4_ch2 = 47

if run_number == 25068:
    beam_stop_time_inPDticks = 11500 # in pd time ticks, for cosmic run doesn't matter
if run_number == 25004 or run_number == 25036 or run_number == 25066 or run_number == 25071 or run_number == 25078 or run_number == 25084 or run_number == 25086:
    beam_stop_time_inPDticks = 6563 # 105us in pd time ticks

# PD coordinates in coldbox: lardon use CB center as origin
# from geo.json in cbbot
C1_x = -116.8
C1_y = -42.05
C2_x = -51.5
C2_y = 107.35
C3_x = 116.8
C3_y = 42.05
C4_x = 51.5
C4_y = -107.35

# ADCs per PE channel by channel
if run_number == 25004:
    C1_ch1_adcgain = 14.1 # 2nd closest to PNS
    C1_ch2_adcgain = 10.3
    C2_ch1_adcgain = 12.7
    C2_ch2_adcgain = 11.1
    C3_ch1_adcgain = 13.5
    C3_ch2_adcgain = 13.1
    C4_ch1_adcgain = 12.65 # closest to PNS
    C4_ch2_adcgain = 12.8
if run_number == 25034 or run_number == 25035 or run_number == 25036 or run_number == 25049 or run_number == 25050 or run_number == 25066:
    C1_ch1_adcgain = 14.1  # 2nd closest to PNS
    C1_ch2_adcgain = 10.3
    C2_ch1_adcgain = 12.7
    C2_ch2_adcgain = 11.1
    C3_ch1_adcgain = 13.5
    C3_ch2_adcgain = 13.1
    C4_ch1_adcgain = 9.1  # closest to PNS
    C4_ch2_adcgain = 16.6
if run_number == 25068:
    C1_ch1_adcgain = 14.1  # 2nd closest to PNS
    C1_ch2_adcgain = 10.3
    C2_ch1_adcgain = 12.7
    C2_ch2_adcgain = 11.1
    C3_ch1_adcgain = 12.65
    C3_ch2_adcgain = 12.8
    C4_ch1_adcgain = 9.1  # closest to PNS
    C4_ch2_adcgain = 16.6
if run_number == 25071 or run_number == 25078 or run_number == 25080 or run_number == 25084 or run_number == 25086:
    C1_ch1_adcgain = 14.1  # 2nd closest to PNS
    C1_ch2_adcgain = 10.3
    C2_ch1_adcgain = 12.7
    C2_ch2_adcgain = 11.1
    C3_ch1_adcgain = 12.65
    C3_ch2_adcgain = 12.8
    C4_ch1_adcgain = 9.1  # closest to PNS
    C4_ch2_adcgain = 16.6
# old adc-pe from Henrique
#C1_ch1_adcgain = 15 # 2nd closest to PNS
#C1_ch2_adcgain = 10
#C2_ch1_adcgain = 15
#C2_ch2_adcgain = 10
#C3_ch1_adcgain = 15
#C3_ch2_adcgain = 15
#C4_ch1_adcgain = 10 # closest to PNS
#C4_ch2_adcgain = 15

# tpc time window (ticks) relative to PD peak time
driftt0 = 0
driftt204 = 204
fulldriftt = 273 # tpc ticks ~ 140us
t_coincidence = 5
max_time_tick_pd = 65625 # for pd system, each tick is 16 ns
max_time_tick_tpc = 2100 # for tpc system, each tick is 512 ns
max_PE_pd = 3500
n_cap_pd_minPE = 50
pd_adc_saturation_cut = 14000 # 14-bit saturation 16384 adcs
#pd_time_gap = 625 # 10us, typical length of pd wfm
#pd_time_gap = 8750 # 1 tpc drfit window in pd time ticks
blip_trk_dist = 30 # cm
trk3d_time_gap = 70 # TPC ticks
fulldriftd = 19 # cm

inname = sys.argv[1]
inname = inname[inname.rfind("/")+1:]
inname = inname[:inname.find(".h5")]
print(inname)

if(inname[:2] == "cb"):
    inname = inname[2:]

is_top = True
if(inname[:3]=="bot"):
    is_top = False

print(inname)
print('Is top? ', is_top)

# Create directory for plots to be stored if it doesn't already exist
out_path = "results"
if not os.path.exists(out_path):
    os.makedirs(out_path)
    print("out_path '" + out_path + "' did not exist. It has been created!")
print(" \n") # separate output

# Create the ROOT file that will hold the output of this script
f_out = TFile('{0}/CBAna_JINST_Timeslice_PE.root'.format(out_path), 'RECREATE')

myTotTriggers = TTree('myTotTriggers', 'myTotTriggers')
myMatchedC1Peaks = TTree('myMatchedC1Peaks', 'myMatchedC1Peaks')
myMatchedC2Peaks = TTree('myMatchedC2Peaks', 'myMatchedC2Peaks')
myMatchedC3Peaks = TTree('myMatchedC3Peaks', 'myMatchedC3Peaks')
myMatchedC4Peaks = TTree('myMatchedC4Peaks', 'myMatchedC4Peaks')

my_nTriggers = array('i', [0])
myTotTriggers.Branch('my_nTriggers', my_nTriggers, 'my_nTriggers/I')
C1_matched_PDPeak_PE = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_PE', C1_matched_PDPeak_PE, 'C1_matched_PDPeak_PE/F')
C1_matched_PDPeak_PE_0_10kpdticks = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_PE_0_10kpdticks', C1_matched_PDPeak_PE_0_10kpdticks, 'C1_matched_PDPeak_PE_0_10kpdticks/F')
C1_matched_PDPeak_PE_10k_30kpdticks = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_PE_10k_30kpdticks', C1_matched_PDPeak_PE_10k_30kpdticks, 'C1_matched_PDPeak_PE_10k_30kpdticks/F')
C1_matched_PDPeak_PE_30kpluspdticks = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_PE_30kpluspdticks', C1_matched_PDPeak_PE_30kpluspdticks, 'C1_matched_PDPeak_PE_30kpluspdticks/F')
C1_matched_PDPeak_Time = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_Time', C1_matched_PDPeak_Time, 'C1_matched_PDPeak_Time/F')
C1_matched_PDPeak_Time_100_1200PE = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_Time_100_1200PE', C1_matched_PDPeak_Time_100_1200PE, 'C1_matched_PDPeak_Time_100_1200PE/F')
C1_matched_PDPeak_Time_above_1200PE = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_Time_above_1200PE', C1_matched_PDPeak_Time_above_1200PE, 'C1_matched_PDPeak_Time_above_1200PE/F')

C2_matched_PDPeak_PE = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_PE', C2_matched_PDPeak_PE, 'C2_matched_PDPeak_PE/F')
C2_matched_PDPeak_PE_0_10kpdticks = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_PE_0_10kpdticks', C2_matched_PDPeak_PE_0_10kpdticks, 'C2_matched_PDPeak_PE_0_10kpdticks/F')
C2_matched_PDPeak_PE_10k_30kpdticks = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_PE_10k_30kpdticks', C2_matched_PDPeak_PE_10k_30kpdticks, 'C2_matched_PDPeak_PE_10k_30kpdticks/F')
C2_matched_PDPeak_PE_30kpluspdticks = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_PE_30kpluspdticks', C2_matched_PDPeak_PE_30kpluspdticks, 'C2_matched_PDPeak_PE_30kpluspdticks/F')
C2_matched_PDPeak_Time = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_Time', C2_matched_PDPeak_Time, 'C2_matched_PDPeak_Time/F')
C2_matched_PDPeak_Time_100_1200PE = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_Time_100_1200PE', C2_matched_PDPeak_Time_100_1200PE, 'C2_matched_PDPeak_Time_100_1200PE/F')
C2_matched_PDPeak_Time_above_1200PE = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_Time_above_1200PE', C2_matched_PDPeak_Time_above_1200PE, 'C2_matched_PDPeak_Time_above_1200PE/F')

C3_matched_PDPeak_PE = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_PE', C3_matched_PDPeak_PE, 'C3_matched_PDPeak_PE/F')
C3_matched_PDPeak_PE_0_10kpdticks = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_PE_0_10kpdticks', C3_matched_PDPeak_PE_0_10kpdticks, 'C3_matched_PDPeak_PE_0_10kpdticks/F')
C3_matched_PDPeak_PE_10k_30kpdticks = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_PE_10k_30kpdticks', C3_matched_PDPeak_PE_10k_30kpdticks, 'C3_matched_PDPeak_PE_10k_30kpdticks/F')
C3_matched_PDPeak_PE_30kpluspdticks = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_PE_30kpluspdticks', C3_matched_PDPeak_PE_30kpluspdticks, 'C3_matched_PDPeak_PE_30kpluspdticks/F')
C3_matched_PDPeak_Time = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_Time', C3_matched_PDPeak_Time, 'C3_matched_PDPeak_Time/F')
C3_matched_PDPeak_Time_100_1200PE = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_Time_100_1200PE', C3_matched_PDPeak_Time_100_1200PE, 'C3_matched_PDPeak_Time_100_1200PE/F')
C3_matched_PDPeak_Time_above_1200PE = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_Time_above_1200PE', C3_matched_PDPeak_Time_above_1200PE, 'C3_matched_PDPeak_Time_above_1200PE/F')

C4_matched_PDPeak_PE = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_PE', C4_matched_PDPeak_PE, 'C4_matched_PDPeak_PE/F')
C4_matched_PDPeak_PE_0_10kpdticks = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_PE_0_10kpdticks', C4_matched_PDPeak_PE_0_10kpdticks, 'C4_matched_PDPeak_PE_0_10kpdticks/F')
C4_matched_PDPeak_PE_10k_30kpdticks = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_PE_10k_30kpdticks', C4_matched_PDPeak_PE_10k_30kpdticks, 'C4_matched_PDPeak_PE_10k_30kpdticks/F')
C4_matched_PDPeak_PE_30kpluspdticks = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_PE_30kpluspdticks', C4_matched_PDPeak_PE_30kpluspdticks, 'C4_matched_PDPeak_PE_30kpluspdticks/F')
C4_matched_PDPeak_Time = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_Time', C4_matched_PDPeak_Time, 'C4_matched_PDPeak_Time/F')
C4_matched_PDPeak_Time_100_1200PE = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_Time_100_1200PE', C4_matched_PDPeak_Time_100_1200PE, 'C4_matched_PDPeak_Time_100_1200PE/F')
C4_matched_PDPeak_Time_above_1200PE = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_Time_above_1200PE', C4_matched_PDPeak_Time_above_1200PE, 'C4_matched_PDPeak_Time_above_1200PE/F')

n_evt = 0
my_nTriggers[0] = 0

# Add two channels per trigger record
# not across multiple triggers
current_trigger = -1
C1_ch1_peaks, C2_ch1_peaks, C3_ch1_peaks, C4_ch1_peaks = [], [], [], []
C1_ch2_peaks, C2_ch2_peaks, C3_ch2_peaks, C4_ch2_peaks = [], [], [], []
C1_chs_dt_all_evts, C2_chs_dt_all_evts, C3_chs_dt_all_evts, C4_chs_dt_all_evts = [], [], [], []
C1_all_evts, C2_all_evts, C3_all_evts, C4_all_evts = [], [], [], []
C1_per_evt_large_signal, C2_per_evt_large_signal, C3_per_evt_large_signal, C4_per_evt_large_signal = [], [], [], []
C1_all_evts_large_signals_gaptime, C2_all_evts_large_signals_gaptime, C3_all_evts_large_signals_gaptime, C4_all_evts_large_signals_gaptime = [], [], [], []

""" loop on all h5 files """
for files in sys.argv[1:]:
    print(files)
    with tab.open_file(files, "r") as f:
        infos = f.get_node('/',name='infos',classname='Table')
        n_evt += infos.read(0,1,field='n_evt')[0].astype(int)

        """ read the pds peaks table """
        t_pds_peaks = f.root.pds_peaks
        d_pds_peaks = t_pds_peaks.read()
        """ read the single hits table """
        t_sh = f.root.single_hits
        d_sh = t_sh.read()
        """ read the 3d tracks table """
        t_trk3d = f.root.tracks3d
        d_trk3d = t_trk3d.read()

        """ Find PDS peaks in all events """
        for row_pdpeak in d_pds_peaks:

            if row_pdpeak['trigger'] != current_trigger:
                """ A new trigger record """
                my_nTriggers[0] = my_nTriggers[0] + 1

                """ Set to new trigger """
                current_trigger = row_pdpeak['trigger']

                """ Compare XA channels in previous trigger record if exists """
                """ Last trigger will not be processed                       """
                """ If one channel has peaks, the other doesn't, discard     """

                if len(C1_ch1_peaks) != 0 and len(C1_ch2_peaks) != 0:

                    for ch1 in range(len(C1_ch1_peaks)):
                        for ch2 in range(len(C1_ch2_peaks)):
                            # For all trigger records: store dt and plot it
                            C1_chs_dt_all_evts.append((abs(C1_ch1_peaks[ch1][1] - C1_ch2_peaks[ch2][1])))

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C1_ch1_peaks[ch1][1] - C1_ch2_peaks[ch2][1]) < t_coincidence:
                                C1_trigger_totPE = C1_ch1_peaks[ch1][0]/C1_ch1_adcgain + C1_ch2_peaks[ch2][0]/C1_ch2_adcgain
                                C1_trigger_time = (C1_ch1_peaks[ch1][1] + C1_ch2_peaks[ch2][1])/2
                                C1_all_evts.append((C1_trigger_totPE, C1_trigger_time)) # 150ADC cut default in lardon
                                if C1_trigger_totPE > n_cap_pd_minPE and C1_ch1_peaks[ch1][0] < pd_adc_saturation_cut and C1_ch2_peaks[ch2][0] < pd_adc_saturation_cut:
                                    C1_per_evt_large_signal.append((C1_trigger_totPE, C1_trigger_time))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C1_per_evt_large_signal)):

                        # store gap time between large pd signals
                        C1_all_evts_large_signals_gaptime.append(C1_per_evt_large_signal[ipdpeak][1] - C1_per_evt_large_signal[ipdpeak-1][1])

                        # output branch to root file
                        if C1_per_evt_large_signal[ipdpeak][1] >= 0 and C1_per_evt_large_signal[ipdpeak][1] < 65625: # DAQ window
                            C1_matched_PDPeak_PE[0] = C1_per_evt_large_signal[ipdpeak][0]/pescaleC1
                        else:
                            C1_matched_PDPeak_PE[0] = -9999
                        # store time sliced PE
                        if C1_per_evt_large_signal[ipdpeak][1] >= 0 and C1_per_evt_large_signal[ipdpeak][1] < 10000: # pd ticks
                            C1_matched_PDPeak_PE_0_10kpdticks[0] = C1_per_evt_large_signal[ipdpeak][0]/pescaleC1
                            C1_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C1_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C1_per_evt_large_signal[ipdpeak][1] >= 10000 and C1_per_evt_large_signal[ipdpeak][1] < 30000:
                            C1_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C1_matched_PDPeak_PE_10k_30kpdticks[0] = C1_per_evt_large_signal[ipdpeak][0]/pescaleC1
                            C1_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C1_per_evt_large_signal[ipdpeak][1] >= 30000 and C1_per_evt_large_signal[ipdpeak][1] < 65625:
                            C1_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C1_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C1_matched_PDPeak_PE_30kpluspdticks[0] = C1_per_evt_large_signal[ipdpeak][0]/pescaleC1

                        # store timing
                        if C1_matched_PDPeak_PE[0] >= 100 and C1_matched_PDPeak_PE[0] < 2100:
                            C1_matched_PDPeak_Time[0] = C1_per_evt_large_signal[ipdpeak][1]
                            if C1_matched_PDPeak_PE[0] < 1200:
                                C1_matched_PDPeak_Time_100_1200PE[0] = C1_per_evt_large_signal[ipdpeak][1]
                                C1_matched_PDPeak_Time_above_1200PE[0] = -9999
                            else: #>=1200
                                C1_matched_PDPeak_Time_above_1200PE[0] = C1_per_evt_large_signal[ipdpeak][1]
                                C1_matched_PDPeak_Time_100_1200PE[0] = -9999
                        else:
                            C1_matched_PDPeak_Time[0] = -9999
                            C1_matched_PDPeak_Time_100_1200PE[0] = -9999
                            C1_matched_PDPeak_Time_above_1200PE[0] = -9999 # assign -1 if not in the PE region - in principle shouldn't plot as an entry

                        # Filled matched PD peak pe
                        myMatchedC1Peaks.Fill()

                if len(C2_ch1_peaks) != 0 and len(C2_ch2_peaks) != 0:

                    for ch1 in range(len(C2_ch1_peaks)):
                        for ch2 in range(len(C2_ch2_peaks)):
                            # For all trigger records: store dt and plot it
                            C2_chs_dt_all_evts.append((abs(C2_ch1_peaks[ch1][1] - C2_ch2_peaks[ch2][1])))

                            """ Only add ADC close in time  """
                            """ Use the avg. time of the 2 channels   """
                            if abs(C2_ch1_peaks[ch1][1] - C2_ch2_peaks[ch2][1]) < t_coincidence:
                                C2_trigger_totPE = C2_ch1_peaks[ch1][0]/C2_ch1_adcgain + C2_ch2_peaks[ch2][0]/C2_ch2_adcgain
                                C2_trigger_time = (C2_ch1_peaks[ch1][1] + C2_ch2_peaks[ch2][1])/2
                                C2_all_evts.append((C2_trigger_totPE, C2_trigger_time))
                                if C2_trigger_totPE > n_cap_pd_minPE and C2_ch1_peaks[ch1][0] < pd_adc_saturation_cut and C2_ch2_peaks[ch2][0] < pd_adc_saturation_cut:
                                    C2_per_evt_large_signal.append((C2_trigger_totPE, C2_trigger_time))

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C2_per_evt_large_signal)):

                        """ For PD: only look at large peaks """
                        # store gap time between large pd signals
                        C2_all_evts_large_signals_gaptime.append(C2_per_evt_large_signal[ipdpeak][1] - C2_per_evt_large_signal[ipdpeak-1][1])

                        # output branch to root file
                        if C2_per_evt_large_signal[ipdpeak][1] >= 0 and C2_per_evt_large_signal[ipdpeak][1] < 65625: # DAQ window
                            C2_matched_PDPeak_PE[0] = C2_per_evt_large_signal[ipdpeak][0]/pescaleC2
                        else:
                            C2_matched_PDPeak_PE[0] = -9999
                        # store time sliced PE
                        if C2_per_evt_large_signal[ipdpeak][1] >= 0 and C2_per_evt_large_signal[ipdpeak][1] < 10000: # pd ticks
                            C2_matched_PDPeak_PE_0_10kpdticks[0] = C2_per_evt_large_signal[ipdpeak][0]/pescaleC2
                            C2_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C2_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C2_per_evt_large_signal[ipdpeak][1] >= 10000 and C2_per_evt_large_signal[ipdpeak][1] < 30000:
                            C2_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C2_matched_PDPeak_PE_10k_30kpdticks[0] = C2_per_evt_large_signal[ipdpeak][0]/pescaleC2
                            C2_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C2_per_evt_large_signal[ipdpeak][1] >= 30000 and C2_per_evt_large_signal[ipdpeak][1] < 65625:
                            C2_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C2_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C2_matched_PDPeak_PE_30kpluspdticks[0] = C2_per_evt_large_signal[ipdpeak][0]/pescaleC2

                        # store timing
                        if C2_matched_PDPeak_PE[0] >= 100 and C2_matched_PDPeak_PE[0] < 2100:
                            C2_matched_PDPeak_Time[0] = C2_per_evt_large_signal[ipdpeak][1]
                            if C2_matched_PDPeak_PE[0] < 1200:
                                C2_matched_PDPeak_Time_100_1200PE[0] = C2_per_evt_large_signal[ipdpeak][1]
                                C2_matched_PDPeak_Time_above_1200PE[0] = -9999
                            else: #>=1200
                                C2_matched_PDPeak_Time_above_1200PE[0] = C2_per_evt_large_signal[ipdpeak][1]
                                C2_matched_PDPeak_Time_100_1200PE[0] = -9999
                        else:
                            C2_matched_PDPeak_Time[0] = -9999
                            C2_matched_PDPeak_Time_100_1200PE[0] = -9999
                            C2_matched_PDPeak_Time_above_1200PE[0] = -9999 # assign -1 if not in the PE region - in principle shouldn't plot as an entry

                        # Filled matched PD peak pe
                        myMatchedC2Peaks.Fill()


                if len(C3_ch1_peaks) != 0 and len(C3_ch2_peaks) != 0:

                    for ch1 in range(len(C3_ch1_peaks)):
                        for ch2 in range(len(C3_ch2_peaks)):
                            # For all trigger records: store dt and plot it
                            C3_chs_dt_all_evts.append((abs(C3_ch1_peaks[ch1][1] - C3_ch2_peaks[ch2][1])))

                            """ Only add ADC close in time    """
                            """ Use the avg. time of the 2 channels    """
                            if abs(C3_ch1_peaks[ch1][1] - C3_ch2_peaks[ch2][1]) < t_coincidence:
                                C3_trigger_totPE = C3_ch1_peaks[ch1][0]/C3_ch1_adcgain + C3_ch2_peaks[ch2][0]/C3_ch2_adcgain
                                C3_trigger_time = (C3_ch1_peaks[ch1][1] + C3_ch2_peaks[ch2][1])/2
                                C3_all_evts.append((C3_trigger_totPE, C3_trigger_time)) # 150ADC cut default in lardon
                                if C3_trigger_totPE > n_cap_pd_minPE and C3_ch1_peaks[ch1][0] < pd_adc_saturation_cut and C3_ch2_peaks[ch2][0] < pd_adc_saturation_cut:
                                    C3_per_evt_large_signal.append((C3_trigger_totPE, C3_trigger_time))


                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C3_per_evt_large_signal)):

                        """ For PD: only look at large peaks """
                        # store gap time between large pd signals
                        C3_all_evts_large_signals_gaptime.append(C3_per_evt_large_signal[ipdpeak][1] - C3_per_evt_large_signal[ipdpeak-1][1])

                        # output branch to root file
                        if C3_per_evt_large_signal[ipdpeak][1] >= 0 and C3_per_evt_large_signal[ipdpeak][1] < 65625: # DAQ window
                            C3_matched_PDPeak_PE[0] = C3_per_evt_large_signal[ipdpeak][0]/pescaleC3
                        else:
                            C3_matched_PDPeak_PE[0] = -9999
                        # store time sliced PE
                        if C3_per_evt_large_signal[ipdpeak][1] >= 0 and C3_per_evt_large_signal[ipdpeak][1] < 10000: # pd ticks
                            C3_matched_PDPeak_PE_0_10kpdticks[0] = C3_per_evt_large_signal[ipdpeak][0]/pescaleC3
                            C3_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C3_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C3_per_evt_large_signal[ipdpeak][1] >= 10000 and C3_per_evt_large_signal[ipdpeak][1] < 30000:
                            C3_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C3_matched_PDPeak_PE_10k_30kpdticks[0] = C3_per_evt_large_signal[ipdpeak][0]/pescaleC3
                            C3_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C3_per_evt_large_signal[ipdpeak][1] >= 30000 and C3_per_evt_large_signal[ipdpeak][1] < 65625:
                            C3_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C3_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C3_matched_PDPeak_PE_30kpluspdticks[0] = C3_per_evt_large_signal[ipdpeak][0]/pescaleC3

                        # store timing
                        if C3_matched_PDPeak_PE[0] >= 100 and C3_matched_PDPeak_PE[0] < 2100:
                            C3_matched_PDPeak_Time[0] = C3_per_evt_large_signal[ipdpeak][1]
                            if C3_matched_PDPeak_PE[0] < 1200:
                                C3_matched_PDPeak_Time_100_1200PE[0] = C3_per_evt_large_signal[ipdpeak][1]
                                C3_matched_PDPeak_Time_above_1200PE[0] = -9999
                            else: #>=1200
                                C3_matched_PDPeak_Time_above_1200PE[0] = C3_per_evt_large_signal[ipdpeak][1]
                                C3_matched_PDPeak_Time_100_1200PE[0] = -9999
                        else:
                            C3_matched_PDPeak_Time[0] = -9999
                            C3_matched_PDPeak_Time_100_1200PE[0] = -9999
                            C3_matched_PDPeak_Time_above_1200PE[0] = -9999 # assign -1 if not in the PE region - in principle shouldn't plot as an entry

                        # Filled matched PD peak pe
                        myMatchedC3Peaks.Fill()


                if len(C4_ch1_peaks) != 0 and len(C4_ch2_peaks) != 0:
                    for ch1 in range(len(C4_ch1_peaks)):
                        for ch2 in range(len(C4_ch2_peaks)):
                            # For all trigger records: store dt and plot it
                            C4_chs_dt_all_evts.append((abs(C4_ch1_peaks[ch1][1] - C4_ch2_peaks[ch2][1])))

                            """ Only add ADC close in time   """
                            """ Use the avg. time of the 2 channels       """
                            if abs(C4_ch1_peaks[ch1][1] - C4_ch2_peaks[ch2][1]) < t_coincidence:
                                C4_trigger_totPE = C4_ch1_peaks[ch1][0]/C4_ch1_adcgain + C4_ch2_peaks[ch2][0]/C4_ch2_adcgain
                                C4_trigger_time = (C4_ch1_peaks[ch1][1] + C4_ch2_peaks[ch2][1])/2
                                C4_all_evts.append((C4_trigger_totPE, C4_trigger_time))
                                if C4_trigger_totPE > n_cap_pd_minPE and C4_ch1_peaks[ch1][0] < pd_adc_saturation_cut and C4_ch2_peaks[ch2][0] < pd_adc_saturation_cut:
                                    C4_per_evt_large_signal.append((C4_trigger_totPE, C4_trigger_time))

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C4_per_evt_large_signal)):

                        """ For PD: only look at large peaks  """
                        # store gap time between large pd signals
                        C4_all_evts_large_signals_gaptime.append(C4_per_evt_large_signal[ipdpeak][1] - C4_per_evt_large_signal[ipdpeak-1][1])

                        # output branch to root file
                        if C4_per_evt_large_signal[ipdpeak][1] >= 0 and C4_per_evt_large_signal[ipdpeak][1] < 65625: # DAQ window
                            C4_matched_PDPeak_PE[0] = C4_per_evt_large_signal[ipdpeak][0]/pescaleC4
                        else:
                            C4_matched_PDPeak_PE[0] = -9999
                        # store time sliced PE
                        if C4_per_evt_large_signal[ipdpeak][1] >= 0 and C4_per_evt_large_signal[ipdpeak][1] < 10000: # pd ticks
                            C4_matched_PDPeak_PE_0_10kpdticks[0] = C4_per_evt_large_signal[ipdpeak][0]/pescaleC4
                            C4_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C4_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C4_per_evt_large_signal[ipdpeak][1] >= 10000 and C4_per_evt_large_signal[ipdpeak][1] < 30000:
                            C4_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C4_matched_PDPeak_PE_10k_30kpdticks[0] = C4_per_evt_large_signal[ipdpeak][0]/pescaleC4
                            C4_matched_PDPeak_PE_30kpluspdticks[0] = -9999
                        elif C4_per_evt_large_signal[ipdpeak][1] >= 30000 and C4_per_evt_large_signal[ipdpeak][1] < 65625:
                            C4_matched_PDPeak_PE_0_10kpdticks[0] = -9999
                            C4_matched_PDPeak_PE_10k_30kpdticks[0] = -9999
                            C4_matched_PDPeak_PE_30kpluspdticks[0] = C4_per_evt_large_signal[ipdpeak][0]/pescaleC4

                        # store timing
                        if C4_matched_PDPeak_PE[0] >= 100 and C4_matched_PDPeak_PE[0] < 2100:
                            C4_matched_PDPeak_Time[0] = C4_per_evt_large_signal[ipdpeak][1]
                            if C4_matched_PDPeak_PE[0] < 1200:
                                C4_matched_PDPeak_Time_100_1200PE[0] = C4_per_evt_large_signal[ipdpeak][1]
                                C4_matched_PDPeak_Time_above_1200PE[0] = -9999
                            else: #>=1200
                                C4_matched_PDPeak_Time_above_1200PE[0] = C4_per_evt_large_signal[ipdpeak][1]
                                C4_matched_PDPeak_Time_100_1200PE[0] = -9999
                        else:
                            C4_matched_PDPeak_Time[0] = -9999
                            C4_matched_PDPeak_Time_100_1200PE[0] = -9999
                            C4_matched_PDPeak_Time_above_1200PE[0] = -9999 # assign -1 if not in the PE region - in principle shouldn't plot as an entry

                        # Filled matched PD peak pe
                        myMatchedC4Peaks.Fill()

                """ Clear arrays per trigger """
                C1_ch1_peaks, C1_ch2_peaks = [], []
                C2_ch1_peaks, C2_ch2_peaks = [], []
                C3_ch1_peaks, C3_ch2_peaks = [], []
                C4_ch1_peaks, C4_ch2_peaks = [], []
                C1_per_evt_large_signal, C2_per_evt_large_signal, C3_per_evt_large_signal, C4_per_evt_large_signal = [], [], [], []

            # Back to main pd peak for loop
            """ Always pd rows in current trigger """
            """ Collect all pd peaks in the trigger within 1ms, some cosmic runs has 4ms long wfm, don't need that for comparison """
            if row_pdpeak['max_t'] <= max_time_tick_pd:
                if row_pdpeak['channel'] == C1_ch1: C1_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C1_ch2: C1_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C2_ch1: C2_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C2_ch2: C2_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C3_ch1: C3_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C3_ch2: C3_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C4_ch1: C4_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
                if row_pdpeak['channel'] == C4_ch2: C4_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))


# Filled matched PD peak pe
myTotTriggers.Fill()

f_out.cd()
myTotTriggers.Write()
myMatchedC1Peaks.Write()
myMatchedC2Peaks.Write()
myMatchedC3Peaks.Write()
myMatchedC4Peaks.Write()

""" PD plots """
# Only plot time
time_pdpeaks_C1 = [(x[1]) for x in C1_all_evts ]
time_pdpeaks_C2 = [(x[1]) for x in C2_all_evts ]
time_pdpeaks_C3 = [(x[1]) for x in C3_all_evts ]
time_pdpeaks_C4 = [(x[1]) for x in C4_all_evts ]
# Only plot adc
PE_pdpeaks_C1 = [(x[0]) for x in C1_all_evts ]
PE_pdpeaks_C2 = [(x[0]) for x in C2_all_evts ]
PE_pdpeaks_C3 = [(x[0]) for x in C3_all_evts ]
PE_pdpeaks_C4 = [(x[0]) for x in C4_all_evts ]
# dt
C1_chs_dt = [(x) for x in C1_chs_dt_all_evts]
C2_chs_dt = [(x) for x in C2_chs_dt_all_evts]
C3_chs_dt = [(x) for x in C3_chs_dt_all_evts]
C4_chs_dt = [(x) for x in C4_chs_dt_all_evts]
# gap time b/t peaks
C1_largepdpeaks_timegap = [(x) for x in C1_all_evts_large_signals_gaptime]
C2_largepdpeaks_timegap = [(x) for x in C2_all_evts_large_signals_gaptime]
C3_largepdpeaks_timegap = [(x) for x in C3_all_evts_large_signals_gaptime]
C4_largepdpeaks_timegap = [(x) for x in C4_all_evts_large_signals_gaptime]

# tpc matched
print('Tot triggers:', my_nTriggers[0])
print('Tot evts:', n_evt)


plt.hist(PE_pdpeaks_C4, range=(0,max_PE_pd), bins=350)
plt.xlabel('C4 peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C4_tot_PE.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C3, range=(0,max_PE_pd), bins=350)
plt.xlabel('C3 peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C3_tot_PE.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C2, range=(0,max_PE_pd), bins=350)
plt.xlabel('C2 peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C2_tot_PE.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C1, range=(0,max_PE_pd), bins=350)
plt.xlabel('C1 peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C1_tot_PE.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C4, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C4 peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C4_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C3, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C3 peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C3_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C2, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C2 peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C2_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C1, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C1 peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C1_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C1_chs_dt, range=(0,100), bins=100)
plt.xlabel('C1 |t_ch1 - t_ch2| [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_C1_chs_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C2_chs_dt, range=(0,100), bins=100)
plt.xlabel('C2 |t_ch1 - t_ch2| [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_C2_chs_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C3_chs_dt, range=(0,100), bins=100)
plt.xlabel('C3 |t_ch1 - t_ch2| [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_C3_chs_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C4_chs_dt, range=(0,100), bins=100)
plt.xlabel('C4 |t_ch1 - t_ch2| [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_C4_chs_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C1_largepdpeaks_timegap, range=(0,9000), bins=1000)
plt.xlabel('C1 large pd peak dt [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C1_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C2_largepdpeaks_timegap, range=(0,9000), bins=1000)
plt.xlabel('C2 large pd peak dt [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C2_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C3_largepdpeaks_timegap, range=(0,9000), bins=1000)
plt.xlabel('C3 large pd peak dt [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C3_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C4_largepdpeaks_timegap, range=(0,9000), bins=1000)
plt.xlabel('C4 large pd peak dt [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C4_dt.pdf")
plt.clf() # important to clear figure
plt.close()
#f_out = TFile('results/pdpeaks_C4_time_tpc_matched.root', 'RECREATE')
#tuple = plt.hist(time_pdpeaks_C4_tpc_matched, range=(0,max_time_tick_pd), bins=625)
#d = ROOT.RDataFrame("myTree", "results/pdpeaks_C4_time_tpc_matched.root");
#myHisto = d.Histo1D(("h", "h", 68, 0, max_time_tick_pd), "tuple")
#myHisto.Write()
#f_out.Close()
