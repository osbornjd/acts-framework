#ifndef ACTS_DD4HEPDETECTOR_H
#define ACTS_DD4HEPDETECTOR 1

// STL include(s)
#include <memory>
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/Units.hpp"

// ACTS include(s)
namespace Acts {
class TrackingGeometry;
}
namespace DD4hepExample {
std::shared_ptr<const Acts::TrackingGeometry>
trackingGeometry(std::string          xmlFileName,
                 Acts::Logging::Level lvl,
                 Acts::BinningType    bTypePhi,
                 Acts::BinningType    bTypeR,
                 Acts::BinningType    bTypeZ,
                 double               envelopeR,
                 double               envelopeZ);

void
extrapolation(std::shared_ptr<const Acts::TrackingGeometry> geometry,
              Acts::Logging::Level                          eLogLevel,
              size_t                                        nEvents,
              size_t                                        nTests,
              std::array<double, 2> etaRange,
              std::array<double, 2> phiRange);
}

#endif  // ACTS_DD4HEPDETECTOR_H
