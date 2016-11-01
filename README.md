# ACTS Minimal Framework

Test framework for ACTS development, not intended for production usage.

## Build requirements

In addition to the build requirements of ACTS, the test framework has the following extra dependencies:

+ [ROOT](https://root.cern.ch/) (>= 6.06.04) is used for file output
+ [cmake](https://cmake.org/) version requirements are bumped to 3.1+

## Using recent CMake releases from CVMFS

Recent releases of CMake can be found on CVMFS at `/cvmfs/sft.cern.ch/lcg/contrib/CMake/`. These builds are self-contained and intended to work even on non-Redhat linux distributions. In particular, they have been successfully used on Ubuntu 14.04.

To use the CMake release x.y.z from this source, do...

    export PATH=/cvmfs/sft.cern.ch/lcg/contrib/CMake/x.y.z/Linux-x86_64/bin:${PATH}
