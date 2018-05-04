#!/bin/sh -ex
#
# setup requirements on lxplus6

release=LCG_90a
basedir=/cvmfs/sft.cern.ch/lcg
platform=x86_64-slc6-gcc62-opt
view=$basedir/views/$release/${platform}

source ${view}/setup.sh
# additional variables that are not set automatically
export BOOST_ROOT="${view}"
export DD4hep_DIR="${view}"
export EIGEN_INCLUDE_DIR="${view}/include/eigen3"
export PYTHIA8_INCLUDE_DIR="${view}/include"
export PYTHIA8_LIBRARY_DIR="${view}/lib"
export HEPMC_DIR="${basedir}/releases/${release}/HepMC/2.06.09/${platform}"
export HEPMC3_DIR="${basedir}/releases/${release}/hepmc3/3.0.0/${platform}"
export HEPPID_INCLUDE_DIR="${view}/include"
export HEPPID_INCLUDE_DIR="${view}/lib"
