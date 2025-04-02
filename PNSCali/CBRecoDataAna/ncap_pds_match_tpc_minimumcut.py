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
C1_ch1_adcgain = 15 # 2nd closest to PNS
C1_ch2_adcgain = 10
C2_ch1_adcgain = 15
C2_ch2_adcgain = 10
C3_ch1_adcgain = 15
C3_ch2_adcgain = 15
C4_ch1_adcgain = 10 # closest to PNS
C4_ch2_adcgain = 15

# Matching
# area to look for hits on top XA: cm
ROI_radius = 30
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
f_out = TFile('{0}/CBAna.root'.format(out_path), 'RECREATE')

myTotTriggers = TTree('myTotTriggers', 'myTotTriggers')
myMatchedC1Peaks = TTree('myMatchedC1Peaks', 'myMatchedC1Peaks')
myMatchedC2Peaks = TTree('myMatchedC2Peaks', 'myMatchedC2Peaks')
myMatchedC3Peaks = TTree('myMatchedC3Peaks', 'myMatchedC3Peaks')
myMatchedC4Peaks = TTree('myMatchedC4Peaks', 'myMatchedC4Peaks')

my_nTriggers = array('i', [0])
myTotTriggers.Branch('my_nTriggers', my_nTriggers, 'my_nTriggers/I')
C1_matched_PDPeak_PE = array('f', [0.0])
myMatchedC1Peaks.Branch('C1_matched_PDPeak_PE', C1_matched_PDPeak_PE, 'C1_matched_PDPeak_PE/F')
C2_matched_PDPeak_PE = array('f', [0.0])
myMatchedC2Peaks.Branch('C2_matched_PDPeak_PE', C2_matched_PDPeak_PE, 'C2_matched_PDPeak_PE/F')
C3_matched_PDPeak_PE = array('f', [0.0])
myMatchedC3Peaks.Branch('C3_matched_PDPeak_PE', C3_matched_PDPeak_PE, 'C3_matched_PDPeak_PE/F')
C4_matched_PDPeak_PE = array('f', [0.0])
myMatchedC4Peaks.Branch('C4_matched_PDPeak_PE', C4_matched_PDPeak_PE, 'C4_matched_PDPeak_PE/F')

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
C1_pd_trk3d_dt_all_evts, C2_pd_trk3d_dt_all_evts, C3_pd_trk3d_dt_all_evts, C4_pd_trk3d_dt_all_evts = [], [], [], []
C1_pdmatched_shs_all_evts, C2_pdmatched_shs_all_evts, C3_pdmatched_shs_all_evts, C4_pdmatched_shs_all_evts = [], [], [], []
C1_pdmatched_shs_per_evt, C2_pdmatched_shs_per_evt, C3_pdmatched_shs_per_evt, C4_pdmatched_shs_per_evt = [], [], [], []
shs = []
trk3ds = []
trk3ds_sel = []
trk3d_time = [] # start and stop time of 3d tracks
trk3d_dz_all_evts = []
shmatched = list()
C1_tpcmatched_totPE_all_evts, C2_tpcmatched_totPE_all_evts, C3_tpcmatched_totPE_all_evts, C4_tpcmatched_totPE_all_evts = [], [], [], []
C1_tpcmatched_time_all_evts, C2_tpcmatched_time_all_evts, C3_tpcmatched_time_all_evts, C4_tpcmatched_time_all_evts = [], [], [], []

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

                """ Find and store 3D track start/stop time in this trigger window """
                for itrk3d in range(len(trk3ds)):
                    # Only care trks after beam stops and within 1ms to be consistent to PNS run condition
                    #if trk3ds[itrk3d][6] > beam_stop_time_inPDticks*16/512 and trk3ds[itrk3d][7] <= max_time_tick_tpc:
                    if trk3ds[itrk3d][7] <= max_time_tick_tpc:
                        trk3ds_sel.append((trk3ds[itrk3d][0], trk3ds[itrk3d][1], trk3ds[itrk3d][2], trk3ds[itrk3d][3], trk3ds[itrk3d][4], trk3ds[itrk3d][5], trk3ds[itrk3d][6], trk3ds[itrk3d][7])) # replace trk3ds with trigger time cuts
                        trk3d_time.append((trk3ds[itrk3d][6], trk3ds[itrk3d][7])) # tpc ticks,
                        trk3d_dz_all_evts.append((trk3ds[itrk3d][0] - trk3ds[itrk3d][1])) # dz

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

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window """
                    """ Cut: For PD only look at large peaks """
                    for ipdpeak in range(len(C1_per_evt_large_signal)):
                        n_close3dtrks_C1 = 0
                        C1_matched_PDPeak_PE[0] = 0

                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C1_per_evt_large_signal)-1:

                            # store gap time between large pd signals
                            C1_all_evts_large_signals_gaptime.append(C1_per_evt_large_signal[ipdpeak][1] - C1_per_evt_large_signal[ipdpeak-1][1])

                            """ Cut: only look at data after neutron beam stopped """
                            #if C1_per_evt_large_signal[ipdpeak][1] > beam_stop_time_inPDticks:
                            if 1 > 0:

                                """ Loop 3D trk """
                                for itrk3d in range(len(trk3ds_sel)):
                                    # start time: trk3d_time[itrk3d][0]
                                    # stop time: trk3d_time[itrk3d][1]

                                    """ Control plots """
                                    # Crossing tracks
                                    if trk3ds_sel[itrk3d][0] - trk3ds_sel[itrk3d][1] >= fulldriftd:
                                        C1_pd_trk3d_dt_all_evts.append((trk3d_time[itrk3d][0] - C1_per_evt_large_signal[ipdpeak][1]*16/512, trk3d_time[itrk3d][1] - C1_per_evt_large_signal[ipdpeak][1]*16/512))

                                    # Count crossing tracks near PD signal in time, tpc should see at a short time window as PD
                                    if trk3d_time[itrk3d][0] - C1_per_evt_large_signal[ipdpeak][1]*16/512 >= 0 and trk3d_time[itrk3d][0] - C1_per_evt_large_signal[ipdpeak][1]*16/512 < trk3d_time_gap:
                                        n_close3dtrks_C1 = n_close3dtrks_C1 + 1
                                """ End Loop 3D trk """

                                """ Cut: clean pd peaks """
                                """      No 3D track close in time to PD peak """
                                #if n_close3dtrks_C1 == 0:
                                if 1>0:
                                    """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks """
                                    """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time) """
                                    for ish in range(len(shs)):
                                        if shs[ish][0] > C1_x - ROI_radius and shs[ish][0] < C1_x + ROI_radius and shs[ish][1] > C1_y - ROI_radius and shs[ish][1] < C1_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C1_per_evt_large_signal[ipdpeak][1]*16/512 + driftt204 and shs[ish][2][0] >= C1_per_evt_large_signal[ipdpeak][1]*16/512 + driftt0 and shs[ish][3] > blip_trk_dist and shs[ish][4] < 3 and shmatched[ish] == False:
                                            shmatched[ish] = True
                                            C1_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C1_per_evt_large_signal[ipdpeak][1]*16/512))

                                    """ Check single hits basic quantities: nshs, max_sh_dr, max_sh_dt, mean x, mean y, mean t """
                                    nshs = len(C1_pdmatched_shs_per_evt)
                                    max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t = 0, 0, 0, 0, 0
                                    # plot one example
                                    if nshs >= 2:
                                        xy_shs_pdmatched_C1_per_evt = [(x[0], x[1]) for x in C1_pdmatched_shs_per_evt]
                                        if len(xy_shs_pdmatched_C1_per_evt) != 0 :
                                            proj2d.proj_above_all(xy_shs_pdmatched_C1_per_evt, out=inname+"_pns_tpc_shs_xy_matched_C1", xmin=-200., xmax=200, ymin=-150, ymax=150, the_title='C1', the_x_title='x[cm]', the_y_title='y[cm]', show=False, point=True, coloraxis=True)

                                    for imsh in range(nshs):
                                        avg_x = avg_x + C1_pdmatched_shs_per_evt[imsh][0]
                                        avg_y = avg_y + C1_pdmatched_shs_per_evt[imsh][1]
                                        avg_t = avg_t + C1_pdmatched_shs_per_evt[imsh][2] # this is tpc timing in ticks relative to pd peak time
                                        for jmsh in range(imsh+1, nshs):
                                            temp_dt = abs(np.float128(C1_pdmatched_shs_per_evt[imsh][2] - C1_pdmatched_shs_per_evt[jmsh][2]))
                                            temp_dr = math.sqrt( pow(np.float128(C1_pdmatched_shs_per_evt[imsh][0] - C1_pdmatched_shs_per_evt[jmsh][0]), 2) + pow(np.float128(C1_pdmatched_shs_per_evt[imsh][1] - C1_pdmatched_shs_per_evt[jmsh][1]), 2) + pow(np.float128(C1_pdmatched_shs_per_evt[imsh][2] - C1_pdmatched_shs_per_evt[jmsh][2])*0.512*1.63/10, 2) ) # drfit velocity 1.63mm/us
                                            if temp_dt > max_sh_dt:
                                                max_sh_dt = temp_dt
                                            if temp_dr > max_sh_dr:
                                                max_sh_dr = temp_dr

                                    if nshs != 0:
                                        avg_x = avg_x/nshs
                                        avg_y = avg_y/nshs
                                        avg_t = avg_t/nshs
                                        C1_pdmatched_shs_all_evts.append((nshs, max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t))

                                    """ Match criteria """
                                    #if nshs >= 1 and avg_t >= 200 and avg_t < 275 and max_sh_dt < 50 and avg_x > C1_x - 5 and avg_x < C1_x + 5 and avg_y > C1_y - 5 and avg_y < C1_y + 5 and max_sh_dr < 10:
                                    #if nshs >= 1 and avg_t >= 0 and avg_t < 50:
                                    #if nshs >= 1:
                                    if 1 > 0:
                                        C1_tpcmatched_totPE_all_evts.append((C1_per_evt_large_signal[ipdpeak][0]))
                                        C1_tpcmatched_time_all_evts.append((C1_per_evt_large_signal[ipdpeak][1]))
                                        # output branch to root file
                                        C1_matched_PDPeak_PE[0] = C1_per_evt_large_signal[ipdpeak][0]
                                        """ matched sh charge info? charge_pos on collection plane..."""

                        # Filled matched PD peak pe
                        myMatchedC1Peaks.Fill()
                    """ End C1 matching """

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

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C2_per_evt_large_signal)):
                        n_close3dtrks_C2 = 0
                        C2_matched_PDPeak_PE[0] = 0

                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C2_per_evt_large_signal)-1:

                            """ For PD: only look at large peaks """
                            # store gap time between large pd signals
                            C2_all_evts_large_signals_gaptime.append(C2_per_evt_large_signal[ipdpeak][1] - C2_per_evt_large_signal[ipdpeak-1][1])

                            """ and after neutron beam is not active """
                            #if C2_per_evt_large_signal[ipdpeak][1] > beam_stop_time_inPDticks:
                            if 1 > 0:

                                """ Loop 3D tracks """
                                for itrk3d in range(len(trk3ds_sel)):
                                    # start time: trk3d_time[itrk3d][0]
                                    # stop time: trk3d_time[itrk3d][1]

                                    """ Control plots """
                                    if trk3ds_sel[itrk3d][0] - trk3ds_sel[itrk3d][1] >= fulldriftd:
                                        C2_pd_trk3d_dt_all_evts.append((trk3d_time[itrk3d][0] - C2_per_evt_large_signal[ipdpeak][1]*16/512, trk3d_time[itrk3d][1] - C2_per_evt_large_signal[ipdpeak][1]*16/512))

                                    if trk3d_time[itrk3d][0] - C2_per_evt_large_signal[ipdpeak][1]*16/512 >= 0 and trk3d_time[itrk3d][0] - C2_per_evt_large_signal[ipdpeak][1]*16/512 < trk3d_time_gap:
                                        n_close3dtrks_C2 = n_close3dtrks_C2 + 1
                                """ End Loop 3D trk """

                                """ Cut: clean pd peaks """
                                """      No 3d track close in time to PD peak """
                                #if n_close3dtrks_C2 == 0:
                                if 1>0:

                                    """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                    """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                    for ish in range(len(shs)):
                                        if shs[ish][0] > C2_x - ROI_radius and shs[ish][0] < C2_x + ROI_radius and shs[ish][1] > C2_y - ROI_radius and shs[ish][1] < C2_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C2_per_evt_large_signal[ipdpeak][1]*16/512 + driftt204 and shs[ish][2][0] >= C2_per_evt_large_signal[ipdpeak][1]*16/512 + driftt0 and shs[ish][3] > blip_trk_dist and shs[ish][4] < 3 and shmatched[ish] == False:
                                            shmatched[ish] = True
                                            C2_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C2_per_evt_large_signal[ipdpeak][1]*16/512))

                                    """ Check single hits basic quantities: nshs, max_sh_dr, max_sh_dt, mean x, mean y, mean t """
                                    nshs = len(C2_pdmatched_shs_per_evt)
                                    max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t = 0, 0, 0, 0, 0
                                    # plot one example
                                    if nshs >= 2:
                                        xy_shs_pdmatched_C2_per_evt = [(x[0], x[1]) for x in C2_pdmatched_shs_per_evt]
                                        if len(xy_shs_pdmatched_C2_per_evt) != 0 :
                                            proj2d.proj_above_all(xy_shs_pdmatched_C2_per_evt, out=inname+"_pns_tpc_shs_xy_matched_C2", xmin=-200., xmax=200, ymin=-150, ymax=150, the_title='C2', the_x_title='x[cm]', the_y_title='y[cm]', show=False, point=True, coloraxis=True)

                                    for imsh in range(nshs):
                                        avg_x = avg_x + C2_pdmatched_shs_per_evt[imsh][0]
                                        avg_y = avg_y + C2_pdmatched_shs_per_evt[imsh][1]
                                        avg_t = avg_t + C2_pdmatched_shs_per_evt[imsh][2]
                                        for jmsh in range(imsh+1, nshs):
                                            temp_dt = abs(np.float128(C2_pdmatched_shs_per_evt[imsh][2] - C2_pdmatched_shs_per_evt[jmsh][2]))
                                            temp_dr = math.sqrt( pow(np.float128(C2_pdmatched_shs_per_evt[imsh][0] - C2_pdmatched_shs_per_evt[jmsh][0]), 2) + pow(np.float128(C2_pdmatched_shs_per_evt[imsh][1] - C2_pdmatched_shs_per_evt[jmsh][1]), 2) + pow(np.float128(C2_pdmatched_shs_per_evt[imsh][2] - C2_pdmatched_shs_per_evt[jmsh][2])*0.512*1.63/10, 2) )
                                            if temp_dt > max_sh_dt:
                                                max_sh_dt = temp_dt
                                            if temp_dr > max_sh_dr:
                                                max_sh_dr = temp_dr

                                    if nshs != 0:
                                        avg_x = avg_x/nshs
                                        avg_y = avg_y/nshs
                                        avg_t = avg_t/nshs
                                        C2_pdmatched_shs_all_evts.append((nshs, max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t))

                                    """ Match criteria """
                                    #if nshs >= 1 and avg_t >= 200 and avg_t < 275 and max_sh_dt < 50 and avg_x > C2_x - 5 and avg_x < C2_x + 5 and avg_y > C2_y - 5 and avg_y < C2_y + 5 and max_sh_dr < 10:
                                    #if nshs >= 1 and avg_t >= 0 and avg_t < 50:
                                    #if nshs >= 1:
                                    if 1 > 0:
                                        C2_tpcmatched_totPE_all_evts.append((C2_per_evt_large_signal[ipdpeak][0]))
                                        C2_tpcmatched_time_all_evts.append((C2_per_evt_large_signal[ipdpeak][1]))
                                        # output branch to root file
                                        C2_matched_PDPeak_PE[0] = C2_per_evt_large_signal[ipdpeak][0]

                        # Filled matched PD peak pe
                        myMatchedC2Peaks.Fill()
                    """ End C2 matching """

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

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C3_per_evt_large_signal)):

                        n_close3dtrks_C3 = 0
                        C3_matched_PDPeak_PE[0] = 0

                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C3_per_evt_large_signal)-1:

                            """ For PD: only look at large peaks """
                            # store gap time between large pd signals
                            C3_all_evts_large_signals_gaptime.append(C3_per_evt_large_signal[ipdpeak][1] - C3_per_evt_large_signal[ipdpeak-1][1])

                            """ and after neutron beam is not active """
                            #if C3_per_evt_large_signal[ipdpeak][1] > beam_stop_time_inPDticks:
                            if 1 > 0:

                                """ Loop 3d tracks """
                                for itrk3d in range(len(trk3ds_sel)):
                                    # start time: trk3d_time[itrk3d][0]
                                    # stop time: trk3d_time[itrk3d][1]

                                    """ Control plots """
                                    if trk3ds_sel[itrk3d][0] - trk3ds_sel[itrk3d][1] >= fulldriftd:
                                        C3_pd_trk3d_dt_all_evts.append((trk3d_time[itrk3d][0] - C3_per_evt_large_signal[ipdpeak][1]*16/512, trk3d_time[itrk3d][1] - C3_per_evt_large_signal[ipdpeak][1]*16/512))

                                    if trk3d_time[itrk3d][0] - C3_per_evt_large_signal[ipdpeak][1]*16/512 >= 0 and trk3d_time[itrk3d][0] - C3_per_evt_large_signal[ipdpeak][1]*16/512 < trk3d_time_gap:
                                        n_close3dtrks_C3 = n_close3dtrks_C3 + 1
                                """ End Loop 3D trk """

                                """ Cut: clean pd peaks """
                                """      No 3d track close in time to PD peak """
                                #if n_close3dtrks_C3 == 0:
                                if 1>0:

                                    """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                    """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                    for ish in range(len(shs)):
                                        if shs[ish][0] > C3_x - ROI_radius and shs[ish][0] < C3_x + ROI_radius and shs[ish][1] > C3_y - ROI_radius and shs[ish][1] < C3_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C3_per_evt_large_signal[ipdpeak][1]*16/512 + driftt204 and shs[ish][2][0] >= C3_per_evt_large_signal[ipdpeak][1]*16/512 + driftt0 and shs[ish][3] > blip_trk_dist and shs[ish][4] < 3 and shmatched[ish] == False:
                                            shmatched[ish] = True
                                            C3_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C3_per_evt_large_signal[ipdpeak][1]*16/512))

                                    """ Check single hits basic quantities: nshs, max_sh_dr, max_sh_dt, mean x, mean y, mean t """
                                    nshs = len(C3_pdmatched_shs_per_evt)
                                    max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t = 0, 0, 0, 0, 0
                                    # plot one example
                                    if nshs >= 2:
                                        xy_shs_pdmatched_C3_per_evt = [(x[0], x[1]) for x in C3_pdmatched_shs_per_evt]
                                        if len(xy_shs_pdmatched_C3_per_evt) != 0 :
                                            proj2d.proj_above_all(xy_shs_pdmatched_C3_per_evt, out=inname+"_pns_tpc_shs_xy_matched_C3", xmin=-200., xmax=200, ymin=-150, ymax=150, the_title='C3', the_x_title='x[cm]', the_y_title='y[cm]', show=False, point=True, coloraxis=True)

                                    for imsh in range(nshs):
                                        avg_x = avg_x + C3_pdmatched_shs_per_evt[imsh][0]
                                        avg_y = avg_y + C3_pdmatched_shs_per_evt[imsh][1]
                                        avg_t = avg_t + C3_pdmatched_shs_per_evt[imsh][2]
                                        for jmsh in range(imsh+1, nshs):
                                            temp_dt = abs(np.float128(C3_pdmatched_shs_per_evt[imsh][2] - C3_pdmatched_shs_per_evt[jmsh][2]))
                                            temp_dr = math.sqrt( pow(np.float128(C3_pdmatched_shs_per_evt[imsh][0] - C3_pdmatched_shs_per_evt[jmsh][0]), 2) + pow(np.float128(C3_pdmatched_shs_per_evt[imsh][1] - C3_pdmatched_shs_per_evt[jmsh][1]), 2) + pow(np.float128(C3_pdmatched_shs_per_evt[imsh][2] - C3_pdmatched_shs_per_evt[jmsh][2])*0.512*1.63/10, 2) )
                                            if temp_dt > max_sh_dt:
                                                max_sh_dt = temp_dt
                                            if temp_dr > max_sh_dr:
                                                max_sh_dr = temp_dr

                                    if nshs != 0:
                                        avg_x = avg_x/nshs
                                        avg_y = avg_y/nshs
                                        avg_t = avg_t/nshs
                                        C3_pdmatched_shs_all_evts.append((nshs, max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t))

                                    """ Match criteria """
                                    #if nshs >= 1 and avg_t >= 200 and avg_t < 275 and max_sh_dt < 50 and avg_x > C3_x - 5 and avg_x < C3_x + 5 and avg_y > C3_y - 5 and avg_y < C3_y + 5 and max_sh_dr < 10:
                                    #if nshs >= 1 and avg_t >= 0 and avg_t < 50:
                                    #if nshs >= 1:
                                    if 1 > 0:
                                        C3_tpcmatched_totPE_all_evts.append((C3_per_evt_large_signal[ipdpeak][0]))
                                        C3_tpcmatched_time_all_evts.append((C3_per_evt_large_signal[ipdpeak][1]))
                                        # output branch to root file
                                        C3_matched_PDPeak_PE[0] = C3_per_evt_large_signal[ipdpeak][0]

                        # Filled matched PD peak pe
                        myMatchedC3Peaks.Fill()
                    """ End C3 matching """

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

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C4_per_evt_large_signal)):
                        n_close3dtrks_C4 = 0
                        C4_matched_PDPeak_PE[0] = 0

                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C4_per_evt_large_signal)-1:

                            """ For PD: only look at large peaks  """
                            # store gap time between large pd signals
                            C4_all_evts_large_signals_gaptime.append(C4_per_evt_large_signal[ipdpeak][1] - C4_per_evt_large_signal[ipdpeak-1][1])

                            """ and after neutron beam is not active """
                            #if C4_per_evt_large_signal[ipdpeak][1] > beam_stop_time_inPDticks:
                            if 1 > 0:

                                """ Loop 3d tracks """
                                for itrk3d in range(len(trk3ds_sel)):
                                    # start time: trk3d_time[itrk3d][0]
                                    # stop time: trk3d_time[itrk3d][1]

                                    """ Control plots """
                                    if trk3ds_sel[itrk3d][0] - trk3ds_sel[itrk3d][1] >= fulldriftd:
                                        C4_pd_trk3d_dt_all_evts.append((trk3d_time[itrk3d][0] - C4_per_evt_large_signal[ipdpeak][1]*16/512, trk3d_time[itrk3d][1] - C4_per_evt_large_signal[ipdpeak][1]*16/512))

                                    if trk3d_time[itrk3d][0] - C4_per_evt_large_signal[ipdpeak][1]*16/512 >= 0 and trk3d_time[itrk3d][0] - C4_per_evt_large_signal[ipdpeak][1]*16/512 < trk3d_time_gap:
                                        n_close3dtrks_C4 = n_close3dtrks_C4 + 1
                                """ End Loop 3D trk """

                                """ Cut: clean pd peaks """
                                """      No 3d track close in time to PD peak  """
                                #if n_close3dtrks_C4 == 0:
                                if 1>0:
                                    """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                    """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                    for ish in range(len(shs)):
                                        if shs[ish][0] > C4_x - ROI_radius and shs[ish][0] < C4_x + ROI_radius and shs[ish][1] > C4_y - ROI_radius and shs[ish][1] < C4_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C4_per_evt_large_signal[ipdpeak][1]*16/512 + driftt204 and shs[ish][2][0] >= C4_per_evt_large_signal[ipdpeak][1]*16/512 + driftt0 and shs[ish][3] > blip_trk_dist and shs[ish][4] < 3 and shmatched[ish] == False:
                                            shmatched[ish] = True
                                            C4_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C4_per_evt_large_signal[ipdpeak][1]*16/512))

                                    """ Check single hits basic quantities: nshs, max_sh_dr, max_sh_dt, mean x, mean y, mean t """
                                    nshs = len(C4_pdmatched_shs_per_evt)
                                    max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t = 0, 0, 0, 0, 0
                                    # plot one example
                                    if nshs >= 2:
                                        xy_shs_pdmatched_C4_per_evt = [(x[0], x[1]) for x in C4_pdmatched_shs_per_evt]
                                        if len(xy_shs_pdmatched_C4_per_evt) != 0 :
                                            proj2d.proj_above_all(xy_shs_pdmatched_C4_per_evt, out=inname+"_pns_tpc_shs_xy_matched_C4", xmin=-200., xmax=200, ymin=-150, ymax=150, the_title='C4', the_x_title='x[cm]', the_y_title='y[cm]', show=False, point=True, coloraxis=True)

                                    for imsh in range(nshs):
                                        avg_x = avg_x + C4_pdmatched_shs_per_evt[imsh][0]
                                        avg_y = avg_y + C4_pdmatched_shs_per_evt[imsh][1]
                                        avg_t = avg_t + C4_pdmatched_shs_per_evt[imsh][2]
                                        for jmsh in range(imsh+1, nshs):
                                            temp_dt = abs(np.float128(C4_pdmatched_shs_per_evt[imsh][2] - C4_pdmatched_shs_per_evt[jmsh][2]))
                                            temp_dr = math.sqrt( pow(np.float128(C4_pdmatched_shs_per_evt[imsh][0] - C4_pdmatched_shs_per_evt[jmsh][0]), 2) + pow(np.float128(C4_pdmatched_shs_per_evt[imsh][1] - C4_pdmatched_shs_per_evt[jmsh][1]), 2) + pow(np.float128(C4_pdmatched_shs_per_evt[imsh][2] - C4_pdmatched_shs_per_evt[jmsh][2])*0.512*1.63/10, 2) )
                                            if temp_dt > max_sh_dt:
                                                max_sh_dt = temp_dt
                                            if temp_dr > max_sh_dr:
                                                max_sh_dr = temp_dr

                                    if nshs != 0:
                                        avg_x = avg_x/nshs
                                        avg_y = avg_y/nshs
                                        avg_t = avg_t/nshs
                                        C4_pdmatched_shs_all_evts.append((nshs, max_sh_dr, max_sh_dt, avg_x, avg_y, avg_t))

                                    """ Match criteria """
                                    #if nshs >= 1 and avg_t >= 200 and avg_t < 275 and max_sh_dt < 50 and avg_x > C4_x - 5 and avg_x < C4_x + 5 and avg_y > C4_y - 5 and avg_y < C4_y + 5 and max_sh_dr < 10:
                                    #if nshs >= 1 and avg_t >= 0 and avg_t < 50:
                                    #if nshs >= 1:
                                    if 1 > 0:
                                        C4_tpcmatched_totPE_all_evts.append((C4_per_evt_large_signal[ipdpeak][0]))
                                        C4_tpcmatched_time_all_evts.append((C4_per_evt_large_signal[ipdpeak][1]))
                                        # output branch to root file
                                        C4_matched_PDPeak_PE[0] = C4_per_evt_large_signal[ipdpeak][0]

                        # Filled matched PD peak pe
                        myMatchedC4Peaks.Fill()

                    """ End C4 matching """

                """ Set to new trigger """
                current_trigger = row_pdpeak['trigger']

                """ Clear arrays per trigger """
                C1_ch1_peaks, C1_ch2_peaks = [], []
                C2_ch1_peaks, C2_ch2_peaks = [], []
                C3_ch1_peaks, C3_ch2_peaks = [], []
                C4_ch1_peaks, C4_ch2_peaks = [], []
                C1_per_evt_large_signal, C2_per_evt_large_signal, C3_per_evt_large_signal, C4_per_evt_large_signal = [], [], [], []
                shs, trk3ds, trk3ds_sel, trk3d_time  = [], [], [], []
                shmatched = list()
                C1_pdmatched_shs_per_evt, C2_pdmatched_shs_per_evt, C3_pdmatched_shs_per_evt, C4_pdmatched_shs_per_evt = [], [], [], []

                """ Collect all single hits in current trigger within 1ms, some cosmic runs has 4ms long wfm, don't need that for comparison """
                """ Only need to do one time whenever we switch to new trigger """
                for row_sh in d_sh:
                    if row_sh['trigger'] == current_trigger and row_sh['tdc_max'][2] <= max_time_tick_tpc:
                        shs.append((row_sh['x'], row_sh['y'], row_sh['tdc_max'], row_sh['d_track_2D'], row_sh['d_bary_max']))
                        shmatched.append(False) # False indicate the hit is not matched to pd signal yet

                """ Collect all 3d tracks in current trigger within 1ms, some cosmic runs has 4ms long wfm, don't need that for comparison """
                """ Only need to do one time whenever we switch to new trigger """
                for row_trk3d in d_trk3d:
                    if row_trk3d['trigger'] == current_trigger:
                        trk3ds.append((row_trk3d['z_ini'], row_trk3d['z_end'], row_trk3d['y_ini'], row_trk3d['y_end'], row_trk3d['x_ini'], row_trk3d['x_end'], row_trk3d['t_ini'], row_trk3d['t_end']))

            # Back to main pd peak for loop
            """ Always append a new pd row """
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

