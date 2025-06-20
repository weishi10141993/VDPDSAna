# VD PDS Analyses

## Latest VD samples
```
# Latest VD LE samples
search "Marley" with FD-VD filter: https://dune-tech.rice.edu/dunecatalog/

Example reco1 file (only ophit, other hit level obj):
/pnfs/dune/persistent/staging/fardet-vd/f9/c1/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250130T063235Z_gen_001305_supernova_g4stage1_g4stage2_detsim_reco.root

Example reco2 file (with flash, other higher level obj, shower...):
Production PENDING:

# Setup rucio to locate file
source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
spack load r-m-dd-config experiment=dune
spack load kx509
kx509
export RUCIO_ACCOUNT="${username}"
voms-proxy-init -noregen -rfc -voms dune:/dune/Role=Analysis --hours 168

rucio -a $USER list-file-replicas fardet-vd:prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250130T063235Z_gen_001305_supernova_g4stage1_g4stage2_detsim_reco.root --pfns
```

## Analyze VD low E sample under v09_91_04d01

[First time only]
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

mkdir LQMatch
cd LQMatch

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_91_04d01 -q e26:prof
mrb newDev
source /exp/dune/app/users/weishi/LQMatch/localProducts_larsoft_v09_91_04_01_e26_prof/setup
cd srcs

git clone https://github.com/weishi10141993/QLMatching.git

# Build the code:
mrb uc # if you have src code need to add to CMake
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b
```

analyze with default analyzer:
```
lar -c  QLMatchAna.fcl -n 10 /pnfs/dune/persistent/staging/fardet-vd/f9/c1/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250130T063235Z_gen_001305_supernova_g4stage1_g4stage2_detsim_reco.root
```

check data products in the file:
```
lar -c eventdump.fcl <filename> -n 1
```

[Next time login]
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup dunesw v09_91_04d01 -q e26:prof
source /exp/dune/app/users/weishi/LQMatch/localProducts_larsoft_v09_91_04_01_e26_prof/setup
mrbsetenv
```

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

## Analyze VD low E sample under dunesw v10_07_00d00
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

cd /exp/dune/app/users/weishi/pDUNEVDsim

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_07_00d00 -q e26:prof

lar -c OpFlashFinder.fcl -n 10 /pnfs/dune/persistent/staging/fardet-vd/f9/c1/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250130T063235Z_gen_001305_supernova_g4stage1_g4stage2_detsim_reco.root
```

## v10_05_00d00 (obselete)

[First time only]
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

mkdir VDsampleana
cd VDsampleana

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_05_00d00 -q e26:prof
mrb newDev
source /exp/dune/app/users/weishi/VDsampleana/localProducts_larsoft_v10_05_00_e26_prof/setup
cd srcs

mrb g dunesw

# Build the code:
mrb uc # if you have src code need to add to CMake
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b
```

analyze with default analyzer:
```
lar -c  marley_anatree_dunevd10kt_1x8x14_3view_30deg.fcl -n 10 /exp/dune/data/users/lpaulucc/leprod/abr25/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20241023T193217Z_gen_000086_supernova_g4stage1_g4stage2_detsim_reco_reco2.root
```

check data products in the file:
```
lar -c eventdump.fcl <filename> -n 1
```

[Next time login]
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup dunesw v10_05_00d00 -q e26:prof
source /exp/dune/app/users/weishi/VDsampleana/localProducts_larsoft_v10_05_00_e26_prof/setup
mrbsetenv

lar -c  marley_anatree_dunevd10kt_1x8x14_3view_30deg.fcl -n 10 /exp/dune/data/users/lpaulucc/leprod/abr25/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20241023T193217Z_gen_000086_supernova_g4stage1_g4stage2_detsim_reco_reco2.root
```


## OLD samples (obselete)
```
# signal + radiologicals
/exp/dune/data/users/lpaulucc/leprod/mar25/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250115T110101Z_gen_000364_supernova_g4stage1_g4stage2_detsim_reco_reco2.root

/pnfs/dune/scratch/users/lpaulucc/fdvd_reco2/06435/1/001/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20250125T221812Z_gen_001769_supernova_g4stage1_g4stage2_detsim_reco_reco2_2025-04-26T_123131Z.root

/exp/dune/data/users/lpaulucc/leprod/abr25/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20241023T193217Z_gen_000086_supernova_g4stage1_g4stage2_detsim_reco_reco2_20250425T145346_merged.root

/exp/dune/data/users/lpaulucc/leprod/abr25/prodmarley_nue_cc_flat_radiological_decay0_dunevd10kt_1x8x14_3view_30deg_20241023T193217Z_gen_000086_supernova_g4stage1_g4stage2_detsim_reco_reco2.root

# These two: Laura not sure if they contain radiologicals?
# marley only
/exp/dune/data/users/lpaulucc/leprod/mar25/fardet-vd_mc_le_mc_2024a_workflow0000003785_out1_standard_reco1_dunevd10kt_1x8x14_3view_30deg_hit-reconstructed_merged_skip000450_lim000025_mergeLar_20250204T141351_reco2.root
/exp/dune/data/users/lpaulucc/leprod/mar25/fardet-vd_mc_le_mc_2024a_workflow0000003785_out1_standard_reco1_dunevd10kt_1x8x14_3view_30deg_hit-reconstructed_merged_skip000475_lim000024_mergeLar_20250204T141425_reco2.root

# this sample may not be complete
/pnfs/dune/scratch/users/lpaulucc/fdvd_pointing/05938/1
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
