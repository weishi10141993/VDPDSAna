# VD PDS Analysis

## Cold Box Data

Official LED/cosmic/random trigger data from RunB++:
```
/eos/experiment/neutplatform/protodune/experiments/ColdBoxVD/September2022run
```

They are also copied to my personal work area:
```
/afs/cern.ch/work/s/shiw/public/ColdBoxVD/September2022run
```

Run B++ PoF light leakage data:
```
/afs/cern.ch/work/s/shiw/public/ColdBoxVD/Coldbox_Sep2022_leakage_check
```

## Analysis Scripts

```
ssh -X shiw@lxplus.cern.ch
cd /afs/cern.ch/work/s/shiw/public/
git clone https://github.com/weishi10141993/VDPDSAna.git

# Script to find peaks/count up-crossings
# Adjust user config file names
root -l -b -q PoFLeakAnaCAEN.C
root -l -b -q PoFLeakAnaScope.C

# Persistence mode: Script to fill all waveform points from a sample in a 2d histogram
root -l -b -q initial_reading.C

# Script to fill all samples in a 2d histogram
root -l -b -q read_all_files.C
```
