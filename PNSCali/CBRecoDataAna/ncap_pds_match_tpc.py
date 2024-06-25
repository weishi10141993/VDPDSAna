#import ROOT
#from ROOT import TFile, TTree
import tables as tab
import numpy as np
import math
import sys
import lar_param as lar
import projection_2d as proj2d
import histograms as hist
import matplotlib
import matplotlib.pyplot as plt

""" get the general infos """
with tab.open_file(sys.argv[1],"r") as f:
    infos = f.get_node('/',name='infos',classname='Table')
    n_channels = infos.read(0,1,field='n_channels')[0].astype(int)
    n_view = infos.read(0,1,field='n_view')[0].astype(int)
    e_drift = infos.read(0,1,field='e_drift')[0].astype(float)
    view_nchan  = infos.read(0,1,field='view_nchan')[0].astype(int)

v_drift = lar.drift_velocity(e_drift)
print("v_drift = ", v_drift)

run_number = 25078
# PD channel map for runs on April 18-19
if run_number == 25050 or run_number == 25068 or run_number == 25071 or run_number == 25078:
    C1_ch1 = 0 # closest to PNS
    C1_ch2 = 7
    C2_ch1 = 10
    C2_ch2 = 17
    C3_ch1 = 30
    C3_ch2 = 37
    C4_ch1 = 40 # closest to PNS
    C4_ch2 = 47
if run_number == 25036 or run_number == 25034:
    C1_ch1 = 0 # closest to PNS
    C1_ch2 = 7
    C2_ch1 = 10
    C2_ch2 = 17
    C3_ch1 = 20
    C3_ch2 = 27
    C4_ch1 = 40 # closest to PNS
    C4_ch2 = 47
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

# Matching
# area to look for hits on top XA: cm
ROI_radius = 30
# tpc time window (ticks) relative to PD peak time
time_window_min = 0
time_window_max = 275
t_coincidence = 10
max_time_tick_pd = 65625 # for pd system, each tick is 16 ns
max_time_tick_tpc = 2100 # for tpc system, each tick is 512 ns
max_adc_pd = 33000
n_cap_pd_adc_cut = 5000
pd_time_gap = 625 # 10us, typical length of pd wfm
#pd_time_gap = 8750 # 1 tpc drfit window in pd time ticks

if run_number == 25068 or run_number == 25078:
    beam_stop_time = 11500 # in pd time ticks, for cosmic run doesn't matter
if run_number == 25050 or run_number == 25071 or run_number == 25036:
    beam_stop_time = 6563 # 105us in pd time ticks

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

n_evt = 0

