#ifndef ACTFW_ALGORITHMS__HITDATA_H
#define ACTFW_ALGORITHMS__HITDATA_H

#include <vector>
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Utilities/Definitions.hpp"

namespace FW {

// Definition of a two dimensional measurement. The components are the local
// position on the surface.
typedef Acts::
    Measurement<Identifier, Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1>
        Measurement2D;

/// @class AnalysisParameters
///
/// @brief Class to store outcome of simple analysis
///
/// The AnalysisParameters are just a simple class to to store parameters
/// related to some analysis. Currently it contains three parameters: the mean,
/// the minimum and the maximum.
class AnalysisParameters
{
public:
  /// Constructor
  AnalysisParameters(double mean, double min, double max)
    : m_mean(mean), m_min(min), m_max(max)
  {
  }
  /// Destructor
  ~AnalysisParameters() = default;

  /// Access mean
  const double&
  mean() const
  {
    return m_mean;
  }

  /// Access min
  const double&
  min() const
  {
    return m_min;
  }

  /// Access max
  const double&
  max() const
  {
    return m_max;
  }

private:
  /// The mean
  double m_mean;
  /// The minimum
  double m_min;
  /// The maximum
  double m_max;
};
}

#endif  // ACTFW_ALGORITHMS__HITDATA_H
