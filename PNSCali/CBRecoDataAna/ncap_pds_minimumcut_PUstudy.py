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

########################################################################################################################
# understand pile up peaks in data within 1us / 10 us, i.e., typical PD signal recovery time, where cosmic large signal precedes n signal
# compare PE distribution from secondary peak in PU events, vs PE of non-PU events
########################################################################################################################

#run_number = 25004 # cosmic run
#run_number = 25066 # cosmic run
#run_number = 25078 # cosmic run
#run_number = 25084 # cosmic run
#run_number = 25086 # cosmic run

#run_number = 25036 # PNS+TPC+PDS run
#run_number = 25050 # PNS+TPC+PDS run? --> do not use
#run_number = 25068 # PNS+TPC+PDS run
run_number = 25071 # PNS+TPC+PDS run

PU_dt_window = 625 # ticks, 1us 62.5 ticks, 10us 625 ticks

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
n_cap_pd_minPE = 100
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
f_out = TFile('{0}/CBAna_PUstudy.root'.format(out_path), 'RECREATE')

myTotTriggers = TTree('myTotTriggers', 'myTotTriggers')
myC1PeaksPU   = TTree('myC1PeaksPU', 'myC1PeaksPU')
myC1PeaksNoPU = TTree('myC1PeaksNoPU', 'myC1PeaksNoPU')
myC2PeaksPU   = TTree('myC2PeaksPU', 'myC2PeaksPU')
myC2PeaksNoPU = TTree('myC2PeaksNoPU', 'myC2PeaksNoPU')
myC3PeaksPU   = TTree('myC3PeaksPU', 'myC3PeaksPU')
myC3PeaksNoPU = TTree('myC3PeaksNoPU', 'myC3PeaksNoPU')
myC4PeaksPU   = TTree('myC4PeaksPU', 'myC4PeaksPU')
myC4PeaksNoPU = TTree('myC4PeaksNoPU', 'myC4PeaksNoPU')

my_nTriggers = array('i', [0])
myTotTriggers.Branch('my_nTriggers', my_nTriggers, 'my_nTriggers/I')

C1_matched_PDPeak_PE_PU   = array('f', [0.0])
C1_matched_PDPeak_PE_NoPU = array('f', [0.0])
myC1PeaksPU.Branch('C1_matched_PDPeak_PE_PU', C1_matched_PDPeak_PE_PU, 'C1_matched_PDPeak_PE_PU/F')
myC1PeaksNoPU.Branch('C1_matched_PDPeak_PE_NoPU', C1_matched_PDPeak_PE_NoPU, 'C1_matched_PDPeak_PE_NoPU/F')

C2_matched_PDPeak_PE_PU   = array('f', [0.0])
C2_matched_PDPeak_PE_NoPU = array('f', [0.0])
myC2PeaksPU.Branch('C2_matched_PDPeak_PE_PU', C2_matched_PDPeak_PE_PU, 'C2_matched_PDPeak_PE_PU/F')
myC2PeaksNoPU.Branch('C2_matched_PDPeak_PE_NoPU', C2_matched_PDPeak_PE_NoPU, 'C2_matched_PDPeak_PE_NoPU/F')

C3_matched_PDPeak_PE_PU   = array('f', [0.0])
C3_matched_PDPeak_PE_NoPU = array('f', [0.0])
myC3PeaksPU.Branch('C3_matched_PDPeak_PE_PU', C3_matched_PDPeak_PE_PU, 'C3_matched_PDPeak_PE_PU/F')
myC3PeaksNoPU.Branch('C3_matched_PDPeak_PE_NoPU', C3_matched_PDPeak_PE_NoPU, 'C3_matched_PDPeak_PE_NoPU/F')

C4_matched_PDPeak_PE_PU   = array('f', [0.0])
C4_matched_PDPeak_PE_NoPU = array('f', [0.0])
myC4PeaksPU.Branch('C4_matched_PDPeak_PE_PU', C4_matched_PDPeak_PE_PU, 'C4_matched_PDPeak_PE_PU/F')
myC4PeaksNoPU.Branch('C4_matched_PDPeak_PE_NoPU', C4_matched_PDPeak_PE_NoPU, 'C4_matched_PDPeak_PE_NoPU/F')

n_evt = 0
my_nTriggers[0] = 0

