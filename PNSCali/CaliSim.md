# Analyze reconstructed ColdBox data (on Fermilab dunegpvm*)

```
# preliminary reconstructed data from L. Zambelli
/pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco

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

nohup python ncap_fastpdsearch.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25078/cbbot_25078_*.h5 >& output_25078.log &
nohup python ana_blip.py /pnfs/dune/persistent/users/weishi/PNSPDSColdBox/LZreco/cbbot_25068/cbbot_25068_100_newcrp.h5 >& output_25068.log &
```

Here are PD channel maps:
```
PD channel map

# April 16-17 (run: 25034)
    # C1->0, 7
    # C2->10, 17
    # C3->20, 27
    # C4->40, 47
    # HD-->1
    # VD-->36
# April 18-19 run 25050, 25068, 25071
    # C1->0, 7
    # C2->10, 17
    # C3->30, 37
    # C4->40, 47
    # HD-->1
    # VD-->26
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

```
# run lardon to reconstruct events
# change json files in settings/cbbot regarding reco parameters and detector geo

# test
/exp/dune/app/users/weishi/miniconda3/envs/lardenv/bin/python3 lardon.py -det cbbot -run 25036 -sub 0000 -trk -pds -out coldbox_LZreco_25036_test

# PNS runs we need
25068
25071
...
```

# Generate gamma cascades

```
cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali/NeutronGen
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
root -l -b -q 'generate_txtgen_w_t0.C(1, 10000, "VDCB_PNS_Side_1_cap_per_evt_10k_evts")'
# 1 means 1 neutron capture per event, can be more than 1 captures
```

# Analysis Setup

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
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof
source /exp/dune/app/users/weishi/PDSPNSCali/localProducts_larsoft_v09_75_00_e20_prof/setup
mrbsetenv

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali
nohup lar -c module1_v1.fcl -n 10000 >& output.log &
```

To analyze the light yield,
```
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
nohup root -l -b -q LightYieldAna.C >& macrooutput.log &
```

## Set up edep-sim

Run edep-sim to simulate event energy deposits in LAr.

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup geant4 v4_10_6_p01e -q e20:prof
setup edepsim v3_2_0 -q e20:prof

edep-sim \
    -C \
    -g LArBath.gdml \
    -o edep_LArBath_1.2MeV_gamma.root \
    -u \
    -e 200 \
    Gen_edepsim.mac
```
