#!/bin/bash
set -uo pipefail

# Check whether the use did specify the name of the example to be run
ARGC=$#
if [[ $ARGC -lt 1 ]]; then
  echo "Usage: "$0" <name of example>"
  exit
fi

# Compute the name of the example executable and its output file
executable=ACTFW$1Example
output=$1Test.root

# Drop any stale result from previous runs of the example
rm -f $output

# Run the example in multi-threaded mode, back up the results
eval "$executable"
result=$?
if [[ result -ne 0 ]]; then
  echo "Multi-threaded run failed!"
  exit result
fi
mt_output=MT$output
mv $output $mt_output

# Run the example in single-threaded mode, back up the results
eval "OMP_NUM_THREADS=1 $executable"
result=$?
if [[ result -ne 0 ]]; then
  echo "Single-threaded run failed!"
  exit result
fi
st_output=ST$output
mv $output $st_output

# Check whether the results were identical (up to thread-induced event reordering)
cmd="root -b -q -l -x -e '.x compareRootFiles.cpp(\"$st_output\", \"$mt_output\")'"
eval $cmd
result=$?

# Clean up, and return 0 if the results were identical
rm -f $mt_output $st_output
exit $result
