# VD PDS Analyses

## Generate and sim Ar39 in protodunne-VD under dunesw v10_07_00d00
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

cd /exp/dune/app/users/weishi/pDUNEVDsim

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_07_00d00 -q e26:prof

# comment out everything but ar39
lar -c gen_protodunevd_Ar39.fcl  -n 10

# g4 stage 1
lar -c protodunevd_refactored_g4_stage1.fcl -n -1 gen_protodunevd_radioAr39.root
# pure LAr g4 stage 2
lar -c protodunevd_refactored_g4_stage2_pureAr.fcl -n -1 gen_protodunevd_radioAr39_g4_stage1.root

# detsim pure LAr
lar -c protodunevd_refactored_detsim_pureAr.fcl -n -1 gen_protodunevd_radioAr39_g4_stage1_g4PLAr_stage2.root

# reco
lar -c protodunevd_reco.fcl -n 100 gen_protodunevd_radioAr39_g4_stage1_g4PLAr_stage2_detsim.root

```

Get raw op det waveforms from detsim:
```
.L PlotOpDetWfSimple.C
PlotOpDetWf()
```

## Analyze protodunne-VD data under dunesw v10_07_00d00

Setup rucio and justin to locate file:
```
# Setup rucio to locate file
source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
spack load r-m-dd-config experiment=dune
spack load kx509
spack load justin@01.03.00
kx509
export RUCIO_ACCOUNT="${username}"
voms-proxy-init -noregen -rfc -voms dune:/dune/Role=Analysis --hours 168

justin show-files --mql "files where core.runs=36467 and core.run_type=vd-protodune and core.data_tier=full-reconstructed"

rucio replica list file vd-protodune-det-reco:np02vd_raw_run036467_0000_df-s04-d0_dw_0_20250514T130202_reco_stage1_20250514T140238_keepup.root

root [2] Events->GetListOfBranches()->ls()
```

To run to run the decoder on hdf5 files:
```
# under v10_06_00d01 or later
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

cd /exp/dune/app/users/weishi/pDUNEVDdata

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_07_00d00 -q e26:prof

LD_PRELOAD=$XROOTD_LIB/libXrdPosixPreload.so lar -c standard_reco_stage1_protodunevd_keepup.fcl <your_file.hdf5>
```

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
root -l -b -q LeakAnaScope.C

# Check raw waveform
root -l -b -q CheckRawWfm.C
# Analyze SPE from scope data
root -l -b -q SNRAnaScope.C

# Persistence mode: Script to fill all waveform points from a sample in a 2d histogram
root -l -b -q initial_reading.C

# Script to fill all samples in a 2d histogram
root -l -b -q read_all_files.C
```
