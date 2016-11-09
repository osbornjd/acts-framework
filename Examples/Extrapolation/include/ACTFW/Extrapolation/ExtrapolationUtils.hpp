///////////////////////////////////////////////////////////////////
// ExtrapolationUtils.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_EXTRAPOLATIONUTILS
#define ACTFW_EXTRAPOLATIONUTILS

#include <memory>

#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"

namespace Acts {
    class IExtrapolationEngine;
    class TrackingGeometry;
}

namespace FWE {

/// Method to setup the Extrapolation Engines
/// @param geo shared_ptr to the Acts::TrackingGeometry which should be used for the extrapolation
/// @return a fully initialized Acts::ExtrapoltionEngine
template <class MagneticField = Acts::ConstantBField>
std::unique_ptr<Acts::IExtrapolationEngine>
    initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo, std::shared_ptr<MagneticField> magField, Acts::Logging::Level eLogLevel);

}

#include "ExtrapolationUtils.ipp"

#endif //ACTFW_EXTRAPOLATIONUTILS
