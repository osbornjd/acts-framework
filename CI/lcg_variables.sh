# identify the os and select a compiler
if [ "$(cat /etc/redhat-release | grep '^Scientific Linux .*6.*')" ]; then
  lcg_platform="x86_64-slc6-gcc8-opt"
elif [ "$(cat /etc/centos-release | grep 'CentOS Linux release 7')" ]; then
  lcg_platform="x86_64-centos7-gcc8-opt"
else
  echo "Unsupported OS" 1>&2
  exit 1
fi

# define the lcg view
lcg_basedir=/cvmfs/sft.cern.ch/lcg
lcg_release=LCG_95apython3
lcg_view=${lcg_basedir}/views/${lcg_release}/${lcg_platform}

# extra variables required to build acts
export DD4hep_DIR="${lcg_view}"
export HepMC3_DIR="${lcg_basedir}/releases/${lcg_release}/hepmc3/3.1.0/${lcg_platform}"
export HEPPID_INCLUDE_DIR="${lcg_view}/include"
export HEPPID_LIBRARY_DIR="${lcg_view}/lib"
export PYTHIA8_INCLUDE_DIR="${lcg_view}/include"
export PYTHIA8_LIBRARY_DIR="${lcg_view}/lib"
