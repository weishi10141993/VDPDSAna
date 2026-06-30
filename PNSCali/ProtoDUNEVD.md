## LArSoft analyzer PNS run at np02

```
# build on dunegpvm

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"
```
```
cd /exp/dune/app/users/weishi/np02pns_larsoft
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

ups list -aK+ dunesw
setup dunesw v10_20_09d00 -q e26:prof
# new G4 version with improved Nuclear De-excitation (NUDEX) package for neutrons:
#   v10_20_09_01d00 featuring Geant4 11.4 (LArSoft v10_20_09_01)

mkdir dunesw_v102009d00
cd dunesw_v102009d00/

mrb newDev
source /exp/dune/app/users/weishi/np02pns_larsoft/dunesw_v102009d00/localProducts_larsoft_v10_20_09_e26_prof/setup

cd srcs
mrb g dunereco

mrb uc # if you have src code need to add to CMake
cd .. # top level, above srcs
mrb z


mrbsetenv
mrbslp # need this to proper config and fix wirecell error

setup ninja
mrb i --generator ninja
#That sets up the default dunereco.
```

Next to setup Blipreco code and the analyzer module, copy two directories below to your dunereco:

```
/exp/dune/app/users/apaudel/dunesw_setups/v10_20_06d01/srcs/dunereco/dunereco/BlipRecoDUNE
/exp/dune/app/users/apaudel/dunesw_setups/v10_20_06d01/srcs/dunereco/dunereco/TrackRemoval
```

add the directory BlipRecoDUNE and TrackRemoval in the CMakeLists file,
```
/exp/dune/app/users/apaudel/dunesw_setups/v10_20_06d01/srcs/dunereco/dunereco/CMakeLists.txt
```

modify the ```dunereco/dunereco/BlipRecoDUNE/CMakeLists.txt``` to add PhotonBackTrackerService, or copy from github: https://github.com/weishi10141993/VDPDSAna/tree/main/PNSCali/PDVD/CMakeLists.txt
sync the  https://github.com/weishi10141993/VDPDSAna/tree/main/PNSCali/PDVD/BlipAnaMC_module.cc

```
cd $MRB_BUILDDIR
mrbsetenv
mrbslp        # required to configure wirecell properly
ninja install
```

Produce sim samples
```
# event dump
lar -c eventdump.fcl <filename> -n 1
# fcl dump
fhicl-dump run_pdvd_blipana.fcl
```

Relogin:
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_20_09d00 -q e26:prof
source /exp/dune/app/users/weishi/np02pns_larsoft/dunesw_v102009d00/localProducts_larsoft_v10_20_09_e26_prof/setup

mrbsetenv
mrbslp
```

Recompile:
```
cd ${MRB_BUILDDIR}  
mrbsetenv
mrbslp     # required to configure wirecell properly
ninja install
```

Jobs:
```
tar -cvf np02pnsAna.tar --exclude='./build_slf7.x86_64' --exclude='./srcs' .

# in the same directory there is justin_profile.sh
source justin_profile.sh

sh launchscript_localdirectory.sh
```

## Test v5 ggd PNS geo LArsoft
```
# from develop branch
git clone https://github.com/weishi10141993/dunecore.git -b pdvdv5pns_test


# build on dunegpvm
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
# check latest dunesw: ups list -aK+ dunesw | tail -n 10
setup dunesw v10_20_05d01 -q e26:prof

mrb newDev
source /exp/dune/app/users/weishi/testpns_np02G4/localProducts_larsoft_v10_20_05_e26_prof/setup

cd srcs

rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/exp/dune/app/users/weishi/testpns_np02G4/srcs

# Build the code:
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali/PDVD
# this uses SinglesGen particle gun
lar -c protoDUNE_VD_neutronDDG_gen.fcl -n 10     
# g4 stage 1
lar -c protodunevd_g4_stage1_PNS.fcl -n -1 ProtoDUNE_VD_mc_gen_pns_allbkg.root
# pure LAr g4 stage 2
lar -c protodunevd_g4_stage2_PNS.fcl -n -1 ProtoDUNE_VD_mc_gen_pns_allbkg_g4_stage1.root
# detsim pure LAr
lar -c protodunevd_detsim_PNS.fcl -n -1 ProtoDUNE_VD_mc_gen_pns_allbkg_g4_stage1_g4_stage2.root
# reco
lar -c protodunevd_reco_PNS.fcl -n -1 ProtoDUNE_VD_mc_gen_pns_allbkg_g4_stage1_g4_stage2_detsim.root
```

If re-login,

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v10_20_05d01 -q e26:prof
source /exp/dune/app/users/weishi/testpns_np02G4/localProducts_larsoft_v10_20_05_e26_prof/setup

mrbsetenv
cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali/PDVD

```

