# setup a LCG release via cvmfs
#
# must be called from the repository root
 
source CI/lcg_variables.sh
source ${lcg_view}/setup.sh
echo "using ${lcg_release} on ${lcg_platform}"
