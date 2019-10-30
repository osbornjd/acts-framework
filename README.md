# ACTS test framework

A test framework for ACTS development. **Absolutely not** intended
for production usage.

## Build requirements

The framework can be build in different configurations. The minimal setup
requires a C++14 compatible compiler, the Intel TBB library, ROOT, and all
requirements of the core ACTS library. Additional packages might be required
depending on which of the following built options

*   USE_DD4HEP
*   USE_GEANT4
*   USE_HEPMC3
*   USE_PYTHIA8
*   USE_TGEO

are activated/deactivated during the configuration step, e.g. as `cmake
-DUSE_DD4HEP=on ...`. The ACTS Core library and the ACTS Fatras library will be
built automatically as part of the framework built process. For details please
see the `CMakeLists.txt` file.

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

## Guidelines for writing an algorithm

All examples in the framework are based on a simple event loop, i.e. a logic
that executes a set of services, readers, processing algorithms, and writers for
each event. The available algorithms differ in their purpose and consequently
in the interface they provide:

*   A service is intended to provide slowly changing or constant information,
    e.g. geometry and alignment, for each event. A service implements the
    `IService` interface  with the `startRun` and `prepare` methods.
    Computationally expensive initialization should be executed in the
    `startRun` method. A service can have an internal state and each
    implementation has to ensure that concurrent calls are valid. Services are
    called before all other algorithms.
*   A reader should be used to read per-event data from disk and provide it
    to other algorithms via the event store. A reader implements the `IReader`
    interface and with the `availableEvents` and `read` methods. A reader
    can have an internal state and each implementation has to ensure that
    concurrent calls are valid. Readers are always called after all services.
*   An algorithm is anything that processes event data e.g. simulations
    or reconstruction algorithms should use this interface. An algorithm
    implements the `IAlgorithm` interface with the `execute` method.
    An algorithm **must not** have an internal state in order to be callable
    concurrently without additional precautions. Processing algorithms are
    called after the readers. Most algorithms should try to use the
    `BareAlgorithm` helper class to simplify the implementation.
*   A writer takes existing data from the event store and writes it to disk.
    A writer implements the `IWriter` interface and with the `write` and
    `endRun` methods. A writer can not modify the event store but can
    have an internal state. Each implementation has to ensure that
    concurrent calls are valid. Writers are called after all other algorithms.

All algorithms also have to implement the `name` method to be able
to identify them.

To simplify interactions between different algorithms please adhere to
the following guidelines:

*   Communication between different algorithms **must** only happen via the
    event store provided by the `AlgorithmContext`. Output collections or
    objects should be transfered to the store at the end of the execution.
    To allow to run the same algorithm with multiple configurations the names of
    input and output objects should be configurable for an algorithm.
*   Try to avoid tight coupling between algorithms. Use the predefined event
    data types to exchange data. That way algorithms can be easily exchanged.
*   Strictly separate computation from input and output. Input and output
    algorithms must be implemented using the `IReader` and `IWriter` interface.
    They should only read in the data from file and add them to the event store
    or read objects from the event store and write them to file. Again,
    names of the objects that are read/written should be configurable.

Examples should be written in a modular way using the framework components,
such as writers and the sequencer. Options are to be defined using the
`boost::program_options` syntax.


[git-book-submodules]: https://git-scm.com/book/en/v2/Git-Tools-Submodules