# Add two channels per trigger record
# not across multiple triggers
current_trigger = -1
C1_ch1_peaks, C2_ch1_peaks, C3_ch1_peaks, C4_ch1_peaks = [], [], [], []
C1_ch2_peaks, C2_ch2_peaks, C3_ch2_peaks, C4_ch2_peaks = [], [], [], []
C1_all_evts, C2_all_evts, C3_all_evts, C4_all_evts = [], [], [], []
CathodePD_all_evts = []
C1_per_evt, C2_per_evt, C3_per_evt, C4_per_evt = [], [], [], []
C1_pdmatched_shs_all_evts, C2_pdmatched_shs_all_evts, C3_pdmatched_shs_all_evts, C4_pdmatched_shs_all_evts = [], [], [], []
C1_pdmatched_shs_per_evt, C2_pdmatched_shs_per_evt, C3_pdmatched_shs_per_evt, C4_pdmatched_shs_per_evt = [], [], [], []
shs = []
shmatched = list()
C1_tpcmatched_totADC, C2_tpcmatched_totADC, C3_tpcmatched_totADC, C4_tpcmatched_totADC = [], [], [], []
C1_tpcmatched_time, C2_tpcmatched_time, C3_tpcmatched_time, C4_tpcmatched_time = [], [], [], []

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

        """ Find PDS peaks in all events """

        for row_pdpeak in d_pds_peaks:

            if row_pdpeak['trigger'] != current_trigger:
                """ A new trigger record """

                """ Compare XA channels in previous trigger record if exists """
                """ Last trigger will not be processed                       """
                """ If one channel has peaks, the other doesn't, discard     """
                """ If ADC is significantly different (TBD) in 2 ch, discard """

                if len(C1_ch1_peaks) != 0 and len(C1_ch2_peaks) != 0:

                    for ch1 in range(len(C1_ch1_peaks)):
                        """ Initialize for each new ch1 peak """
                        min_dt_C1_chs = 70000 # trigger time window is ~1ms/16ns = 62500 ticks

                        for ch2 in range(len(C1_ch2_peaks)):
                            if abs(C1_ch1_peaks[ch1][1] - C1_ch2_peaks[ch2][1]) < min_dt_C1_chs:
                                min_dt_C1_chs = abs(C1_ch1_peaks[ch1][1] - C1_ch2_peaks[ch2][1])

                                """ Only add ADC close in time   """
                                """ Use the avg. time of the 2 channels """
                                if min_dt_C1_chs < t_coincidence:
                                    C1_trigger_totADC = C1_ch1_peaks[ch1][0] + C1_ch2_peaks[ch2][0]
                                    C1_trigger_time = (C1_ch1_peaks[ch1][1] + C1_ch2_peaks[ch2][1])/2
                                    C1_all_evts.append((C1_trigger_totADC, C1_trigger_time))
                                    C1_per_evt.append((C1_trigger_totADC, C1_trigger_time))
                                    CathodePD_all_evts.append((C1_trigger_totADC, C1_trigger_time))

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C1_per_evt)):
                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C1_per_evt)-1:

                            """ For PD: only look at peaks with totADC >5000ADC """
                            """ and no other >5000ADC peaks +/- pd time gap """
                            """ and after neutron beam is not active """
                            if C1_per_evt[ipdpeak][0] > n_cap_pd_adc_cut and C1_per_evt[ipdpeak][1] > beam_stop_time and  C1_per_evt[ipdpeak+1][1] - C1_per_evt[ipdpeak][1] > pd_time_gap and C1_per_evt[ipdpeak][1] - C1_per_evt[ipdpeak-1][1] > pd_time_gap:

                                """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks """
                                """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time) """
                                for ish in range(len(shs)):
                                    if shs[ish][0] > C1_x - ROI_radius and shs[ish][0] < C1_x + ROI_radius and shs[ish][1] > C1_y - ROI_radius and shs[ish][1] < C1_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C1_per_evt[ipdpeak][1]*16/512 + time_window_max and shs[ish][2][0] >= C1_per_evt[ipdpeak][1]*16/512 + time_window_min and shs[ish][3] > 100 and shs[ish][4] < 3 and shmatched[ish] == False:
                                        shmatched[ish] = True
                                        C1_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C1_per_evt[ipdpeak][1]*16/512))

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
                                        temp_dr = math.sqrt( pow(np.float128(C1_pdmatched_shs_per_evt[imsh][0] - C1_pdmatched_shs_per_evt[jmsh][0]), 2) + pow(np.float128(C1_pdmatched_shs_per_evt[imsh][1] - C1_pdmatched_shs_per_evt[jmsh][1]), 2) + pow(np.float128(C1_pdmatched_shs_per_evt[imsh][2] - C1_pdmatched_shs_per_evt[jmsh][2])*0.512*1.63/10, 2) )
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
                                if nshs >= 1:
                                    C1_tpcmatched_totADC.append((C1_per_evt[ipdpeak][0]))
                                    C1_tpcmatched_time.append((C1_per_evt[ipdpeak][1]))

                    """ End C1 matching """

                if len(C2_ch1_peaks) != 0 and len(C2_ch2_peaks) != 0:

                    for ch1 in range(len(C2_ch1_peaks)):
                        """ Initialize for each new ch1 peak """
                        min_dt_C2_chs = 70000 # trigger time window is ~1ms/16ns = 62500 ticks

                        for ch2 in range(len(C2_ch2_peaks)):
                            if abs(C2_ch1_peaks[ch1][1] - C2_ch2_peaks[ch2][1]) < min_dt_C2_chs:
                                min_dt_C2_chs = abs(C2_ch1_peaks[ch1][1] - C2_ch2_peaks[ch2][1])

                                """ Only add ADC close in time (within 20 ticks, 320ns)   """
                                """ Use the avg. time of the 2 channels                   """
                                if min_dt_C2_chs < t_coincidence:
                                    C2_trigger_totADC = C2_ch1_peaks[ch1][0] + C2_ch2_peaks[ch2][0]
                                    C2_trigger_time = (C2_ch1_peaks[ch1][1] + C2_ch2_peaks[ch2][1])/2
                                    C2_per_evt.append((C2_trigger_totADC, C2_trigger_time))
                                    C2_all_evts.append((C2_trigger_totADC, C2_trigger_time))
                                    CathodePD_all_evts.append((C2_trigger_totADC, C2_trigger_time))

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C2_per_evt)):
                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C2_per_evt)-1:

                            """ For PD: only look at peaks with totADC >5000ADC """
                            """ and no other >5000ADC peaks +/- pd time gap """
                            """ and after neutron beam is not active """
                            if C2_per_evt[ipdpeak][0] > n_cap_pd_adc_cut and C2_per_evt[ipdpeak][1] > beam_stop_time and C2_per_evt[ipdpeak+1][1] - C2_per_evt[ipdpeak][1] > pd_time_gap and C2_per_evt[ipdpeak][1] - C2_per_evt[ipdpeak-1][1] > pd_time_gap:

                                """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                for ish in range(len(shs)):
                                    if shs[ish][0] > C2_x - ROI_radius and shs[ish][0] < C2_x + ROI_radius and shs[ish][1] > C2_y - ROI_radius and shs[ish][1] < C2_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C2_per_evt[ipdpeak][1]*16/512 + time_window_max and shs[ish][2][0] >= C2_per_evt[ipdpeak][1]*16/512 + time_window_min and shs[ish][3] > 100 and shs[ish][4] < 3 and shmatched[ish] == False:
                                        shmatched[ish] = True
                                        C2_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C2_per_evt[ipdpeak][1]*16/512))

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
                                if nshs >= 1:
                                    C2_tpcmatched_totADC.append((C2_per_evt[ipdpeak][0]))
                                    C2_tpcmatched_time.append((C2_per_evt[ipdpeak][1]))

                    """ End C2 matching """

                if len(C3_ch1_peaks) != 0 and len(C3_ch2_peaks) != 0:

                    for ch1 in range(len(C3_ch1_peaks)):
                        """ Initialize for each new ch1 peak """
                        min_dt_C3_chs = 70000 # trigger time window is ~1ms/16ns = 62500 ticks

                        for ch2 in range(len(C3_ch2_peaks)):
                            if abs(C3_ch1_peaks[ch1][1] - C3_ch2_peaks[ch2][1]) < min_dt_C3_chs:
                                min_dt_C3_chs = abs(C3_ch1_peaks[ch1][1] - C3_ch2_peaks[ch2][1])

                                """ Only add ADC close in time (within 20 ticks, 320ns)   """
                                """ Use the avg. time of the 2 channels                   """
                                if min_dt_C3_chs < t_coincidence:
                                    C3_trigger_totADC = C3_ch1_peaks[ch1][0] + C3_ch2_peaks[ch2][0]
                                    C3_trigger_time = (C3_ch1_peaks[ch1][1] + C3_ch2_peaks[ch2][1])/2
                                    C3_per_evt.append((C3_trigger_totADC, C3_trigger_time))
                                    C3_all_evts.append((C3_trigger_totADC, C3_trigger_time))
                                    CathodePD_all_evts.append((C3_trigger_totADC, C3_trigger_time))

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C3_per_evt)):
                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C3_per_evt)-1:

                            """ For PD: only look at peaks with totADC >5000ADC """
                            """ and no other >5000ADC peaks +/- pd time gap """
                            """ and after neutron beam is not active """
                            if C3_per_evt[ipdpeak][0] > n_cap_pd_adc_cut and C3_per_evt[ipdpeak][1] > beam_stop_time and C3_per_evt[ipdpeak+1][1] - C3_per_evt[ipdpeak][1] > pd_time_gap and C3_per_evt[ipdpeak][1] - C3_per_evt[ipdpeak-1][1] > pd_time_gap:

                                """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                for ish in range(len(shs)):
                                    if shs[ish][0] > C3_x - ROI_radius and shs[ish][0] < C3_x + ROI_radius and shs[ish][1] > C3_y - ROI_radius and shs[ish][1] < C3_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C3_per_evt[ipdpeak][1]*16/512 + time_window_max and shs[ish][2][0] >= C3_per_evt[ipdpeak][1]*16/512 + time_window_min and shs[ish][3] > 100 and shs[ish][4] < 3 and shmatched[ish] == False:
                                        shmatched[ish] = True
                                        C3_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C3_per_evt[ipdpeak][1]*16/512))

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
                                if nshs >= 1:
                                    C3_tpcmatched_totADC.append((C3_per_evt[ipdpeak][0]))
                                    C3_tpcmatched_time.append((C3_per_evt[ipdpeak][1]))

                    """ End C3 matching """

                if len(C4_ch1_peaks) != 0 and len(C4_ch2_peaks) != 0:

                    for ch1 in range(len(C4_ch1_peaks)):

                        """ Initialize for each new ch1 peak """
                        min_dt_C4_chs = 70000 # trigger time window is ~1ms/16ns = 62500 ticks

                        for ch2 in range(len(C4_ch2_peaks)):

                            if abs(C4_ch1_peaks[ch1][1] - C4_ch2_peaks[ch2][1]) < min_dt_C4_chs:
                                min_dt_C4_chs = abs(C4_ch1_peaks[ch1][1] - C4_ch2_peaks[ch2][1])

                                """ Only add ADC close in time (within 20 ticks, 320ns)   """
                                """ Use the avg. time of the 2 channels                   """
                                if min_dt_C4_chs < t_coincidence:
                                    C4_trigger_totADC = C4_ch1_peaks[ch1][0] + C4_ch2_peaks[ch2][0]
                                    C4_trigger_time = (C4_ch1_peaks[ch1][1] + C4_ch2_peaks[ch2][1])/2
                                    C4_per_evt.append((C4_trigger_totADC, C4_trigger_time))
                                    C4_all_evts.append((C4_trigger_totADC, C4_trigger_time))
                                    CathodePD_all_evts.append((C4_trigger_totADC, C4_trigger_time))

                    """"""""""""""""""""""""""
                    """ PDS + TPC matching """
                    """"""""""""""""""""""""""

                    """ multiple peaks exist in one trigger window, match for each peak """
                    for ipdpeak in range(len(C4_per_evt)):
                        # ignore first and last pd peak
                        if ipdpeak != 0 and ipdpeak != len(C4_per_evt)-1:

                            """ For PD: only look at peaks with totADC >5000ADC """
                            """ and no other >5000ADC peaks +/- pd time gap """
                            """ and after neutron beam is not active """
                            if C4_per_evt[ipdpeak][0] > n_cap_pd_adc_cut and C4_per_evt[ipdpeak][1] > beam_stop_time and C4_per_evt[ipdpeak+1][1] - C4_per_evt[ipdpeak][1] > pd_time_gap and C4_per_evt[ipdpeak][1] - C4_per_evt[ipdpeak-1][1] > pd_time_gap:

                                """ TPC hit time tick is 512ns, one drift time ~140us, 273 ticks"""
                                """ For PD peaks on the detector, look for single hits (x, y) on top of it, collection hit time within 140us (1 drift time)"""
                                for ish in range(len(shs)):
                                    if shs[ish][0] > C4_x - ROI_radius and shs[ish][0] < C4_x + ROI_radius and shs[ish][1] > C4_y - ROI_radius and shs[ish][1] < C4_y + ROI_radius and shs[ish][2][2] >= shs[ish][2][1] and shs[ish][2][1] >= shs[ish][2][0] and shs[ish][2][2] < C4_per_evt[ipdpeak][1]*16/512 + time_window_max and shs[ish][2][0] >= C4_per_evt[ipdpeak][1]*16/512 + time_window_min and shs[ish][3] > 100 and shs[ish][4] < 3 and shmatched[ish] == False:
                                        shmatched[ish] = True
                                        C4_pdmatched_shs_per_evt.append((shs[ish][0], shs[ish][1], shs[ish][2][2] - C4_per_evt[ipdpeak][1]*16/512))

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
                                if nshs >= 1:
                                    C4_tpcmatched_totADC.append((C4_per_evt[ipdpeak][0]))
                                    C4_tpcmatched_time.append((C4_per_evt[ipdpeak][1]))

                    """ End C4 matching """

                """ Set to new trigger """
                current_trigger = row_pdpeak['trigger']

                """ Clear arrays """
                C1_ch1_peaks, C1_ch2_peaks = [], []
                C2_ch1_peaks, C2_ch2_peaks = [], []
                C3_ch1_peaks, C3_ch2_peaks = [], []
                C4_ch1_peaks, C4_ch2_peaks = [], []
                C1_per_evt, C2_per_evt, C3_per_evt, C4_per_evt = [], [], [], []
                shs = []
                shmatched = list()
                C1_pdmatched_shs_per_evt, C2_pdmatched_shs_per_evt, C3_pdmatched_shs_per_evt, C4_pdmatched_shs_per_evt = [], [], [], []

                """ Collect all single hits in current trigger within 1ms, some cosmic runs has 4ms long wfm, don't need that for comparison """
                """ Only need to do one time whenever we switch to new trigger """
                for row_sh in d_sh:
                    if row_sh['trigger'] == current_trigger and row_sh['tdc_max'][2] <= max_time_tick_tpc:
                        shs.append((row_sh['x'], row_sh['y'], row_sh['tdc_max'], row_sh['d_track_2D'], row_sh['d_bary_max']))
                        shmatched.append(False) # False indicate the hit is not matched to pd signal yet

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


