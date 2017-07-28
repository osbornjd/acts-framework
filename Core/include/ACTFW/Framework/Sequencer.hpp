/// @file
/// @date 2016-05-11 Initial version
/// @date 2017-07-27 Clean up with simplified interfaces
/// @author Andreas Salzburger
/// @author Moritz Kiehn <msmk@cern.ch>

#ifndef ACTFW_SEQUENCER_H
#define ACTFW_SEQUENCER_H

#include <memory>
#include <string>
#include <vector>

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/IReader.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/IWriter.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// @class  Sequencer
///
/// This is the backbone of the mini framework, it initializes all algorithms,
/// calls execute per event and deals with the event store */
///
class Sequencer
{
public:
  struct Config
  {
    /// the serivces
    std::vector<std::shared_ptr<IService>> services = {};
    /// reader algorithms
    std::vector<std::shared_ptr<IReader>> readers = {};
    /// writer algorithms
    std::vector<std::shared_ptr<IWriter>> writers = {};
    /// algorithms
    std::vector<std::shared_ptr<IAlgorithm>> algorithms = {};
    /// job store logging level
    Acts::Logging::Level jobStoreLogLevel = Acts::Logging::INFO;
    /// event store logging level
    Acts::Logging::Level eventStoreLogLevel = Acts::Logging::INFO;
  };

  /// Constructor
  ///
  /// @param cfg is the configuration object
  Sequencer(const Config&                       cfg,
            std::unique_ptr<const Acts::Logger> logger
            = Acts::getDefaultLogger("Sequencer", Acts::Logging::INFO));
  ~Sequencer();

  /// Add services
  ///
  /// @param services is the vector of services to be added
  ProcessCode
  addServices(std::vector<std::shared_ptr<IService>> services);

  /// Add algorithms for reading
  ///
  /// @param readers is the vector of reader algorithms to be added
  ProcessCode
  addReaders(std::vector<std::shared_ptr<IReader>> readers);

  /// Add algorithms for writing
  ///
  /// @param writers is the vector of writer algorithms to be added
  ProcessCode
  addWriters(std::vector<std::shared_ptr<IWriter>> writers);

  /// Prepend algorithms
  ///
  /// @param algorithms is the vector of algorithms to be prepended
  ProcessCode
  prependEventAlgorithms(std::vector<std::shared_ptr<IAlgorithm>> algorithms);

  /// Append algorithms
  ///
  /// @param algorithms is the vector of algorithms to be appended
  ProcessCode
  appendEventAlgorithms(std::vector<std::shared_ptr<IAlgorithm>> algorithms);

  /// Run the event loop over the given number of events.
  ///
  /// @param events Number of events to process
  /// @param skip Number of events to skip before processing
  ///
  /// This will first initialize all configured services and algorithms, then
  /// run all configure algorithms for each event potentially parallelized, and
  /// then finalize all algorithms and services in reverse order.
  ProcessCode
  run(size_t events, size_t skip = 0);

private:
  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;

  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW

#endif  // ACTFW_SEQUENCER_H
