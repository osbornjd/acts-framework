# ACTS test framework

A test framework for the ACTS development: **absolutely not** intended
for production usage.

## Build requirements

The framework can be build in different configurations. The minimal setup
requires the ACTS library with the MaterialPlugin (default configuration) and
[ROOT >= 6.08](https://root.cern.ch/).

Additional code can be activated by defining one or more of the following
options during the `cmake` configuration step:

*   USE_DD4HEP
*   USE_GEANT4
*   USE_OPENMP
*   USE_PYTHIA8

Depending on the active options, additional ACTS plugins and external software
must be available. For details please see the `CMakeLists.txt` file.
