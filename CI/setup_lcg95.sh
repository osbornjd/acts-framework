# setup LCG release 95 via cvmfs

# determine os release
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env_info.sh

release=LCG_95
platform=x86_64-${ACTS_OS}-${ACTS_COMPILER}
basedir=/cvmfs/sft.cern.ch/lcg
lcg=${basedir}/views/${release}/${platform}

source ${lcg}/setup.sh
# extra variables required to build acts
export DD4hep_DIR=${lcg}
export PYTHIA8_INCLUDE_DIR="${lcg}/include"
export PYTHIA8_LIBRARY_DIR="${lcg}/lib"
export HEPMC3_DIR="${basedir}/releases/${release}/hepmc3/3.0.0/${platform}"
export HEPPID_INCLUDE_DIR="${lcg}/include"
export HEPPID_LIBRARY_DIR="${lcg}/lib"
