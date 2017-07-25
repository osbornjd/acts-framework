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
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Framework/IService.hpp"
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
  /// @class Config
  ///
  /// Nested, public configuration class for the algorithm sequences
  struct Config
  {
    /// the serivces
    std::vector<std::shared_ptr<IService>> services = {};
    /// i/o algorithms
    std::vector<std::shared_ptr<IOAlgorithm>> ioAlgorithms = {};
    /// algorithms
    std::vector<std::shared_ptr<IAlgorithm>> eventAlgorithms = {};
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

  /// Destructor
  ~Sequencer();

  /// Add the algorithms - for Services
  ///
  /// @param iservices is the vector of services to be added
  ProcessCode
  addServices(std::vector<std::shared_ptr<IService>> iservices);

  /// Add the algorithms - for IO
  ///
  /// @param ioalgs is the vector of I/O algorithms to be added
  ProcessCode
  addIOAlgorithms(std::vector<std::shared_ptr<IOAlgorithm>> ioalgs);

  /// Prepend algorithms
  ///
  /// @param ialgs is the vector of algorithms to be prepended
  ProcessCode
  prependEventAlgorithms(std::vector<std::shared_ptr<IAlgorithm>> ialgs);

  /// Append algorithms
  ///
  /// @param ialgs is the vector of algorithms to be appended
  ProcessCode
  appendEventAlgorithms(std::vector<std::shared_ptr<IAlgorithm>> ialgs);

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

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

inline ProcessCode
Sequencer::addServices(std::vector<std::shared_ptr<IService>> iservices)
{
  for (auto& isvc : iservices) {
    if (!isvc) {
      ACTS_FATAL("Trying to add empty Service to Sequencer. Abort.");
      return ProcessCode::ABORT;
    }
    ACTS_INFO("Sucessfully added IO Algorithm " << isvc->name()
                                                << " to Seqeuencer.");
    m_cfg.services.push_back(std::move(isvc));
  }
  return ProcessCode::SUCCESS;
}

inline ProcessCode
Sequencer::addIOAlgorithms(std::vector<std::shared_ptr<IOAlgorithm>> ioalgs)
{
  for (auto& ioalg : ioalgs) {
    if (!ioalg) {
      ACTS_FATAL("Trying to add empty IO Algorithm to Sequencer. Abort.");
      return ProcessCode::ABORT;
    }
    ACTS_INFO("Sucessfully added IO Algorithm " << ioalg->name()
                                                << " to Seqeuencer.");
    m_cfg.ioAlgorithms.push_back(std::move(ioalg));
  }
  return ProcessCode::SUCCESS;
}

inline ProcessCode
Sequencer::prependEventAlgorithms(
    std::vector<std::shared_ptr<IAlgorithm>> ialgs)
{
  for (auto& ialg : ialgs) {
    if (!ialg) {
      ACTS_FATAL(
          "Trying to prepend empty Event Algorithm to Sequencer. Abort.");
      return ProcessCode::ABORT;
    }
    ACTS_INFO("Sucessfully prepended Event Algorithm " << ialg->name()
                                                       << " to Seqeuencer.");
    m_cfg.eventAlgorithms.insert(m_cfg.eventAlgorithms.begin(),
                                 std::move(ialg));
  }
  return ProcessCode::SUCCESS;
}

inline ProcessCode
Sequencer::appendEventAlgorithms(std::vector<std::shared_ptr<IAlgorithm>> ialgs)
{
  for (auto& ialg : ialgs) {
    if (!ialg) {
      ACTS_FATAL("Trying to append empty Event Algorithm to Sequencer. Abort.");
      return ProcessCode::ABORT;
    }
    m_cfg.eventAlgorithms.push_back(ialg);
    ACTS_INFO("Sucessfully appended Event Algorithm " << ialg->name()
                                                      << " to Seqeuencer.");
  }
  return ProcessCode::SUCCESS;
}
}  // namespace FW

#endif  // ACTFW_SEQUENCER_H
