///////////////////////////////////////////////////////////////////
// ExtrapolationUtils.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_EXTRAPOLATIONUTILS
#define ACTFW_EXTRAPOLATIONUTILS

#include <memory>

namespace Acts {
    class IExtrapolationEngine;
    class TrackingGeometry;
}

namespace FWE {

/// Method to setup the Extrapolation Engines
/// @param geo shared_ptr to the Acts::TrackingGeometry which should be used for the extrapolation
/// @return a fully initialized Acts::ExtrapoltionEngine
std::shared_ptr<Acts::IExtrapolationEngine>
    initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo);
    
}

#endif //ACTFW_EXTRAPOLATIONUTILS