""" PD plots """
# Only plot time
time_pdpeaks_C1_all_evts = [(x[1]) for x in C1_all_evts ]
time_pdpeaks_C2_all_evts = [(x[1]) for x in C2_all_evts ]
time_pdpeaks_C3_all_evts = [(x[1]) for x in C3_all_evts ]
time_pdpeaks_C4_all_evts = [(x[1]) for x in C4_all_evts ]
# Only plot adc
adc_pdpeaks_C1_all_evts = [(x[0]) for x in C1_all_evts ]
adc_pdpeaks_C2_all_evts = [(x[0]) for x in C2_all_evts ]
adc_pdpeaks_C3_all_evts = [(x[0]) for x in C3_all_evts ]
adc_pdpeaks_C4_all_evts = [(x[0]) for x in C4_all_evts ]

# tpc matched
print('C1 tpc-matched pdpeaks:', len(C1_tpcmatched_totADC))
print('C2 tpc-matched pdpeaks:', len(C2_tpcmatched_totADC))
print('C3 tpc-matched pdpeaks:', len(C3_tpcmatched_totADC))
print('C4 tpc-matched pdpeaks:', len(C4_tpcmatched_totADC))
adc_pdpeaks_C1_tpc_matched_all_evts = [(x) for x in C1_tpcmatched_totADC]
adc_pdpeaks_C2_tpc_matched_all_evts = [(x) for x in C2_tpcmatched_totADC]
adc_pdpeaks_C3_tpc_matched_all_evts = [(x) for x in C3_tpcmatched_totADC]
adc_pdpeaks_C4_tpc_matched_all_evts = [(x) for x in C4_tpcmatched_totADC]

