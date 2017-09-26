//
//  WhiteBoardAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H
#define ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H 1

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FWE {

/// @class Algorithm
///
/// Test algorithm for the WhiteBoard writing/reading
///
class WhiteBoardAlgorithm : public FW::BareAlgorithm
{
public:
  /// @class Config
  /// Nested Configuration class for the WhiteBoardAlgorithm
  /// It extends the Algorithm::Config Class
  struct Config
  {
    // Input collection of DataClassOne (optional)
    std::string inputClassOneCollection  = "";
    // Output collection of DataClassOne (optional)
    std::string outputClassOneCollection = "";
    // Input collection of DataClassTwo (optional)
    std::string inputClassTwoCollection  = "";
    // Output collection of DataClassTwo (optional)
    std::string outputClassTwoCollection = "";
  };

  /// Constructor
  ///
  /// @param cfg is the configruation
  WhiteBoardAlgorithm(const Config& cfg,
  Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execode method
  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FWE

#endif
