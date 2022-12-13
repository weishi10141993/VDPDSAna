# Impact of noise light on VD PDS resolution

## Set up

The main module is ```duneopdet/OpticalDetector/SIPMOpSensorSim_module.cc```, where we are interested in the parameter ```DarkNoiseRate```(default to 10Hz). This is equivalently the noise rate, can be treated as a proxy for PoF light noise.

```
cd /dune/app/users/weishi
mkdir VDPDSRes
cd VDPDSRes

# Check everything:

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_58_01d00 -q e20:prof
mrb newDev
source /dune/app/users/weishi/VDPDSRes/localProducts_larsoft_v09_58_01_e20_prof/setup
cd srcs

mrb g dunesw
cd dunesw
git checkout tags/v09_58_01d00

cd ..
mrb g protoduneana  
cd protoduneana
git checkout tags/v09_58_01d00

cd ..
mrb g duneprototypes  
cd duneprototypes
git checkout tags/v09_58_01d00

cd ..
mrb g duneana  
cd duneana
git checkout tags/v09_58_01d00

cd ..
mrb g duneopdet
cd duneopdet
git checkout tags/v09_58_01d00

cd ..
mrb uc
cd $MRB_BUILDDIR
setup ninja
mrbsetenv
mrb i --generator ninja
```

Marley events (nue CC 5, 10, 15, 20, 25, 30MeV, 5k events) will be generated, modify the number of events and energy in
```
/dune/app/users/weishi/VDPDSRes/srcs/dunesw/fcl/dunefdvd/gen/supernova/prodmarley_nue_mono10_dunevd10kt_1x8x14_3view_30deg.fcl
```

the following xml file takes care of submitting jobs to the grid for gen-g4, detsim, reco, and ana:
```
cd /dune/app/users/weishi/VDPDSAna/MarleySimConfig
projectgui.py Marley_10MeV_darkcount_10Hz.xml
```

To change dark noise rate [study 10 Hz, 10^4Hz, 10^5Hz (1PE/10us), 10^6Hz (10PE/10us)], modify and recompile:
```
# FD2-VD: SIPMOpSensorSim_module.cc
/dune/app/users/weishi/VDPDSRes/srcs/duneopdet/OpticalDetector/SIPMOpSensorSim.fcl
# FD1-HD: OpDetDigitizerDUNE_module.cc (not of interest as there is no PoF)
/dune/app/users/weishi/VDPDSRes/srcs/duneopdet/OpticalDetector/opticaldetectormodules_dune.fcl
```

To list all files: ```find . -iname 'ana_hist*.root'```.

To produce resolution plot:
```
cd /dune/app/users/weishi/VDPDSAna/ERes

# first produce voxel LY calibration
echo 'gROOT->ProcessLine(".L Mergednobkgnd50kEtrue.C+"); gROOT->ProcessLine("Mergednobkgnd50kEtrue k"); gROOT->ProcessLine("k.Loop()")' | root -l -b

# then get resolution
echo 'gROOT->ProcessLine(".L Mergednobkgnd50kEtrue_Eres.C+"); gROOT->ProcessLine("Mergednobkgnd50kEtrue_Eres k"); gROOT->ProcessLine("k.Loop()")' | root -l -b
```

## Re-login
```
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_58_01d00 -q e20:prof
source /dune/app/users/weishi/VDPDSRes/localProducts_larsoft_v09_58_01_e20_prof/setup
mrbsetenv
cd /dune/app/users/weishi/
```

## Re-compile

```
cd $MRB_BUILDDIR
ninja install
```

For a complete rebuild,
```
cd $MRB_BUILDDIR
mrb z
mrbsetenv
setup ninja
mrb i --generator ninja
```


## Other study

SNB trigger rate & efficiency