# gap time b/t large pd peak and 3d trks start time
C1_largepdpeaks_3dtrks_start_timegap = [(x[0]) for x in C1_pd_trk3d_dt_all_evts]
C2_largepdpeaks_3dtrks_start_timegap = [(x[0]) for x in C2_pd_trk3d_dt_all_evts]
C3_largepdpeaks_3dtrks_start_timegap = [(x[0]) for x in C3_pd_trk3d_dt_all_evts]
C4_largepdpeaks_3dtrks_start_timegap = [(x[0]) for x in C4_pd_trk3d_dt_all_evts]

# gap time b/t large pd peak and 3d trks stop time
C1_largepdpeaks_3dtrks_stop_timegap = [(x[1]) for x in C1_pd_trk3d_dt_all_evts]
C2_largepdpeaks_3dtrks_stop_timegap = [(x[1]) for x in C2_pd_trk3d_dt_all_evts]
C3_largepdpeaks_3dtrks_stop_timegap = [(x[1]) for x in C3_pd_trk3d_dt_all_evts]
C4_largepdpeaks_3dtrks_stop_timegap = [(x[1]) for x in C4_pd_trk3d_dt_all_evts]

# 3d trk dz in drift
dz_3dtrks = [(x) for x in trk3d_dz_all_evts]

