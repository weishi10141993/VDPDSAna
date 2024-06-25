#!/bin/bash
echo "Export miniconda env"
export PATH=/exp/dune/app/users/weishi/miniconda3/bin:$PATH

echo "Running lardon..."
for ifile in {0..214}; do
  echo "Running: /exp/dune/app/users/weishi/miniconda3/envs/lardenv/bin/python3 lardon.py -det cbbot -run 25036 -sub ${ifile} -trk -pds -out coldbox_maxperview4_outlierdmax2500_25036_${ifile}"
  /exp/dune/app/users/weishi/miniconda3/envs/lardenv/bin/python3 lardon.py -det cbbot -run 25036 -sub ${ifile} -trk -pds -out coldbox_maxperview4_outlierdmax2500_25036_${ifile}
done

echo "Done."
