## PDVD light noise hunt

Under Alma9, set up the DAQ environment:
```
cd PDVDLightNoiseHunt
source /cvmfs/dunedaq.opensciencegrid.org/setup_dunedaq.sh
setup_dbt latest
dbt-create -l
dbt-create fddaq-v5.2.1-a9
```

Installing WAFFLES inside the DAQ environment:
```
cd fddaq-v5.2.1-a9
source env.sh # Activate your DAQ environment
# Ensure your SSH keys are properly set up, then:
cd ..
git clone git@github.com:DUNE/waffles.git
cd waffles
which python3 # Should show the .venv Python
python3 -m pip install -r requirements.txt .
```

Setting up Rucio to fetch raw data (alma9, lxplus at CERN, gvpn cluster at Fermilab):
```
cd fddaq-v5.2.1-a9
# from daq folder
source env.sh
cd waffles/scripts
source setup_rucio_a9.sh # need FNAL service account and authentication
python3 fetch_rucio_replicas.py --runs 28676 --max-files 5
```

File looking at (most recent circa May 7, 2025)
```
/pnfs/dune/persistent/users/weishi/PDVDNoiseHunt/processed_np02vd_raw_run036362_0000_df-s04-d0_dw_0_20250507T145213.hdf5.copied_structured.hdf5

/pnfs/dune/persistent/users/weishi/PDVDNoiseHunt/np02vd_raw_run036362_0000_df-s04-d0_dw_0_20250507T145213.hdf5.copied
```

# Light Simulation Analysis Setup

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
