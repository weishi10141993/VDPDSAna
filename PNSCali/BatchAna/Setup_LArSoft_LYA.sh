#!/bin/bash

DIRECTORY=LightYieldAna
# we cannot rely on "whoami" in a grid job. We have no idea what the local username will be.
# Use the GRID_USER environment variable instead (set automatically by jobsub).
USERNAME=${GRID_USER}
echo "Got username"
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh

echo "dunesw is set"
export WORKDIR=${_CONDOR_JOB_IWD} # if we use the RCDS the localProducts area will be placed in $CONDOR_DIR_INPUT
if [ ! -d "$WORKDIR" ]; then
  export WORKDIR=`echo ~`
fi

# _CONDOR_JOB_IWD is /srv
echo "Check work dir _CONDOR_JOB_IWD: ls -l ${_CONDOR_JOB_IWD}"
ls -l ${_CONDOR_JOB_IWD}

echo "See where are we: pwd"
pwd

echo "copy other input files to ${_CONDOR_JOB_IWD}"
cp ${INPUT_TAR_DIR_LOCAL}/LightYieldAna.C .

echo "Check work dir again _CONDOR_JOB_IWD: ls -l ${_CONDOR_JOB_IWD}"
ls -l ${_CONDOR_JOB_IWD}

echo "Finished set up grid"
# INPUT_TAR_DIR_LOCAL is /srv/no_xfer/0/TRANSFERRED_INPUT_FILES/FDEff
# copy FDEff to local work dir _CONDOR_JOB_IWD: /srv
# now is /srv/FDEff/srcs