time_pdpeaks_C1_tpc_matched_all_evts = [(x) for x in C1_tpcmatched_time]
time_pdpeaks_C2_tpc_matched_all_evts = [(x) for x in C2_tpcmatched_time]
time_pdpeaks_C3_tpc_matched_all_evts = [(x) for x in C3_tpcmatched_time]
time_pdpeaks_C4_tpc_matched_all_evts = [(x) for x in C4_tpcmatched_time]

nshs_C1_matched_all_evts = [(x[0]) for x in C1_pdmatched_shs_all_evts]
nshs_C2_matched_all_evts = [(x[0]) for x in C2_pdmatched_shs_all_evts]
nshs_C3_matched_all_evts = [(x[0]) for x in C3_pdmatched_shs_all_evts]
nshs_C4_matched_all_evts = [(x[0]) for x in C4_pdmatched_shs_all_evts]

max_sh_dr_C1_matched_all_evts = [(x[1]) for x in C1_pdmatched_shs_all_evts]
max_sh_dr_C2_matched_all_evts = [(x[1]) for x in C2_pdmatched_shs_all_evts]
max_sh_dr_C3_matched_all_evts = [(x[1]) for x in C3_pdmatched_shs_all_evts]
max_sh_dr_C4_matched_all_evts = [(x[1]) for x in C4_pdmatched_shs_all_evts]