# tpc matched
print('Tot triggers:', my_nTriggers[0])
print('Tot evts:', n_evt)
print('C1 tpc-matched pdpeaks:', len(C1_tpcmatched_totPE_all_evts))
print('C2 tpc-matched pdpeaks:', len(C2_tpcmatched_totPE_all_evts))
print('C3 tpc-matched pdpeaks:', len(C3_tpcmatched_totPE_all_evts))
print('C4 tpc-matched pdpeaks:', len(C4_tpcmatched_totPE_all_evts))
PE_pdpeaks_C1_tpc_matched = [(x) for x in C1_tpcmatched_totPE_all_evts]
PE_pdpeaks_C2_tpc_matched = [(x) for x in C2_tpcmatched_totPE_all_evts]
PE_pdpeaks_C3_tpc_matched = [(x) for x in C3_tpcmatched_totPE_all_evts]
PE_pdpeaks_C4_tpc_matched = [(x) for x in C4_tpcmatched_totPE_all_evts]

time_pdpeaks_C1_tpc_matched = [(x) for x in C1_tpcmatched_time_all_evts]
time_pdpeaks_C2_tpc_matched = [(x) for x in C2_tpcmatched_time_all_evts]
time_pdpeaks_C3_tpc_matched = [(x) for x in C3_tpcmatched_time_all_evts]
time_pdpeaks_C4_tpc_matched = [(x) for x in C4_tpcmatched_time_all_evts]

