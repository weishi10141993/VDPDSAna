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
