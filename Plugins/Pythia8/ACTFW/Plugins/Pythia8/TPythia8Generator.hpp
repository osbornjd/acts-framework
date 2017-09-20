//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_PYTHIA8GENERATOR_H
#define ACTFW_PYTHIA8GENERATOR_H

#include <memory>
#include <mutex>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Readers/IReaderT.hpp"

class TPythia8;

namespace FW {
namespace Pythia8 {

  /// @class IParticleReader
  ///
  /// Interface class that fills a vector of process vertices
  /// proerties for feeding into the fast simulation
  ///
  class TPythia8Generator
    : public FW::IReaderT<std::vector<Acts::ProcessVertex>>
  {
  public:
    struct Config
    {
      int    pdgBeam0  = 2212;    ///< pdg code of incoming beam 1
      int    pdgBeam1  = 2212;    ///< pdg code of incoming beam 2
      double cmsEnergy = 14000.;  ///< center of mass energy
      std::vector<std::string> processStrings
          = {{"HardQCD:all = on"}};  ///< pocesses
      std::string name = "TPythia8Generator";
    };

    /// Constructor
    /// @param cfg is the configuration class
    /// @param logger is the logger instance
    TPythia8Generator(const Config&                       cfg,
                      std::unique_ptr<const Acts::Logger> logger
                      = Acts::getDefaultLogger("TPythia8Generator",
                                               Acts::Logging::INFO));

    /// Framework name() method
    std::string
    name() const final override;

    // clang-format off
    /// @copydoc FW::IReaderT::read(std::vector<Acts::ProcessVertex>&,size_t,const FW::AlgorithmContext*)
    // clang-format on
    FW::ProcessCode
    read(std::vector<Acts::ProcessVertex>& pProperties,
         size_t                                 skip = 0,
         const FW::AlgorithmContext* context         = nullptr) final override;

  private:
    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return (*m_logger);
    }

    /// the configuration class
    Config m_cfg;
    /// the pythia object
    std::unique_ptr<TPythia8> m_pythia8;
    /// logger instance
    std::unique_ptr<const Acts::Logger> m_logger;
    /// mutex used to protect multi-threaded reads
    std::mutex m_read_mutex;
  };
}  // namespace Pythia8
}  // namespace FW

#endif  // ACTFW_PYTHIA8GENERATOR_H
