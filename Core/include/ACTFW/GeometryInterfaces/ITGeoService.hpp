///////////////////////////////////////////////////////////////////
// ITGeoService.hpp
///////////////////////////////////////////////////////////////////

#ifndef GEOMETRYINTERFACES_ITGEOSERVICE_H
#define GEOMETRYINTERFACES_ITGEOSERVICE_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

class TGeoNode;

namespace FW {
    
    
    /// @class ITGeoService
    ///
    /// The ITGeoService is the interface to return the TGeoGeometry.
    ///
    /// @TODO solve problem with double inheritance
    
    class ITGeoService {//: public IService {

    public:
        /// Virtual destructor
        virtual ~ITGeoService() = default;
        /// Access to the TGeo geometry
        /// @return The world TGeoNode (physical volume)
        virtual TGeoNode* tgeoGeometry() = 0;
        
    };
}
#endif // GEOMETRYINTERFACES_ITGEOSERVICE_H