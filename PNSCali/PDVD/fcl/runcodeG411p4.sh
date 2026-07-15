lar -c protoDUNE_VD_neutronDDG_gen.fcl -o gentestbkg_pns.root -n 1
lar -c protodunevd_g4_v11p4_stage1_PNS.fcl gentestbkg_pns.root -o g4stage1testbkg_pns.root
lar -c protodunevd_g4_stage2_PNS.fcl g4stage1testbkg_pns.root -o g4stage2testbkg_pns.root
lar -c protodunevd_detsim_PNS.fcl g4stage2testbkg_pns.root -o detsimtestbkg_pns.root
lar -c protodunevd_reco_PNS.fcl detsimtestbkg_pns.root -o recotestbkg_pns.root
lar -c run_pdvd_blipana.fcl recotestbkg_pns.root
