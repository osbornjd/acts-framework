///////////////////////////////////////////////////////////////////
/// IDD4hepService.hpp
///////////////////////////////////////////////////////////////////

#ifndef GEOMETRYINTERFACES_IDD4HEPSERVICE_H
#define GEOMETRYINTERFACES_IDD4HEPSERVICE_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace DD4hep {
    namespace Geometry {
        class DetElement;
        class LCDD;
    }
}


namespace FW {
    
    
    /// @class IDD4hepService
    ///
    /// Interface class for a service returning the DD4hep geometry.
    
    class IDD4hepService : public IService {
        
    public:
        /// Virtual destructor
        virtual ~IDD4hepService() = default;
        /// Access to the DD4hep geometry
        /// @return The world DD4hep DetElement
        virtual DD4hep::Geometry::DetElement dd4hepGeometry() = 0;
        /// Access to the interface of the DD4hep geometry
        virtual DD4hep::Geometry::LCDD* lcdd() = 0;
        
    };
}
#endif // GEOMETRYINTERFACES_IDD4HEPSERVICE_H