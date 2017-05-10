//
//  Algorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_FRAMEWORK_ALGORITHM_H
#define ACTFW_FRAMEWORK_ALGORITHM_H 1

#include <memory>
#include <string>

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// @class Algorithm
///
///  Base implementation of an algorithm
///
class Algorithm : public IAlgorithm
{
public:
  /// @class Config
  ///  nested class definition */
  struct Config
  {
    /// the job WhiteBoard
    std::shared_ptr<WhiteBoard> jBoard = nullptr;
    /// the name of the algorithm
    std::string name = "Algorithm";

    Config() = default;
    Config(std::string name_) : name(std::move(name_)) {}
  };

  /// Constructor
  ///
  /// @param cfg is the configuration struct
  Algorithm(const Config&                 cfg,
            std::unique_ptr<Acts::Logger> logger
            = Acts::getDefaultLogger("Algorithm", Acts::Logging::INFO));

  /// Destructor
  ~Algorithm();

  /// Framework intialize method
  ///
  /// @param jobStore is the WhiteBoard that gets cleared at the end of the job
  virtual ProcessCode
  initialize(std::shared_ptr<WhiteBoard> jobStore   = nullptr) override;

  /// Framework execute method
  ///
  /// @param context contains all the processing data that is specific to one
  ///                algorithm execution: active event info, algorithm ID...
  virtual ProcessCode
  execute(const AlgorithmContext context) const override;

  /// Framework finalize mehtod
  virtual ProcessCode
  finalize() override;

  /// Framework name() method
  const std::string&
  name() const final override;

  /// return the jobStore - things that live for the full job
  std::shared_ptr<WhiteBoard>
  jobStore() const final override;

protected:
  Config                        m_cfg;
  std::unique_ptr<Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

inline std::shared_ptr<WhiteBoard>
Algorithm::jobStore() const
{
  return m_cfg.jBoard;
}

inline const std::string&
Algorithm::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_FRAMEWORK_ALGORITHM_H
