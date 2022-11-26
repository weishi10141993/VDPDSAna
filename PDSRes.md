# Impact of noise light on VD PDS resolution

## Set up

The main module is ```duneopdet/OpticalDetector/SIPMOpSensorSim_module.cc```, where we are interested in the parameter ```DarkNoiseRate```(default to 10Hz). This is equivalently the noise rate, can be treated as a proxy for PoF light noise.

```
cd /dune/app/users/weishi
mkdir VDPDSRes
cd VDPDSRes

source /grid/fermiapp/products/dune/setup_dune.sh
ups list -aK+ dunesw
setup dunesw v09_63_00d01 -q e20:prof
mrb newDev
source /dune/app/users/weishi/VDPDSRes/localProducts_larsoft_v09_63_00_e20_prof/setup
cd srcs
mrb g duneopdet
cd $MRB_BUILDDIR
setup ninja
mrbsetenv
mrb i --generator ninja
```

Marley events (nue CC 5-30MeV, 5k events) will be generated, simulated, and reconstructed via the following xml file:

```
# 10 MeV
wget https://raw.githubusercontent.com/weishi10141993/VDPDSAna/main/Marley_10MeV_darkcount_10Hz

projectgui.py Marley_10MeV_darkcount_10Hz.xml
```

To produce resolution plot:
```
/dune/app/users/apaudel/code/PDEnergyresolution_tdr/PDEcal_Sept11/.C ==> produce voxel LY calibration
/dune/app/users/apaudel/code/PDEnergyresolution_tdr/PDEcal_Sept11/*Res ==> produce resolution
```

## Re-login
```
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_63_00d01 -q e20:prof
source /dune/app/users/weishi/VDPDSRes/localProducts_larsoft_v09_63_00_e20_prof/setup
mrbsetenv
cd /dune/app/users/weishi/
```

## Other study

1. PDS Energy resolution: analysis of track with highest charge + blips in time and position coincidence
2. SNB trigger rate & efficiency
