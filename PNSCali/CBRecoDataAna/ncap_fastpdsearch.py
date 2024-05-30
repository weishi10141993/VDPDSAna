import tables as tab
import numpy as np
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

# PD channel map for runs on April 18-19
C1_ch1 = 0 # closest to PNS
C1_ch2 = 7
C2_ch1 = 10
C2_ch2 = 17
C3_ch1 = 30
C3_ch2 = 37
C4_ch1 = 40 # closest to PNS
C4_ch2 = 47

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

n_sh_evt = []
n_evt, n_sh = 0, 0
t_coincidence = 10
max_trigger_time_plot = 68000
max_trigger_adc_plot = 33000

""" loop on all h5 files """
for files in sys.argv[1:]:
    print(files)
    with tab.open_file(files, "r") as f:
        infos = f.get_node('/',name='infos',classname='Table')
        n_evt += infos.read(0,1,field='n_evt')[0].astype(int)

        """ read the pds peaks table """
        t_pds_peaks = f.root.pds_peaks
        d_pds_peaks = t_pds_peaks.read()

        """ Find PDS peaks in all events """
        # Add two channels per trigger record
        # not across multiple triggers
        current_trigger = -1
        C1_ch1_peaks, C2_ch1_peaks, C3_ch1_peaks, C4_ch1_peaks = [], [], [], []
        C1_ch2_peaks, C2_ch2_peaks, C3_ch2_peaks, C4_ch2_peaks = [], [], [], []
        C1_all_evts, C2_all_evts, C3_all_evts, C4_all_evts = [], [], [], []
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

                                """ Only add ADC close in time (within 20 ticks, 320ns)   """
                                """ Use the avg. time of the 2 channels                   """
                                if min_dt_C1_chs < t_coincidence:
                                    C1_trigger_totADC = C1_ch1_peaks[ch1][0] + C1_ch2_peaks[ch2][0]
                                    C1_trigger_time = (C1_ch1_peaks[ch1][1] + C1_ch2_peaks[ch2][1])/2
                                    C1_all_evts.append((C1_trigger_totADC, C1_trigger_time))

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
                                    C2_all_evts.append((C2_trigger_totADC, C2_trigger_time))

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
                                    C3_all_evts.append((C3_trigger_totADC, C3_trigger_time))

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
                                    C4_all_evts.append((C4_trigger_totADC, C4_trigger_time))

                """ Set to new trigger """
                current_trigger = row_pdpeak['trigger']
                """ Clear arrays """
                C1_ch1_peaks, C1_ch2_peaks = [], []
                C2_ch1_peaks, C2_ch2_peaks = [], []
                C3_ch1_peaks, C3_ch2_peaks = [], []
                C4_ch1_peaks, C4_ch2_peaks = [], []

            """ Always append a new row """
            if row_pdpeak['channel'] == C1_ch1: C1_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C1_ch2: C1_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C2_ch1: C2_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C2_ch2: C2_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C3_ch1: C3_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C3_ch2: C3_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C4_ch1: C4_ch1_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))
            if row_pdpeak['channel'] == C4_ch2: C4_ch2_peaks.append((row_pdpeak['max_adc'], row_pdpeak['max_t'], row_pdpeak['charge']))

""" PD plots """
time_adc_pdpeaks_C1_all_evts = [(x[1], x[0]) for x in C1_all_evts]
time_adc_pdpeaks_C2_all_evts = [(x[1], x[0]) for x in C2_all_evts]
time_adc_pdpeaks_C3_all_evts = [(x[1], x[0]) for x in C3_all_evts]
time_adc_pdpeaks_C4_all_evts = [(x[1], x[0]) for x in C4_all_evts]

# Only plot max time max_trigger_time_plot
time_pdpeaks_C1_all_evts = [(x[1]) for x in C1_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
time_pdpeaks_C2_all_evts = [(x[1]) for x in C2_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
time_pdpeaks_C3_all_evts = [(x[1]) for x in C3_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
time_pdpeaks_C4_all_evts = [(x[1]) for x in C4_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
# Only plot max time max_trigger_time_plot
adc_pdpeaks_C1_all_evts = [(x[0]) for x in C1_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
adc_pdpeaks_C2_all_evts = [(x[0]) for x in C2_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
adc_pdpeaks_C3_all_evts = [(x[0]) for x in C3_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]
adc_pdpeaks_C4_all_evts = [(x[0]) for x in C4_all_evts if x[1] < max_trigger_time_plot and x[0] > 5000 ]

if len(time_adc_pdpeaks_C1_all_evts) != 0 :
    proj2d.proj_above_all(time_adc_pdpeaks_C1_all_evts, out=inname+"_pns_pdpeaks_C1_time_adc", xmin=0., xmax=max_trigger_time_plot, ymin= 0, ymax=max_trigger_adc_plot, the_title='C1', the_x_title='time [x 16ns]', the_y_title='Tot ADC', show=False, point=True, coloraxis=True)
if len(time_adc_pdpeaks_C2_all_evts) != 0 :
    proj2d.proj_above_all(time_adc_pdpeaks_C2_all_evts, out=inname+"_pns_pdpeaks_C2_time_adc", xmin=0., xmax=max_trigger_time_plot, ymin= 0, ymax=max_trigger_adc_plot, the_title='C2', the_x_title='time [x 16ns]', the_y_title='Tot ADC', show=False, point=True, coloraxis=True)
if len(time_adc_pdpeaks_C3_all_evts) != 0 :
    proj2d.proj_above_all(time_adc_pdpeaks_C3_all_evts, out=inname+"_pns_pdpeaks_C3_time_adc", xmin=0., xmax=max_trigger_time_plot, ymin= 0, ymax=max_trigger_adc_plot, the_title='C3', the_x_title='time [x 16ns]', the_y_title='Tot ADC', show=False, point=True, coloraxis=True)
if len(time_adc_pdpeaks_C4_all_evts) != 0 :
    proj2d.proj_above_all(time_adc_pdpeaks_C4_all_evts, out=inname+"_pns_pdpeaks_C4_time_adc", xmin=0., xmax=max_trigger_time_plot, ymin= 0, ymax=max_trigger_adc_plot, the_title='C4', the_x_title='time [x 16ns]', the_y_title='Tot ADC', show=False, point=True, coloraxis=True)

plt.hist(adc_pdpeaks_C4_all_evts, range=(0,max_trigger_adc_plot), bins=33)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C3_all_evts, range=(0,max_trigger_adc_plot), bins=33)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C2_all_evts, range=(0,max_trigger_adc_plot), bins=33)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(adc_pdpeaks_C1_all_evts, range=(0,max_trigger_adc_plot), bins=33)
plt.xlabel('Peak total ADC')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_tot_adc.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C4_all_evts, range=(0,max_trigger_time_plot), bins=68)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C4_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C3_all_evts, range=(0,max_trigger_time_plot), bins=68)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C3_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C2_all_evts, range=(0,max_trigger_time_plot), bins=68)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C2_time.pdf")
plt.clf() # important to clear figure
plt.close()

plt.hist(time_pdpeaks_C1_all_evts, range=(0,max_trigger_time_plot), bins=68)
plt.xlabel('Peak time [ticks x 16ns]')
plt.draw()
plt.savefig("results/"+inname+"pdpeaks_C1_time.pdf")
plt.clf() # important to clear figure
plt.close()