nshs_C1_matched = [(x[0]) for x in C1_pdmatched_shs_all_evts]
nshs_C2_matched = [(x[0]) for x in C2_pdmatched_shs_all_evts]
nshs_C3_matched = [(x[0]) for x in C3_pdmatched_shs_all_evts]
nshs_C4_matched = [(x[0]) for x in C4_pdmatched_shs_all_evts]

max_sh_dr_C1_matched = [(x[1]) for x in C1_pdmatched_shs_all_evts]
max_sh_dr_C2_matched = [(x[1]) for x in C2_pdmatched_shs_all_evts]
max_sh_dr_C3_matched = [(x[1]) for x in C3_pdmatched_shs_all_evts]
max_sh_dr_C4_matched = [(x[1]) for x in C4_pdmatched_shs_all_evts]

max_sh_dt_C1_matched = [(x[2]) for x in C1_pdmatched_shs_all_evts]
max_sh_dt_C2_matched = [(x[2]) for x in C2_pdmatched_shs_all_evts]
max_sh_dt_C3_matched = [(x[2]) for x in C3_pdmatched_shs_all_evts]
max_sh_dt_C4_matched = [(x[2]) for x in C4_pdmatched_shs_all_evts]

avg_x_C1_matched = [(x[3]) for x in C1_pdmatched_shs_all_evts]
avg_x_C2_matched = [(x[3]) for x in C2_pdmatched_shs_all_evts]
avg_x_C3_matched = [(x[3]) for x in C3_pdmatched_shs_all_evts]
avg_x_C4_matched = [(x[3]) for x in C4_pdmatched_shs_all_evts]

