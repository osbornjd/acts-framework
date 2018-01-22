#ifndef ACTFW_ALGORITHMS__HITDATA_H
#define ACTFW_ALGORITHMS__HITDATA_H

#include <vector>
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Utilities/Definitions.hpp"

namespace FW {

/// Definition of a two dimensional measurement. The components are the local
// position on the surface.
typedef Acts::
    Measurement<Identifier, Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1>
        Measurement2D;

/// @classe fccMeasurement
/// @brief Class to describe an fcc measurement
class fccMeasurement
{
public:
  // constructor
  fccMeasurement(const Acts::Surface& surface,
                 const Identifier&    id,
                 Acts::Measurement<Identifier,
                                   Acts::ParDef::eLOC_0,
                                   Acts::ParDef::eLOC_1>::CovMatrix_t cov,
                 const double&                                        loc0,
                 const double&                                        loc1,
                 const unsigned&                                      trackID)
    : m_measurement(surface, id, cov, loc0, loc1), m_trackID(trackID)
  {
  }

  // constructor using measurement
  fccMeasurement(const Measurement2D& measurement, const unsigned& trackID)
    : m_measurement(measurement), m_trackID(trackID)
  {
  }

  ///@return measurement
  const Measurement2D
  measurement() const
  {
    return m_measurement;
  }

  ///@return local position of measurement
  const Acts::Vector2D
  locPos() const
  {
    return Acts::Vector2D(m_measurement.get<Acts::ParDef::eLOC_0>(),
                          m_measurement.get<Acts::ParDef::eLOC_1>());
  }

  ///@return the trackID (identification of truth)
  const unsigned
  trackID() const
  {
    return m_trackID;
  }

private:
  Measurement2D m_measurement;
  unsigned      m_trackID;
};

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
  AnalysisParameters(const double& mean, const double& min, const double& max)
    : m_mean(mean), m_min(min), m_max(max)
  {
  }
  /// Destructor
  ~AnalysisParameters() = default;

  /// Access mean
  const double
  mean() const
  {
    return m_mean;
  }

  /// Access min
  const double
  min() const
  {
    return m_min;
  }

  /// Access max
  const double
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
