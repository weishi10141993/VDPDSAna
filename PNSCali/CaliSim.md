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
lar -c module1_v1.fcl -n 100     # this uses SinglesGen particle gun
lar -c module1_v1data.fcl -n 100 # this uses gamma cascade input from neutron captures
```

If re-login,

```
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof
source /exp/dune/app/users/weishi/PDSPNSCali/localProducts_larsoft_v09_75_00_e20_prof/setup
mrbsetenv

cd /exp/dune/app/users/weishi/VDPDSAna/PNSCali
nohup lar -c module1_v1.fcl -n 10000 >& output.log &
 # this uses gamma cascade input from neutron captures
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
