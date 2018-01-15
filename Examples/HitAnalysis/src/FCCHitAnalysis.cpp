///////////////////////////////////////////////////////////////////
// FCCHitAnalysis.cpp
///////////////////////////////////////////////////////////////////

#include <boost/program_options.hpp>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/HitAnalysis/HitDistanceAlgorithm.hpp"
#include "ACTFW/HitAnalysis/HitDistanceAlgorithm.hpp"
#include "ACTFW/Plugins/DD4hep/DD4hepDetectorOptions.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/FCCedm/fccTrackHitReader.hpp"
#include "ACTFW/Plugins/FCCedm/fccTrackHitReaderOptions.hpp"
#include "ACTFW/Plugins/Root/RootHitDistanceAnalysisWriter.hpp"
#include "ACTFW/Plugins/Root/RootPositionWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"

/// This example reads in a collection of fcc::PositionedTrackerHit creates
/// measurements within Acts and calculates the distances of the hits on one
/// module. In the end the minimum, maximum and mean for each component are
/// written out per layer onto a root file.

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 100, 2);
  // add the dd4hep detector options
  FW::Options::addDD4hepOptions<po::options_description>(desc);
  // add the track hit reader options
  FW::Options::addFccTrackHitReaderOptions<po::options_description>(desc);
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;

  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc
      = std::make_shared<FW::DD4hep::GeometryService>(dd4HepDetectorConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  // Set up fccTrackHitReader
  auto hitReaderConfig
      = FW::Options::readFccTrackHitReaderConfig<po::variables_map>(vm);
  hitReaderConfig.trackingGeometry = dd4tGeometry;
  hitReaderConfig.mask
      = 0xfffffff;  // In FCChh currently only the first 28 bits are used
  hitReaderConfig.collection = "measurements";
  hitReaderConfig.treeName   = "events";
  // hitReaderConfig.branchName      = "overlaidPositionedTrackHits";
  hitReaderConfig.branchName      = "positionedHits";
  hitReaderConfig.missedPositions = "missedPositions";
  hitReaderConfig.foundPositions  = "foundPositions";
  auto hitReader
      = std::make_shared<FW::FCCedm::fccTrackHitReader>(hitReaderConfig);

  // Set up the Hit Analysis algorithm calculating the distance
  FW::HitDistanceAlgorithm::Config hitAlgorithmConfig;
  hitAlgorithmConfig.collection       = "measurements";
  hitAlgorithmConfig.layerHitAnalysis = "layerHitAnalysis";
  auto hitAlgorithm = std::make_shared<FW::HitDistanceAlgorithm>(
      hitAlgorithmConfig, logLevel);

  // Set up the writer
  FW::Root::RootHitDistanceAnalysisWriter::Config writerConfig;
  writerConfig.layerHitAnalysis = "layerHitAnalysis";
  writerConfig.filePath         = "distance_analysis.root";
  writerConfig.fileMode         = "RECREATE";
  writerConfig.treeName         = "distance_analysis";
  auto writer
      = std::make_shared<FW::Root::RootHitDistanceAnalysisWriter>(writerConfig);

  // @todo remove later
  // Set up positions writer
  FW::Root::RootPositionWriter::Config posWriterConfig;
  posWriterConfig.positions = "missedPositions";
  posWriterConfig.filePath  = "missedPositions.root";
  posWriterConfig.fileMode  = "RECREATE";
  posWriterConfig.treeName  = "positions";
  auto posWriter
      = std::make_shared<FW::Root::RootPositionWriter>(posWriterConfig);

  FW::Root::RootPositionWriter::Config posWriterConfig1;
  posWriterConfig1.positions = "foundPositions";
  posWriterConfig1.filePath  = "foundPositions.root";
  posWriterConfig1.fileMode  = "RECREATE";
  posWriterConfig1.treeName  = "positions";
  auto posWriter1
      = std::make_shared<FW::Root::RootPositionWriter>(posWriterConfig1);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({});
  sequencer.addReaders({hitReader});
  sequencer.appendEventAlgorithms({hitAlgorithm});
  sequencer.addWriters({writer, posWriter, posWriter1});
  // The reader determines the number of events
  sequencer.run(nEvents);

  return 0;
}
