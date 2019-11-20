# setup a LCG release via cvmfs
#
# must be called from the repository root
dir=$(dirname $(realpath $BASH_SOURCE))
 
source $dir/lcg_variables.sh
source ${lcg_view}/setup.sh
echo "using ${lcg_release} on ${lcg_platform}"
