# Analyze reconstructed ColdBox data (on Fermilab dunegpvm*)

```
# preliminary reconstructed data from L. Zambelli: 3 max raw hits per view, 2.5cm outlier
/pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco
# single hits reconstruct with 4 max raw hits per view, 2.5cm outlier
/pnfs/dune/persistent/users/weishi/PNSPDSColdBox/reco_sh_maxperview4_outlierdmax2p5

# location on CERN lxplus eos
/eos/user/l/lzambell/analysis/coldbox/lardon/reco/CRP6
```
To view the h5 file, install hdfview (with brew on mac).

To analyze the .h5 files:
```
[First time only]
pip install --force-reinstall --target=/exp/dune/app/users/weishi/lardonlibs tables
pip install --force-reinstall --target=/exp/dune/app/users/weishi/lardonlibs fast-histogram
pip install --force-reinstall --target=/exp/dune/app/users/weishi/lardonlibs colorcet
pip install --force-reinstall --target=/exp/dune/app/users/weishi/lardonlibs "matplotlib==3.1.3"
pip install --force-reinstall --target=/exp/dune/app/users/weishi/lardonlibs scipy

[Everytime relogin]
export PYTHONPATH=/exp/dune/app/users/weishi/lardonlibs:$PYTHONPATH
source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
spack load root@6.28.12

# LZreco:
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25036/cbbot_25036_*.h5 >& output_25036_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25068/cbbot_25068_*.h5 >& output_25068_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25071/cbbot_25071_*.h5 >& output_25071_LZ.log &

nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25004/cbbot_25004_*.h5 >& output_25004_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25066/cbbot_25066_*.h5 >& output_25066_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25078/cbbot_25078_*.h5 >& output_25078_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25084/cbbot_25084_*.h5 >& output_25084_LZ.log &
nohup python ncap_pds_minimumcut_jinst_timesliced_PEstudy.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25086/cbbot_25086_*.h5 >& output_25086_LZ.log &
```

Here are PD channel maps:
```
PD channel map

# April 16-17 run: 25036, 25034-1k events
    # C1->0, 7
    # C2->10, 17
    # C3->20, 27
    # C4->40, 47
    # HD-->1
    # VD-->36
# April 18-19 run 25068, 25071
    # C1->0, 7
    # C2->10, 17
    # C3->30, 37
    # C4->40, 47
    # HD-->1
    # VD-->26

# PDS only run
25050, 25080
# cosmic
25078
```

# Reconstruct ColdBox data (on Fermilab dunegpvm*)

We use an existing framework developed by L. Zambelli called [LArDon](https://github.com/dune-lardon/lardon).

[First time only]
```
# Install minicoda
mkdir -p ~/miniconda3
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda3/miniconda.sh
bash ~/miniconda3/miniconda.sh -b -u -p ~/miniconda3
rm -rf ~/miniconda3/miniconda.sh

# initiate minicoda
~/miniconda3/bin/conda init bash
```

Install LArdon,
```
[First time only]
git clone https://github.com/weishi10141993/lardon.git

[Everytime relogin]
export PATH=/exp/dune/app/users/weishi/miniconda3/bin:$PATH

[First time only]
conda env create -f lardenv.yml
# activate the environment
conda activate lardenv
# check env is activated --> this doesn't seem to work properly
conda env list
```

Raw data files are stored at fnal:
```
# before 25050
/pnfs/dune/tape_backed/dunepro/vd-coldbox/raw/2024/detector/test/None/00/02
# after 25050
/pnfs/dune/tape_backed/dunepro/vd-coldbox/raw/2024/detector/cosmics/None/00/02
```

```
# run lardon to reconstruct events
# change json files in settings/cbbot regarding reco parameters and detector geo
/exp/dune/app/users/weishi/miniconda3/envs/lardenv/bin/python3 lardon.py -det cbbot -run 25036 -sub 0000 -trk -pds -out coldbox_LZreco_25036_0000
```

# Generate gamma cascades

```
cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali/NeutronGen
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
root -l -b -q 'generate_txtgen_w_t0.C(1, 10000, "VDCB_PNS_Side_1_cap_per_evt_10k_evts")'
# 1 means 1 neutron capture per event, can be more than 1 captures

## Run on grid ##
tar -czvf BatchNeutronGen.tar.gz coldbox_side_dist.root generate_txtgen_w_t0.C Outputname.txt

jobsub_submit -G dune -N 100 --memory=1000MB --disk=1GB --expected-lifetime=1h --cpu=1 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --tar_file_name=dropbox:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchNeutronGen/BatchNeutronGen.tar.gz --use-cvmfs-dropbox -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --append_condor_requirements='(TARGET.HAS_Singularity==true&&TARGET.HAS_CVMFS_dune_opensciencegrid_org==true&&TARGET.HAS_CVMFS_larsoft_opensciencegrid_org==true&&TARGET.CVMFS_dune_opensciencegrid_org_REVISION>=1105&&TARGET.HAS_CVMFS_fifeuser1_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser2_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser3_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser4_opensciencegrid_org==true)' file:///exp/dune/app/users/weishi/VDPDSAna/PNSCali/BatchNeutronGen/Run_NeutronGen_grid.sh
```

To cp files from scratch to perisitent:
```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc
export IFDH_TOKEN_ENABLE=0
export IFDH_PROXY_ENABLE=1
ifdh cp -D /pnfs/dune/scratch/users/weishi/PNSCaliNeutronGen/* .
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
lar -c module1_v1.fcl -n 10     # this uses SinglesGen particle gun
lar -c module1_v1data.fcl -n 10 # this uses gamma cascade input from neutron captures
```

If re-login,

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof
source /exp/dune/app/users/weishi/PDSPNSCali/localProducts_larsoft_v09_75_00_e20_prof/setup
mrbsetenv

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali
nohup lar -c module1_v1data.fcl -n 10000 >& output.log &

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

## Set up edep-sim

Run edep-sim to simulate event energy deposits in LAr.

```
# Work in dunegpvm alma9
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup geant4 v4_10_6_p01e -q e20:prof
setup edepsim v3_2_0 -q e20:prof

edep-sim \
    -C \
    -g LArBath.gdml \
    -o edep_LArBath_6.1MeV_gammas_10k.root \
    -u \
    -e 10000 \
    Gen_edepsim_3gamma.mac
```

## analyze fluka sim
