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

## ACTS Submodule

To keep track of the correct combination of framework and ACTS version,
a copy of the ACTS library is provided as a git submodule in the `acts`
directory. Please have a look at the
[git-submodule documentation][git-book-submodule] for an introduction to
how git submodules work.

If you clone the framework repository you have to either clone with the
`--recursive` option or run the following commands afterwards

    git submodule init
    git submodule update

to download a copy of the specified version. The framework repository
only stores the remote path and the commit id of the used ACTS version.
On your branch you can update it to the specific branch that you are
working on, but please switch to a tagged-version or the master branch
before merging back.

The ACTS version defined by the submodule is also used to run the
continous integration on Gitlab and you can the scripts are therefore
run automatically with your specific version.

[git-book-submodules]: https://git-scm.com/book/en/v2/Git-Tools-Submodules
