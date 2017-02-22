///////////////////////////////////////////////////////////////////
// ITrackingGeometryService.hpp
///////////////////////////////////////////////////////////////////

#ifndef GEOMETRYINTERFACES_ITRACKINGGEOMETRYSERVICE_H
#define GEOMETRYINTERFACES_ITRACKINGGEOMETRYSERVICE_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>


namespace Acts {
    class TrackingGeometry;
}

namespace FW {
    
    
    /// @class ITrackingGeometryService
    ///
    /// The ITrackingGeometryService is the interface to access the ACTS tracking geometry.
    ///
    /// @TODO solve problem with double inheritance
    
    class ITrackingGeometryService {//: public IService {
        
    public:
        /// virtual destructor
        virtual ~ITrackingGeometryService() = default;
        /// Access to the ACTS TrackingGeometry
        virtual std::unique_ptr<const Acts::TrackingGeometry> trackingGeometry() = 0;
        
    };
}
#endif // GEOMETRYINTERFACES_ITRACKINGGEOMETRYSERVICE_H