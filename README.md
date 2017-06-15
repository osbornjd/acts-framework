# ACTS test framework

A test framework for the ACTS development: **absolutely not** intended
for production usage.

## Build requirements

The framework can be build in different configurations. The default setup
requires a C++14 compatible compiler with OpenMP support, the ACTS library
with Core and MaterialPlugin (default configuration), and
[ROOT >= 6.08](https://root.cern.ch/).

Additional code can be activated/deactivated by defining one or more of the
following options

*   USE_DD4HEP
*   USE_GEANT4
*   USE_OPENMP (on by default)
*   USE_PYTHIA8

during the configuration step, e.g. as `cmake -DUSE_DD4HEP=on ...`. Depending
on the selected options, additional ACTS plugins and external software
must be available. For details please see the `CMakeLists.txt` file.

