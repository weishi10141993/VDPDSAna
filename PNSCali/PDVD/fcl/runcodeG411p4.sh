lar -c protoDUNE_VD_neutronDDG_gen.fcl -o gentestbkg_pns.root -n 1

# Create a short symlink to the long cvmfs path, nudex limit to 1000 characters
ln -s /cvmfs/larsoft.opensciencegrid.org/products/g4nudex/v1_0/G4NUDEXLIB1.0 ./nudex

# Verify it's short enough (should be well under 100 chars)
echo ./nudex/LevelDensities/level-densities-bfmeff.dat | wc -c

# Set the environment variable to the short path
export G4NUDEXLIBDATA=./nudex

# Verify
echo $G4NUDEXLIBDATA
ls $G4NUDEXLIBDATA/LevelDensities/
lar -c protodunevd_g4_v11p4_stage1_PNS.fcl gentestbkg_pns.root -o g4stage1testbkg_pns.root

lar -c protodunevd_g4_stage2_PNS.fcl g4stage1testbkg_pns.root -o g4stage2testbkg_pns.root
lar -c protodunevd_detsim_PNS.fcl g4stage2testbkg_pns.root -o detsimtestbkg_pns.root
lar -c protodunevd_reco_PNS.fcl detsimtestbkg_pns.root -o recotestbkg_pns.root
lar -c run_pdvd_blipana.fcl recotestbkg_pns.root