max_sh_dt_C1_matched_all_evts = [(x[2]) for x in C1_pdmatched_shs_all_evts]
max_sh_dt_C2_matched_all_evts = [(x[2]) for x in C2_pdmatched_shs_all_evts]
max_sh_dt_C3_matched_all_evts = [(x[2]) for x in C3_pdmatched_shs_all_evts]
max_sh_dt_C4_matched_all_evts = [(x[2]) for x in C4_pdmatched_shs_all_evts]

avg_x_C1_matched_all_evts = [(x[3]) for x in C1_pdmatched_shs_all_evts]
avg_x_C2_matched_all_evts = [(x[3]) for x in C2_pdmatched_shs_all_evts]
avg_x_C3_matched_all_evts = [(x[3]) for x in C3_pdmatched_shs_all_evts]
avg_x_C4_matched_all_evts = [(x[3]) for x in C4_pdmatched_shs_all_evts]

avg_y_C1_matched_all_evts = [(x[4]) for x in C1_pdmatched_shs_all_evts]
avg_y_C2_matched_all_evts = [(x[4]) for x in C2_pdmatched_shs_all_evts]
avg_y_C3_matched_all_evts = [(x[4]) for x in C3_pdmatched_shs_all_evts]
avg_y_C4_matched_all_evts = [(x[4]) for x in C4_pdmatched_shs_all_evts]