# Add two channels per trigger record
# not across multiple triggers
current_trigger = -1
C1_ch1_peaks, C2_ch1_peaks, C3_ch1_peaks, C4_ch1_peaks = [], [], [], []
C1_ch2_peaks, C2_ch2_peaks, C3_ch2_peaks, C4_ch2_peaks = [], [], [], []
C1_ch1_peaks_NoPU, C1_ch1_peaks_PU, C1_ch2_peaks_NoPU, C1_ch2_peaks_PU = [], [], [], []
C2_ch1_peaks_NoPU, C2_ch1_peaks_PU, C2_ch2_peaks_NoPU, C2_ch2_peaks_PU = [], [], [], []
C3_ch1_peaks_NoPU, C3_ch1_peaks_PU, C3_ch2_peaks_NoPU, C3_ch2_peaks_PU = [], [], [], []
C4_ch1_peaks_NoPU, C4_ch1_peaks_PU, C4_ch2_peaks_NoPU, C4_ch2_peaks_PU = [], [], [], []
C1_per_evt_large_signal_PU, C2_per_evt_large_signal_PU, C3_per_evt_large_signal_PU, C4_per_evt_large_signal_PU = [], [], [], []
C1_per_evt_large_signal_NoPU, C2_per_evt_large_signal_NoPU, C3_per_evt_large_signal_NoPU, C4_per_evt_large_signal_NoPU = [], [], [], []

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

                    # Here we need to separate peaks in each channel
                    # PU events (i.e. more than 1 peak), and take the second/later peak

                    # control plot?: distribution of # of peaks per trig record

                    C1_ch1_PUpeaktag = np.zeros((len(C1_ch1_peaks),), dtype=np.int32)
                    C1_ch2_PUpeaktag = np.zeros((len(C1_ch2_peaks),), dtype=np.int32)

                    # check if peaks close in time exist
                    for ch1ipeak in range(len(C1_ch1_peaks)-1):
                        for ch1jpeak in range(ch1ipeak+1,len(C1_ch1_peaks)):
                            if ( abs( C1_ch1_peaks[ch1ipeak][1] - C1_ch1_peaks[ch1jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C1_ch1_peaks[ch1ipeak][1] < C1_ch1_peaks[ch1jpeak][1]:
                                    C1_ch1_PUpeaktag[ch1jpeak] = 1
                                else:
                                    C1_ch1_PUpeaktag[ch1ipeak] = 1

                    # build PU and non PU collection for ch1
                    for ch1ipeak in range(len(C1_ch1_peaks)):
                        if C1_ch1_PUpeaktag[ch1ipeak] == 1:
                            C1_ch1_peaks_PU.append((C1_ch1_peaks[ch1ipeak][0], C1_ch1_peaks[ch1ipeak][1], C1_ch1_peaks[ch1ipeak][2]))
                        else:
                            C1_ch1_peaks_NoPU.append((C1_ch1_peaks[ch1ipeak][0], C1_ch1_peaks[ch1ipeak][1], C1_ch1_peaks[ch1ipeak][2]))

                    # copy for ch2
                    # check if peaks close in time exist
                    for ch2ipeak in range(len(C1_ch2_peaks)-1):
                        for ch2jpeak in range(ch2ipeak+1,len(C1_ch2_peaks)):
                            if ( abs( C1_ch2_peaks[ch2ipeak][1] - C1_ch2_peaks[ch2jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C1_ch2_peaks[ch2ipeak][1] < C1_ch2_peaks[ch2jpeak][1]:
                                    C1_ch2_PUpeaktag[ch2jpeak] = 1
                                else:
                                    C1_ch2_PUpeaktag[ch2ipeak] = 1

                    # Build PU and non PU collection
                    for ch2ipeak in range(len(C1_ch2_peaks)):
                        if C1_ch2_PUpeaktag[ch2ipeak] == 1:
                            C1_ch2_peaks_PU.append((C1_ch2_peaks[ch2ipeak][0], C1_ch2_peaks[ch2ipeak][1], C1_ch2_peaks[ch2ipeak][2]))
                        else:
                            C1_ch2_peaks_NoPU.append((C1_ch2_peaks[ch2ipeak][0], C1_ch2_peaks[ch2ipeak][1], C1_ch2_peaks[ch2ipeak][2]))

                    # Here run the normal selection using the newly built PU collection
                    for ch1 in range(len(C1_ch1_peaks_PU)):
                        for ch2 in range(len(C1_ch2_peaks_PU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C1_ch1_peaks_PU[ch1][1] - C1_ch2_peaks_PU[ch2][1]) < t_coincidence: # actual selection!!!
                                C1_trigger_totPE_PU = C1_ch1_peaks_PU[ch1][0]/C1_ch1_adcgain + C1_ch2_peaks_PU[ch2][0]/C1_ch2_adcgain
                                C1_trigger_time_PU = (C1_ch1_peaks_PU[ch1][1] + C1_ch2_peaks_PU[ch2][1])/2
                                if C1_trigger_totPE_PU > n_cap_pd_minPE and C1_ch1_peaks_PU[ch1][0] < pd_adc_saturation_cut and C1_ch2_peaks_PU[ch2][0] < pd_adc_saturation_cut:
                                    C1_per_evt_large_signal_PU.append((C1_trigger_totPE_PU, C1_trigger_time_PU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C1_per_evt_large_signal_PU)):

                        # output branch to root file
                        C1_matched_PDPeak_PE_PU[0] = C1_per_evt_large_signal_PU[ipdpeak][0]/pescaleC1
                        # Filled matched PD peak pe
                        myC1PeaksPU.Fill()

                    # Here cp for noPU collection
                    for ch1 in range(len(C1_ch1_peaks_NoPU)):
                        for ch2 in range(len(C1_ch2_peaks_NoPU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C1_ch1_peaks_NoPU[ch1][1] - C1_ch2_peaks_NoPU[ch2][1]) < t_coincidence: # actual selection!!!
                                C1_trigger_totPE_NoPU = C1_ch1_peaks_NoPU[ch1][0]/C1_ch1_adcgain + C1_ch2_peaks_NoPU[ch2][0]/C1_ch2_adcgain
                                C1_trigger_time_NoPU = (C1_ch1_peaks_NoPU[ch1][1] + C1_ch2_peaks_NoPU[ch2][1])/2
                                if C1_trigger_totPE_NoPU > n_cap_pd_minPE and C1_ch1_peaks_NoPU[ch1][0] < pd_adc_saturation_cut and C1_ch2_peaks_NoPU[ch2][0] < pd_adc_saturation_cut:
                                    C1_per_evt_large_signal_NoPU.append((C1_trigger_totPE_NoPU, C1_trigger_time_NoPU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C1_per_evt_large_signal_NoPU)):

                        # output branch to root file
                        C1_matched_PDPeak_PE_NoPU[0] = C1_per_evt_large_signal_NoPU[ipdpeak][0]/pescaleC1
                        # Filled matched PD peak pe
                        myC1PeaksNoPU.Fill()



                if len(C2_ch1_peaks) != 0 and len(C2_ch2_peaks) != 0:

                    C2_ch1_PUpeaktag = np.zeros((len(C2_ch1_peaks),), dtype=np.int32)
                    C2_ch2_PUpeaktag = np.zeros((len(C2_ch2_peaks),), dtype=np.int32)

                    # check if peaks close in time exist
                    for ch1ipeak in range(len(C2_ch1_peaks)-1):
                        for ch1jpeak in range(ch1ipeak+1,len(C2_ch1_peaks)):
                            if ( abs( C2_ch1_peaks[ch1ipeak][1] - C2_ch1_peaks[ch1jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C2_ch1_peaks[ch1ipeak][1] < C2_ch1_peaks[ch1jpeak][1]:
                                    C2_ch1_PUpeaktag[ch1jpeak] = 1
                                else:
                                    C2_ch1_PUpeaktag[ch1ipeak] = 1

                    # build PU and non PU collection for ch1
                    for ch1ipeak in range(len(C2_ch1_peaks)):
                        if C2_ch1_PUpeaktag[ch1ipeak] == 1:
                            C2_ch1_peaks_PU.append((C2_ch1_peaks[ch1ipeak][0], C2_ch1_peaks[ch1ipeak][1], C2_ch1_peaks[ch1ipeak][2]))
                        else:
                            C2_ch1_peaks_NoPU.append((C2_ch1_peaks[ch1ipeak][0], C2_ch1_peaks[ch1ipeak][1], C2_ch1_peaks[ch1ipeak][2]))

                    # copy for ch2
                    # check if peaks close in time exist
                    for ch2ipeak in range(len(C2_ch2_peaks)-1):
                        for ch2jpeak in range(ch2ipeak+1,len(C2_ch2_peaks)):
                            if ( abs( C2_ch2_peaks[ch2ipeak][1] - C2_ch2_peaks[ch2jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C2_ch2_peaks[ch2ipeak][1] < C2_ch2_peaks[ch2jpeak][1]:
                                    C2_ch2_PUpeaktag[ch2jpeak] = 1
                                else:
                                    C2_ch2_PUpeaktag[ch2ipeak] = 1

                    # Build PU and non PU collection
                    for ch2ipeak in range(len(C2_ch2_peaks)):
                        if C2_ch2_PUpeaktag[ch2ipeak] == 1:
                            C2_ch2_peaks_PU.append((C2_ch2_peaks[ch2ipeak][0], C2_ch2_peaks[ch2ipeak][1], C2_ch2_peaks[ch2ipeak][2]))
                        else:
                            C2_ch2_peaks_NoPU.append((C2_ch2_peaks[ch2ipeak][0], C2_ch2_peaks[ch2ipeak][1], C2_ch2_peaks[ch2ipeak][2]))

                    # Here run the normal selection using the newly built PU collection
                    for ch1 in range(len(C2_ch1_peaks_PU)):
                        for ch2 in range(len(C2_ch2_peaks_PU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C2_ch1_peaks_PU[ch1][1] - C2_ch2_peaks_PU[ch2][1]) < t_coincidence: # actual selection!!!
                                C2_trigger_totPE_PU = C2_ch1_peaks_PU[ch1][0]/C2_ch1_adcgain + C2_ch2_peaks_PU[ch2][0]/C2_ch2_adcgain
                                C2_trigger_time_PU = (C2_ch1_peaks_PU[ch1][1] + C2_ch2_peaks_PU[ch2][1])/2
                                if C2_trigger_totPE_PU > n_cap_pd_minPE and C2_ch1_peaks_PU[ch1][0] < pd_adc_saturation_cut and C2_ch2_peaks_PU[ch2][0] < pd_adc_saturation_cut:
                                    C2_per_evt_large_signal_PU.append((C2_trigger_totPE_PU, C2_trigger_time_PU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C2_per_evt_large_signal_PU)):

                        # output branch to root file
                        C2_matched_PDPeak_PE_PU[0] = C2_per_evt_large_signal_PU[ipdpeak][0]/pescaleC2
                        # Filled matched PD peak pe
                        myC2PeaksPU.Fill()

                    # Here cp for noPU collection
                    for ch1 in range(len(C2_ch1_peaks_NoPU)):
                        for ch2 in range(len(C2_ch2_peaks_NoPU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C2_ch1_peaks_NoPU[ch1][1] - C2_ch2_peaks_NoPU[ch2][1]) < t_coincidence: # actual selection!!!
                                C2_trigger_totPE_NoPU = C2_ch1_peaks_NoPU[ch1][0]/C2_ch1_adcgain + C2_ch2_peaks_NoPU[ch2][0]/C2_ch2_adcgain
                                C2_trigger_time_NoPU = (C2_ch1_peaks_NoPU[ch1][1] + C2_ch2_peaks_NoPU[ch2][1])/2
                                if C2_trigger_totPE_NoPU > n_cap_pd_minPE and C2_ch1_peaks_NoPU[ch1][0] < pd_adc_saturation_cut and C2_ch2_peaks_NoPU[ch2][0] < pd_adc_saturation_cut:
                                    C2_per_evt_large_signal_NoPU.append((C2_trigger_totPE_NoPU, C2_trigger_time_NoPU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C2_per_evt_large_signal_NoPU)):

                        # output branch to root file
                        C2_matched_PDPeak_PE_NoPU[0] = C2_per_evt_large_signal_NoPU[ipdpeak][0]/pescaleC2
                        # Filled matched PD peak pe
                        myC2PeaksNoPU.Fill()


                if len(C3_ch1_peaks) != 0 and len(C3_ch2_peaks) != 0:

                    C3_ch1_PUpeaktag = np.zeros((len(C3_ch1_peaks),), dtype=np.int32)
                    C3_ch2_PUpeaktag = np.zeros((len(C3_ch2_peaks),), dtype=np.int32)

                    # check if peaks close in time exist
                    for ch1ipeak in range(len(C3_ch1_peaks)-1):
                        for ch1jpeak in range(ch1ipeak+1,len(C3_ch1_peaks)):
                            if ( abs( C3_ch1_peaks[ch1ipeak][1] - C3_ch1_peaks[ch1jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C3_ch1_peaks[ch1ipeak][1] < C3_ch1_peaks[ch1jpeak][1]:
                                    C3_ch1_PUpeaktag[ch1jpeak] = 1
                                else:
                                    C3_ch1_PUpeaktag[ch1ipeak] = 1

                    # build PU and non PU collection for ch1
                    for ch1ipeak in range(len(C3_ch1_peaks)):
                        if C3_ch1_PUpeaktag[ch1ipeak] == 1:
                            C3_ch1_peaks_PU.append((C3_ch1_peaks[ch1ipeak][0], C3_ch1_peaks[ch1ipeak][1], C3_ch1_peaks[ch1ipeak][2]))
                        else:
                            C3_ch1_peaks_NoPU.append((C3_ch1_peaks[ch1ipeak][0], C3_ch1_peaks[ch1ipeak][1], C3_ch1_peaks[ch1ipeak][2]))

                    # copy for ch2
                    # check if peaks close in time exist
                    for ch2ipeak in range(len(C3_ch2_peaks)-1):
                        for ch2jpeak in range(ch2ipeak+1,len(C3_ch2_peaks)):
                            if ( abs( C3_ch2_peaks[ch2ipeak][1] - C3_ch2_peaks[ch2jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C3_ch2_peaks[ch2ipeak][1] < C3_ch2_peaks[ch2jpeak][1]:
                                    C3_ch2_PUpeaktag[ch2jpeak] = 1
                                else:
                                    C3_ch2_PUpeaktag[ch2ipeak] = 1

                    # Build PU and non PU collection
                    for ch2ipeak in range(len(C3_ch2_peaks)):
                        if C3_ch2_PUpeaktag[ch2ipeak] == 1:
                            C3_ch2_peaks_PU.append((C3_ch2_peaks[ch2ipeak][0], C3_ch2_peaks[ch2ipeak][1], C3_ch2_peaks[ch2ipeak][2]))
                        else:
                            C3_ch2_peaks_NoPU.append((C3_ch2_peaks[ch2ipeak][0], C3_ch2_peaks[ch2ipeak][1], C3_ch2_peaks[ch2ipeak][2]))

                    # Here run the normal selection using the newly built PU collection
                    for ch1 in range(len(C3_ch1_peaks_PU)):
                        for ch2 in range(len(C3_ch2_peaks_PU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C3_ch1_peaks_PU[ch1][1] - C3_ch2_peaks_PU[ch2][1]) < t_coincidence: # actual selection!!!
                                C3_trigger_totPE_PU = C3_ch1_peaks_PU[ch1][0]/C3_ch1_adcgain + C3_ch2_peaks_PU[ch2][0]/C3_ch2_adcgain
                                C3_trigger_time_PU = (C3_ch1_peaks_PU[ch1][1] + C3_ch2_peaks_PU[ch2][1])/2
                                if C3_trigger_totPE_PU > n_cap_pd_minPE and C3_ch1_peaks_PU[ch1][0] < pd_adc_saturation_cut and C3_ch2_peaks_PU[ch2][0] < pd_adc_saturation_cut:
                                    C3_per_evt_large_signal_PU.append((C3_trigger_totPE_PU, C3_trigger_time_PU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C3_per_evt_large_signal_PU)):

                        # output branch to root file
                        C3_matched_PDPeak_PE_PU[0] = C3_per_evt_large_signal_PU[ipdpeak][0]/pescaleC3
                        # Filled matched PD peak pe
                        myC3PeaksPU.Fill()

                    # Here cp for noPU collection
                    for ch1 in range(len(C3_ch1_peaks_NoPU)):
                        for ch2 in range(len(C3_ch2_peaks_NoPU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C3_ch1_peaks_NoPU[ch1][1] - C3_ch2_peaks_NoPU[ch2][1]) < t_coincidence: # actual selection!!!
                                C3_trigger_totPE_NoPU = C3_ch1_peaks_NoPU[ch1][0]/C3_ch1_adcgain + C3_ch2_peaks_NoPU[ch2][0]/C3_ch2_adcgain
                                C3_trigger_time_NoPU = (C3_ch1_peaks_NoPU[ch1][1] + C3_ch2_peaks_NoPU[ch2][1])/2
                                if C3_trigger_totPE_NoPU > n_cap_pd_minPE and C3_ch1_peaks_NoPU[ch1][0] < pd_adc_saturation_cut and C3_ch2_peaks_NoPU[ch2][0] < pd_adc_saturation_cut:
                                    C3_per_evt_large_signal_NoPU.append((C3_trigger_totPE_NoPU, C3_trigger_time_NoPU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C3_per_evt_large_signal_NoPU)):

                        # output branch to root file
                        C3_matched_PDPeak_PE_NoPU[0] = C3_per_evt_large_signal_NoPU[ipdpeak][0]/pescaleC3
                        # Filled matched PD peak pe
                        myC3PeaksNoPU.Fill()


                if len(C4_ch1_peaks) != 0 and len(C4_ch2_peaks) != 0:

                    C4_ch1_PUpeaktag = np.zeros((len(C4_ch1_peaks),), dtype=np.int32)
                    C4_ch2_PUpeaktag = np.zeros((len(C4_ch2_peaks),), dtype=np.int32)

                    # check if peaks close in time exist
                    for ch1ipeak in range(len(C4_ch1_peaks)-1):
                        for ch1jpeak in range(ch1ipeak+1,len(C4_ch1_peaks)):
                            if ( abs( C4_ch1_peaks[ch1ipeak][1] - C4_ch1_peaks[ch1jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C4_ch1_peaks[ch1ipeak][1] < C4_ch1_peaks[ch1jpeak][1]:
                                    C4_ch1_PUpeaktag[ch1jpeak] = 1
                                else:
                                    C4_ch1_PUpeaktag[ch1ipeak] = 1

                    # build PU and non PU collection for ch1
                    for ch1ipeak in range(len(C4_ch1_peaks)):
                        if C4_ch1_PUpeaktag[ch1ipeak] == 1:
                            C4_ch1_peaks_PU.append((C4_ch1_peaks[ch1ipeak][0], C4_ch1_peaks[ch1ipeak][1], C4_ch1_peaks[ch1ipeak][2]))
                        else:
                            C4_ch1_peaks_NoPU.append((C4_ch1_peaks[ch1ipeak][0], C4_ch1_peaks[ch1ipeak][1], C4_ch1_peaks[ch1ipeak][2]))

                    # copy for ch2
                    # check if peaks close in time exist
                    for ch2ipeak in range(len(C4_ch2_peaks)-1):
                        for ch2jpeak in range(ch2ipeak+1,len(C4_ch2_peaks)):
                            if ( abs( C4_ch2_peaks[ch2ipeak][1] - C4_ch2_peaks[ch2jpeak][1] ) ) < PU_dt_window:
                                # tag the peak as a PU peak if it sits on a previous one's tail
                                # the original time stamp was stored in increasing time, but be extra safe here
                                if C4_ch2_peaks[ch2ipeak][1] < C4_ch2_peaks[ch2jpeak][1]:
                                    C4_ch2_PUpeaktag[ch2jpeak] = 1
                                else:
                                    C4_ch2_PUpeaktag[ch2ipeak] = 1

                    # Build PU and non PU collection
                    for ch2ipeak in range(len(C4_ch2_peaks)):
                        if C4_ch2_PUpeaktag[ch2ipeak] == 1:
                            C4_ch2_peaks_PU.append((C4_ch2_peaks[ch2ipeak][0], C4_ch2_peaks[ch2ipeak][1], C4_ch2_peaks[ch2ipeak][2]))
                        else:
                            C4_ch2_peaks_NoPU.append((C4_ch2_peaks[ch2ipeak][0], C4_ch2_peaks[ch2ipeak][1], C4_ch2_peaks[ch2ipeak][2]))

                    # Here run the normal selection using the newly built PU collection
                    for ch1 in range(len(C4_ch1_peaks_PU)):
                        for ch2 in range(len(C4_ch2_peaks_PU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C4_ch1_peaks_PU[ch1][1] - C4_ch2_peaks_PU[ch2][1]) < t_coincidence: # actual selection!!!
                                C4_trigger_totPE_PU = C4_ch1_peaks_PU[ch1][0]/C4_ch1_adcgain + C4_ch2_peaks_PU[ch2][0]/C4_ch2_adcgain
                                C4_trigger_time_PU = (C4_ch1_peaks_PU[ch1][1] + C4_ch2_peaks_PU[ch2][1])/2
                                if C4_trigger_totPE_PU > n_cap_pd_minPE and C4_ch1_peaks_PU[ch1][0] < pd_adc_saturation_cut and C4_ch2_peaks_PU[ch2][0] < pd_adc_saturation_cut:
                                    C4_per_evt_large_signal_PU.append((C4_trigger_totPE_PU, C4_trigger_time_PU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C4_per_evt_large_signal_PU)):

                        # output branch to root file
                        C4_matched_PDPeak_PE_PU[0] = C4_per_evt_large_signal_PU[ipdpeak][0]/pescaleC4
                        # Filled matched PD peak pe
                        myC4PeaksPU.Fill()

                    # Here cp for noPU collection
                    for ch1 in range(len(C4_ch1_peaks_NoPU)):
                        for ch2 in range(len(C4_ch2_peaks_NoPU)):

                            """ Only add signal close in time   """
                            """ Use the avg. time of the 2 channels """
                            if abs(C4_ch1_peaks_NoPU[ch1][1] - C4_ch2_peaks_NoPU[ch2][1]) < t_coincidence: # actual selection!!!
                                C4_trigger_totPE_NoPU = C4_ch1_peaks_NoPU[ch1][0]/C4_ch1_adcgain + C4_ch2_peaks_NoPU[ch2][0]/C4_ch2_adcgain
                                C4_trigger_time_NoPU = (C4_ch1_peaks_NoPU[ch1][1] + C4_ch2_peaks_NoPU[ch2][1])/2
                                if C4_trigger_totPE_NoPU > n_cap_pd_minPE and C4_ch1_peaks_NoPU[ch1][0] < pd_adc_saturation_cut and C4_ch2_peaks_NoPU[ch2][0] < pd_adc_saturation_cut:
                                    C4_per_evt_large_signal_NoPU.append((C4_trigger_totPE_NoPU, C4_trigger_time_NoPU))

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C4_per_evt_large_signal_NoPU)):

                        # output branch to root file
                        C4_matched_PDPeak_PE_NoPU[0] = C4_per_evt_large_signal_NoPU[ipdpeak][0]/pescaleC4
                        # Filled matched PD peak pe
                        myC4PeaksNoPU.Fill()

                """ Clear arrays per trigger """
                C1_ch1_peaks, C1_ch2_peaks = [], []
                C2_ch1_peaks, C2_ch2_peaks = [], []
                C3_ch1_peaks, C3_ch2_peaks = [], []
                C4_ch1_peaks, C4_ch2_peaks = [], []
                C1_ch1_peaks_NoPU, C1_ch1_peaks_PU, C1_ch2_peaks_NoPU, C1_ch2_peaks_PU = [], [], [], []
                C2_ch1_peaks_NoPU, C2_ch1_peaks_PU, C2_ch2_peaks_NoPU, C2_ch2_peaks_PU = [], [], [], []
                C3_ch1_peaks_NoPU, C3_ch1_peaks_PU, C3_ch2_peaks_NoPU, C3_ch2_peaks_PU = [], [], [], []
                C4_ch1_peaks_NoPU, C4_ch1_peaks_PU, C4_ch2_peaks_NoPU, C4_ch2_peaks_PU = [], [], [], []
                C1_per_evt_large_signal_PU, C2_per_evt_large_signal_PU, C3_per_evt_large_signal_PU, C4_per_evt_large_signal_PU = [], [], [], []
                C1_per_evt_large_signal_NoPU, C2_per_evt_large_signal_NoPU, C3_per_evt_large_signal_NoPU, C4_per_evt_large_signal_NoPU = [], [], [], []

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
myC1PeaksPU.Write()
myC1PeaksNoPU.Write()
myC2PeaksPU.Write()
myC2PeaksNoPU.Write()
myC3PeaksPU.Write()
myC3PeaksNoPU.Write()
myC4PeaksPU.Write()
myC4PeaksNoPU.Write()


""" PD plots """
# tpc matched
print('Tot triggers:', my_nTriggers[0])
print('Tot evts:', n_evt)
