#!/bin/bash

# Drop any stale result of the ExtrapolationExample
rm -f ExtrapolationTest.root

# Run the ExtrapolationExample in multi-threaded mode, back up the results
ACTFWExtrapolationExample
mv ExtrapolationTest.root ExtrapolationTest_MT.root

# Run the ExtrapolationExample in single-threaded mode, back up the results
OMP_NUM_THREADS=1 ACTFWExtrapolationExample
mv ExtrapolationTest.root ExtrapolationTest_ST.root

# Check whether the results were identical (up to thread-induced event reordering)
root -b -q -l -e '.X compareRootFiles.cpp("ExtrapolationTest_ST.root", "ExtrapolationTest_MT.root")'
result=$?

# Clean up, and return 0 if the results were identical
rm -f ExtrapolationTest_MT.root ExtrapolationTest_ST.root
exit $result