## Fast np02 data analysis with lardon
```
[First time only]
git clone https://github.com/weishi10141993/lardon.git

# install pixi:
curl -fsSL https://pixi.sh/install.sh | PIXI_HOME=/exp/dune/app/users/weishi/lardonnp02libs bash

[Every time]
cd lardon
source ~/.bashrc
pixi install -a

source set_lardon.sh

# at lxplus
# pixi run lardon -file /eos/experiment/neutplatform/protodune/dune/vd-protodune/9e/39/np02vd_raw_run042164_0000_df-s04-d0_dw_0_20260116T222256.hdf5 -gallery <your_choice>

# at fnal
pixi run lardon -file /pnfs/dune/persistent/users/weishi/examplenp02data/np02vd_raw_run042164_0000_df-s04-d0_dw_0_20260116T222256.hdf5 -gallery bottom
pixi run lardon -file /pnfs/dune/persistent/users/weishi/examplenp02data/pns/np02vd_raw_run042375_0003_df-s05-d0_dw_0_20260130T165435.hdf5 -gallery top # first pns run
```

where ```your_choice``` could be ```top``` : to see CRP2+CRP3, ```bottom``` to see CRP4+CRP5, ```both``` = bottom and then top separately

## Analyze np02 data under dunesw v10_07_00d00 LArSoft

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

To run the decoder on hdf5 files:
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

## Generate and sim Ar39 in np02 under dunesw v10_07_00d00 LArSoft
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


# PNS at NP02 Light Simulation Analysis Setup

On Fermilab Alma9 machines, the simulation needs to run in a SL7 container. Set up container first:
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"
```

First time set up
```
cd /exp/dune/app/users/weishi/PDSPNSCali
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof
mrb newDev
source /exp/dune/app/users/weishi/PDSPNSCali/localProducts_larsoft_v09_75_00_e20_prof/setup

cd srcs

git clone https://github.com/weishi10141993/larana.git -b pdspnscali

# Build the code:
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali
lar -c module0_v5.fcl -n 10     # this uses SinglesGen particle gun
```

If re-login,

```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof
source /exp/dune/app/users/weishi/PDSPNSCali/localProducts_larsoft_v09_75_00_e20_prof/setup
mrbsetenv

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali/PDVD
nohup lar -c module0_v5.fcl -n 10 >& output.log &

## Run on grid ##
tar -czvf PDSPNSCali.tar.gz /exp/dune/app/users/weishi/PDSPNSCali Setup_LArSoft.sh dunevdcb1_v2_refactored_M1.gdml dunevdcb1_v2_refactored_M1_nowires.gdml module1_v1data.fcl NeutronGen.txt

jobsub_submit -G dune -N 100 --memory=2000MB --disk=1GB --expected-lifetime=2h --cpu=1 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --tar_file_name=dropbox:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchLightSim/PDSPNSCali.tar.gz --use-cvmfs-dropbox -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --append_condor_requirements='(TARGET.HAS_Singularity==true&&TARGET.HAS_CVMFS_dune_opensciencegrid_org==true&&TARGET.HAS_CVMFS_larsoft_opensciencegrid_org==true&&TARGET.CVMFS_dune_opensciencegrid_org_REVISION>=1105&&TARGET.HAS_CVMFS_fifeuser1_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser2_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser3_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser4_opensciencegrid_org==true)' file:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchLightSim/Run_LightSim_grid.sh
```

# Analyze the light yield
```
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
nohup root -l -b -q LightYieldAna.C >& macrooutput.log &

# Run on grid
tar -czvf LightYieldAna.tar.gz Setup_LArSoft_LYA.sh LightYieldAna.C LightSim.txt

jobsub_submit -G dune -N 100 --memory=2000MB --disk=2GB --expected-lifetime=8h --cpu=1 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --tar_file_name=dropbox:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchAna/LightYieldAna.tar.gz --use-cvmfs-dropbox -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --append_condor_requirements='(TARGET.HAS_Singularity==true&&TARGET.HAS_CVMFS_dune_opensciencegrid_org==true&&TARGET.HAS_CVMFS_larsoft_opensciencegrid_org==true&&TARGET.CVMFS_dune_opensciencegrid_org_REVISION>=1105&&TARGET.HAS_CVMFS_fifeuser1_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser2_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser3_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser4_opensciencegrid_org==true)' file:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchAna/Run_LightAna_grid.sh
```
