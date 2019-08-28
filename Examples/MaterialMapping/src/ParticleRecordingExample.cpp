// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <string>
#include <boost/program_options.hpp>
#include "ACTFW/DD4hepDetector/DD4hepDetectorOptions.hpp"
#include "ACTFW/DD4hepDetector/DD4hepGeometryService.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/MaterialMapping/OutcomeRecording.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"

#include "ACTFW/Plugins/Geant4/OREventAction.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTFW/EventData/Barcode.hpp"

#include "ACTFW/DD4hepDetector/DD4hepDetector.hpp"
#include "../../Common/src/detail/FatrasDigitizationBase.hpp"
#include "../../Common/src/detail/FatrasEvgenBase.hpp"
#include "../../Common/src/detail/FatrasSimulationBase.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"


namespace po = boost::program_options;

using PartRec = std::vector<std::vector<FW::Geant4::ParticleRecord>>;
using WriteIt = FW::WriterT<PartRec>;

namespace FW
{
class ParticleRecordWriting : public WriteIt
{
public:

	struct Config
    {
      std::string collection;              ///< particle collection to write
      std::shared_ptr<BarcodeSvc>
             barcodeSvc;          ///< the barcode service to decode (optional)
    };
    
    /// Constructor
    ///
    /// @param cfg Configuration struct
    /// @param level Message level declaration
    ParticleRecordWriting(const Config&        cfg,
                       Acts::Logging::Level level = Acts::Logging::INFO) : WriteIt(cfg.collection, "ParticleRecordWriter", level), m_cfg(cfg) {}

    /// Virtual destructor
    ~ParticleRecordWriting() override {}

    /// End-of-run hook
    ProcessCode
    endRun() final override 
    {
		  return ProcessCode::SUCCESS;
	} // TODO: Maybe write the particles here to file

  protected:
    /// @brief Write method called by the base class
    /// @param [in] context is the algorithm context for event information
    /// @param [in] vertices is the process vertex collection for the
    /// particles to be attached
    ProcessCode
    writeT(const AlgorithmContext&       context,
           const PartRec& vertices) final override
	{
		int eventNr = context.eventNumber;
		
		std::cout << "evtNr: " << eventNr << std::endl;
		for(const auto& evt : vertices)
		{
			std::cout << "New Track: " << std::endl;
			for(const auto& p : evt)
			{
				//~ if(p.volume == "No volume")
					std::cout << "TID: " << p.trackid << ",Parent: " << p.parentid << ", PID: " << p.pdg <<
						", Position: (" << p.position[0] << ", " << p.position[1] << ", " << p.position[2] << 
						"), Momentum: (" << p.momentum[0] << ", " << p.momentum[1] << ", " << p.momentum[2] << "), Volume: " << p.volume << ", Process: " << p.process << std::endl;				
			}
		}
		return ProcessCode::SUCCESS;
	}

  private:
    Config     m_cfg;         ///< The config class
};
}

FW::Sequencer
fatrasSequencerBuild(boost::program_options::variables_map& vm, DD4hepDetector& detector)
{
	FW::Sequencer fatrasSequencer(FW::Options::readSequencerConfig(vm));

  // Create the random number engine
  auto randomNumberSvcCfg = FW::Options::readRandomNumbersConfig(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbers>(randomNumberSvcCfg);

  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(barcodeSvcCfg);

  // The geometry, material and decoration
  auto geometry          = FW::Geometry::build(vm, detector);
  auto tGeometry         = geometry.first;
  auto contextDecorators = geometry.second;
  // Add the decorator to the sequencer
  for (auto cdr : contextDecorators) {
    fatrasSequencer.addContextDecorator(cdr);
  }

  // (A) EVGEN
  // Setup the evgen input to the simulation
  setupEvgenInput(vm, fatrasSequencer, barcodeSvc, randomNumberSvc);

  // (B) SIMULATION
  // Setup the simulation
  setupSimulation(vm, fatrasSequencer, tGeometry, barcodeSvc, randomNumberSvc);

  // (C) DIGITIZATION
  // Setup the digitization
  setupDigitization(vm, fatrasSequencer, barcodeSvc, randomNumberSvc);
  
  // (D) TRUTH TRACKING

  // (E) PATTERN RECOGNITION
  
  
  return fatrasSequencer;
}

int
main(int argc, char* argv[])
{
	using po::value;
	
  DD4hepDetector detector;

  // Declare the supported program options.
  // Setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addOutputOptions(desc);
  FW::Options::addDD4hepOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addParticleGunOptions(desc); // TODO: Replace whatever is given here and in outcomerecoptions
  FW::Options::addBFieldOptions(desc);
  FW::Options::addFatrasOptions(desc);
  FW::Options::addRandomNumbersOptions(desc);
  FW::Options::addMaterialOptions(desc);
  FW::Options::addPythia8Options(desc);
  FW::Options::addRandomNumbersOptions(desc);
  FW::Options::addDigitizationOptions(desc);
  desc.add_options()("evg-input-type",
                     value<std::string>()->default_value("pythia8"),
                     "Type of evgen input 'gun', 'pythia8'");

  // Add specific options for this geometry
  detector.addOptions(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  FW::Sequencer g4Sequencer(FW::Options::readSequencerConfig(vm));
  int    randomSeed1 = 536235167;
  int    randomSeed2 = 729237523;

  FW::Sequencer fatrasSequencer = fatrasSequencerBuild(vm, detector);

  Acts::GeometryContext geoContext;

  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc = std::make_shared<FW::DD4hep::DD4hepGeometryService>(
      dd4HepDetectorConfig);
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry(geoContext);

  // DD4Hep to Geant4 conversion
  //
  FW::DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4",
                                               Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc = std::make_shared<FW::DD4hepG4::DD4hepToG4Svc>(dgConfig);

  // --------------------------------------------------------------------------------
  // Geant4 JOB:
  // --------------------------------------------------------------------------------
  // set up the writer for
  // ---------------------------------------------------------------------------------

  // set up the algorithm writing out the material map
  FW::OutcomeRecording::Config g4rConfig = FW::Options::readOutcomeRecordingConfig<po::variables_map>(vm);
  g4rConfig.geant4Service  = dd4hepToG4Svc;
  g4rConfig.seed1          = randomSeed1;
  g4rConfig.seed2          = randomSeed2;
  g4rConfig.particleCollection = "geant-outcome-tracks";
  
  // create the geant4 algorithm
  auto g4rAlgorithm
      = std::make_shared<FW::OutcomeRecording>(g4rConfig, Acts::Logging::INFO);

  // Output directory
  std::string particleCollection = g4rConfig.particleCollection;

    // Write the propagation steps as ROOT TTree
    FW::ParticleRecordWriting::Config config;
    config.collection  = particleCollection;
    auto writer
        = std::make_shared<FW::ParticleRecordWriting>(
            config);
    g4Sequencer.addWriter(writer);

  // Append the algorithm and run
  g4Sequencer.addAlgorithm(g4rAlgorithm);
  g4Sequencer.run();
}