avg_y_C1_matched = [(x[4]) for x in C1_pdmatched_shs_all_evts]
avg_y_C2_matched = [(x[4]) for x in C2_pdmatched_shs_all_evts]
avg_y_C3_matched = [(x[4]) for x in C3_pdmatched_shs_all_evts]
avg_y_C4_matched = [(x[4]) for x in C4_pdmatched_shs_all_evts]

avg_t_C1_matched = [(x[5]) for x in C1_pdmatched_shs_all_evts]
avg_t_C2_matched = [(x[5]) for x in C2_pdmatched_shs_all_evts]
avg_t_C3_matched = [(x[5]) for x in C3_pdmatched_shs_all_evts]
avg_t_C4_matched = [(x[5]) for x in C4_pdmatched_shs_all_evts]

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

plt.hist(C1_largepdpeaks_3dtrks_start_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_start - C1 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C1_crossing3dtrkstart_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C2_largepdpeaks_3dtrks_start_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_start - C2 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C2_crossing3dtrkstart_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C3_largepdpeaks_3dtrks_start_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_start - C3 large pd peak  [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C3_crossing3dtrkstart_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C4_largepdpeaks_3dtrks_start_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_start - C4 large pd peak  [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C4_crossing3dtrkstart_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C1_largepdpeaks_3dtrks_stop_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_stop - C1 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C1_crossing3dtrkstop_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C2_largepdpeaks_3dtrks_stop_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_stop - C2 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C2_crossing3dtrkstop_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C3_largepdpeaks_3dtrks_stop_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_stop - C3 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C3_crossing3dtrkstop_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(C4_largepdpeaks_3dtrks_stop_timegap, range=(-2200,2200), bins=2200)
plt.xlabel('trk3d t_stop - C4 large pd peak [ticks x 512ns]')
plt.draw()
plt.savefig("results/"+inname+"_largepdpeaks_C4_crossing3dtrkstop_dt.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(dz_3dtrks, range=(-25,25), bins=50)
plt.xlabel('trk3d dz [cm]')
plt.draw()
plt.savefig("results/"+inname+"_3dtrk_dz.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C1_tpc_matched, range=(0,max_PE_pd), bins=70)
plt.xlabel('C1 matched peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C1_tot_PE_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C2_tpc_matched, range=(0,max_PE_pd), bins=70)
plt.xlabel('C2 matched peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C2_tot_PE_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C3_tpc_matched, range=(0,max_PE_pd), bins=70)
plt.xlabel('C3 matched peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C3_tot_PE_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(PE_pdpeaks_C4_tpc_matched, range=(0,max_PE_pd), bins=70)
plt.xlabel('C4 matched peak total PE')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C4_tot_PE_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C1_tpc_matched, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C1 matched peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C1_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C2_tpc_matched, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C2 matched peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C2_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C3_tpc_matched, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C3 matched peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C3_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C4_tpc_matched, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('C4 matched peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"_pdpeaks_C4_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C1_matched, range=(0,50), bins=50)
plt.xlabel('C1 matched n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C2_matched, range=(0,50), bins=50)
plt.xlabel('C2 matched n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C3_matched, range=(0,50), bins=50)
plt.xlabel('C3 matched n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C4_matched, range=(0,50), bins=50)
plt.xlabel('C4 matched n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C1_matched, range=(0,300), bins=300)
plt.xlabel('C1 max dr matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C2_matched, range=(0,300), bins=300)
plt.xlabel('C2 max dr matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C3_matched, range=(0,300), bins=300)
plt.xlabel('C3 max dr matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C4_matched, range=(0,300), bins=300)
plt.xlabel('C4 max dr matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C1_matched, range=(0,275), bins=275)
plt.xlabel('C1 max dt matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C2_matched, range=(0,275), bins=275)
plt.xlabel('C2 max dt matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C3_matched, range=(0,275), bins=275)
plt.xlabel('C3 max dt matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C4_matched, range=(0,275), bins=275)
plt.xlabel('C4 max dt matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C1_matched, range=(-200,200), bins=400)
plt.xlabel('C1 mean x matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C2_matched, range=(-200,200), bins=400)
plt.xlabel('C2 mean x matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C3_matched, range=(-200,200), bins=400)
plt.xlabel('C3 mean x matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C4_matched, range=(-200,200), bins=400)
plt.xlabel('C4 mean x matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C1_matched, range=(-150,150), bins=300)
plt.xlabel('C1 mean y matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C2_matched, range=(-150,150), bins=300)
plt.xlabel('C2 mean y matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C3_matched, range=(-150,150), bins=300)
plt.xlabel('C3 mean y matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C4_matched, range=(-150,150), bins=300)
plt.xlabel('C4 mean y matched single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C1_matched, range=(0,300), bins=300)
plt.xlabel('C1 mean time matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C2_matched, range=(0,300), bins=300)
plt.xlabel('C2 mean time matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C3_matched, range=(0,300), bins=300)
plt.xlabel('C3 mean time matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C4_matched, range=(0,300), bins=300)
plt.xlabel('C4 mean time matched single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()
#f_out = TFile('results/pdpeaks_C4_time_tpc_matched.root', 'RECREATE')
#tuple = plt.hist(time_pdpeaks_C4_tpc_matched, range=(0,max_time_tick_pd), bins=625)
#d = ROOT.RDataFrame("myTree", "results/pdpeaks_C4_time_tpc_matched.root");
#myHisto = d.Histo1D(("h", "h", 68, 0, max_time_tick_pd), "tuple")
#myHisto.Write()
#f_out.Close()
