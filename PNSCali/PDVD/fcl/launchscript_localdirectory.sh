#!/bin/bash

USERF=$USER
FNALURL='https://fndcadoor.fnal.gov:2880/dune/scratch/users'

htgettoken -a htvaultprod.fnal.gov -i dune 
#INPUT_TAR_DIR_LOCAL=$(justin-cvmfs-upload v102006_neutron.tar)
#INPUT_TAR_DIR_LOCAL=$(justin-cvmfs-upload tarballname.tar)
INPUT_TAR_DIR_LOCAL=$(justin-cvmfs-upload np02pnssim.tar)

echo "INPUT_TAR_DIR_LOCAL: ${INPUT_TAR_DIR_LOCAL}"
ls -l $INPUT_TAR_DIR_LOCAL

a=2
while true; do
  stat $INPUT_TAR_DIR_LOCAL > /dev/null 2>&1
  if [ $? == 0 ]; then
    echo "Found in RCDS"
    break
  fi

  echo "File not found in RCDS"
  echo "sleeping $a"
  sleep $a
  a=$((2*a))
done

: '
justin-test-jobscript --monte-carlo 1 \ #--mql "$MQL_QUERY" \
 --jobscript pdvd_blip_pds.jobscript \
 --env INPUT_TAR_DIR_LOCAL="$INPUT_TAR_DIR_LOCAL" \
 --env NUM_EVENTS=1 
exit
'


justin simple-workflow --monte-carlo 50 \
 --jobscript pdvd_blip_pds.jobscript \
 --env INPUT_TAR_DIR_LOCAL="$INPUT_TAR_DIR_LOCAL" \
 --rss-mb 15000 --max-distance 30 \
 --wall-seconds 54000 \
 --scope usertests \
 --output-pattern "*ana_hist.root:$FNALURL/$USERF" 
