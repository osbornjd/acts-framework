///////////////////////////////////////////////////////////////////
// MaterialRootificationTest.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecWriter.hpp"
#include "MaterialRootification.hpp"
/*
int main() {

    size_t nEvents = 1;
    size_t numberofEvents = 3;

    // set up the writer
    FWRoot::RootMaterialTrackRecReader::Config
writerConfig("MaterialWriter",Acts::Logging::VERBOSE);;
    writerConfig.fileName   = "MaterialTrackRecords.root";
    writerConfig.treeName   = "MaterialTrackRecords";
    auto materialTrackRecWriter =
std::make_shared<FWRoot::RootMaterialTrackRecWriter>(writerConfig);

    // set up the reader
     FWRoot::RootMaterialTrackRecReader::Config
readerConfig("MaterialReader",Acts::Logging::VERBOSE);
     readerConfig.fileName   =
"/afs/cern.ch/work/j/jhrdinka/ACTS/ACTS/acts-test-fw/MaterialTrackRecords.root";
     readerConfig.treeName   = "MaterialTrackRecords";
     auto materialTrackRecReader =
std::make_shared<FWRoot::RootMaterialTrackRecReader>(readerConfig);

    // set up the algorithm
    FWE::MaterialRootification::Config algorithmConfig;
    algorithmConfig.name                = "MaterialRootification";
    algorithmConfig.materialTrackRecWriter = materialTrackRecWriter;
    algorithmConfig.materialTrackRecReader = materialTrackRecReader;

    auto materialRootAlg =
std::make_shared<FWE::MaterialRootification>(algorithmConfig);
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig;
    seqConfig.name = "MaterialRootificationSequencer";
    // now create the sequencer
    FW::Sequencer sequencer(seqConfig);
    sequencer.addServices({materialTrackRecWriter});
    sequencer.appendEventAlgorithms({materialRootAlg});

    // initialize loop
    sequencer.initializeEventLoop();
    // run the loop
    sequencer.processEventLoop(nEvents);
    // finalize loop
    sequencer.finalizeEventLoop();

}*/
