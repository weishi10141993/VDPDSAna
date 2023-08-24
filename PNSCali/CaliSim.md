```
source /grid/fermiapp/products/dune/setup_dune.sh
setup dunesw v09_75_00d00 -q e20:prof

lar -c module1_v1.fcl -n 100     # this uses SinglesGen particle gun
lar -c module1_v1data.fcl -n 100 # this uses gamma cascade input from neutron captures
```