avg_t_C1_matched_all_evts = [(x[5]) for x in C1_pdmatched_shs_all_evts]
avg_t_C2_matched_all_evts = [(x[5]) for x in C2_pdmatched_shs_all_evts]
avg_t_C3_matched_all_evts = [(x[5]) for x in C3_pdmatched_shs_all_evts]
avg_t_C4_matched_all_evts = [(x[5]) for x in C4_pdmatched_shs_all_evts]

plt.hist(adc_pdpeaks_C4_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C3_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C2_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C1_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C4_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C3_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C2_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C1_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C1_tpc_matched_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_tot_adc_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C2_tpc_matched_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_tot_adc_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C3_tpc_matched_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_tot_adc_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C4_tpc_matched_all_evts, range=(0,max_adc_pd), bins=66)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_tot_adc_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C1_tpc_matched_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C2_tpc_matched_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C3_tpc_matched_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C4_tpc_matched_all_evts, range=(0,max_time_tick_pd), bins=625)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_time_tpc_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C1_matched_all_evts, range=(0,50), bins=50)
plt.xlabel('n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C2_matched_all_evts, range=(0,50), bins=50)
plt.xlabel('n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C3_matched_all_evts, range=(0,50), bins=50)
plt.xlabel('n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(nshs_C4_matched_all_evts, range=(0,50), bins=50)
plt.xlabel('n single hits')
plt.draw()
plt.savefig("results/"+inname+"_nshs_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C1_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('max dr between single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C2_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('max dr between single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C3_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('max dr between single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dr_C4_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('max dr between single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dr_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C1_matched_all_evts, range=(0,275), bins=275)
plt.xlabel('max dt between single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C2_matched_all_evts, range=(0,275), bins=275)
plt.xlabel('max dt between single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C3_matched_all_evts, range=(0,275), bins=275)
plt.xlabel('max dt between single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(max_sh_dt_C4_matched_all_evts, range=(0,275), bins=275)
plt.xlabel('max dt between single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_max_sh_dt_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C1_matched_all_evts, range=(-200,200), bins=400)
plt.xlabel('mean x of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C2_matched_all_evts, range=(-200,200), bins=400)
plt.xlabel('mean x of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C3_matched_all_evts, range=(-200,200), bins=400)
plt.xlabel('mean x of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_x_C4_matched_all_evts, range=(-200,200), bins=400)
plt.xlabel('mean x of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_x_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C1_matched_all_evts, range=(-150,150), bins=300)
plt.xlabel('mean y of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C2_matched_all_evts, range=(-150,150), bins=300)
plt.xlabel('mean y of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C3_matched_all_evts, range=(-150,150), bins=300)
plt.xlabel('mean y of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_y_C4_matched_all_evts, range=(-150,150), bins=300)
plt.xlabel('mean y of single hits [cm]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_y_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C1_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('mean time of single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C1_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C2_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('mean time of single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C2_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C3_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('mean time of single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C3_matched.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(avg_t_C4_matched_all_evts, range=(0,300), bins=300)
plt.xlabel('mean time of single hits [tpc ticks]')
plt.draw()
plt.savefig("results/"+inname+"_avg_sh_t_C4_matched.pdf")
plt.clf() # important to clear figure
plt.close()
#f_out = TFile('results/pdpeaks_C4_time_tpc_matched.root', 'RECREATE')
#tuple = plt.hist(time_pdpeaks_C4_tpc_matched_all_evts, range=(0,max_time_tick_pd), bins=625)
#d = ROOT.RDataFrame("myTree", "results/pdpeaks_C4_time_tpc_matched.root");
#myHisto = d.Histo1D(("h", "h", 68, 0, max_time_tick_pd), "tuple")
#myHisto.Write()
#f_out.Close()
