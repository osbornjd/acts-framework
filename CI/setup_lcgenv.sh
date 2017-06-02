#!/bin/sh -ex
#
# setup requirements via lcgenv

lcgenv=/cvmfs/sft.cern.ch/lcg/releases/lcgenv/latest/lcgenv
base=/cvmfs/sft.cern.ch/lcg/releases/LCG_88
platform=x86_64-centos7-gcc62-opt

export LCGENV_PATH=${base}
eval "$(${lcgenv} ${platform} gcc)"
eval "$(${lcgenv} ${platform} CMake --no-gcc)"
eval "$(${lcgenv} ${platform} ninja --no-gcc)"
eval "$(${lcgenv} ${platform} DD4hep --no-gcc --developer)" # w/ ROOT, Geant4
eval "$(${lcgenv} ${platform} pythia8 226 --no-gcc --developer)"
# additional variables that are not set automatically
export BOOST_ROOT="${LCGENV_PATH}/Boost/1.62.0/${platform}"
export DD4hep_DIR="${LCGENV_PATH}/DD4hep/00-20/${platform}"
export EIGEN_INCLUDE_DIR="${LCGENV_PATH}/eigen/3.2.9/${platform}/include/eigen3"
export PYTHIA8_INCLUDE_DIR="${LCGENV_PATH}/MCGenerators/pythia8/226/${platform}/include"
export PYTHIA8_LIBRARY_DIR="${LCGENV_PATH}/MCGenerators/pythia8/226/${platform}/lib"
