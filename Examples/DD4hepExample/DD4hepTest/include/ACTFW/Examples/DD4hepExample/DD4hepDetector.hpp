#ifndef ACTS_DD4HEPDETECTOR_H
#define ACTS_DD4HEPDETECTOR 1

// STL include(s)
#include <memory>
#include "ACTS/Utilities/Logger.hpp"

// ACTS include(s)
namespace Acts {
class TrackingGeometry;
}
namespace DD4hepExample {
std::shared_ptr<const Acts::TrackingGeometry>
trackingGeometry(std::string xmlFileName, Acts::Logging::Level lvl);
void
extrapolation(std::shared_ptr<const Acts::TrackingGeometry> geometry,
              Acts::Logging::Level                          eLogLevel);
}

#endif  // ACTS_DD4HEPDETECTOR_H
