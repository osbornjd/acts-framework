# setup a LCG release via cvmfs
#
# must be called from the repository root
if [ -n "$BASH_SOURCE" ]; then
  this_script=$BASH_SOURCE
elif [ -n "$ZSH_VERSION" ]; then
  setopt function_argzero
  this_script=$0
else
  echo 1>&2 "Unsupported shell. Please use bash, or zsh."
  exit 2
fi

dir=$(dirname $(realpath $this_script))

source $dir/lcg_variables.sh

source ${lcg_view}/setup.sh
echo "using ${lcg_release} on ${lcg_platform}"

