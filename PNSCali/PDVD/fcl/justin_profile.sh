#!/bin/bash


source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup python v3_9_15
setup rucio
setup justin
setup metacat
#setup kx509

#kx509
voms-proxy-init -noregen -rfc -voms dune:/dune/Role=Analysis

export RUCIO_ACCOUNT=$USER

#export X509_USER_PROXY=$(kx509 | cut -d ' ' -f 3)
#export X509_USER_KEY=$(kx509 | cut -d ' ' -f 3)

export DATA_DISPATCHER_URL=https://metacat.fnal.gov:9443/dune/dd/data
export DATA_DISPATCHER_AUTH_URL=https://metacat.fnal.gov:8143/auth/dune

export METACAT_AUTH_SERVER_URL=https://metacat.fnal.gov:8143/auth/dune ;
export METACAT_SERVER_URL=https://metacat.fnal.gov:9443/dune_meta_prod/app

metacat auth mydn
metacat version
metacat auth whoami
htgettoken -a htvaultprod.fnal.gov -i dune
export RUCIO_CONFIG=~/rucio.cfg.oidc
