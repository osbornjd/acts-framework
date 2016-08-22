///////////////////////////////////////////////////////////////////
/// IGeant4Service.hpp
///////////////////////////////////////////////////////////////////

#ifndef GEOMETRYINTERFACES_IGEANTSERVICE_H
#define GEOMETRYINTERFACES_IGEANTSERVICE_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

class G4VUserDetectorConstruction;


namespace FW {
    
    
     /// @class IGeant4Service
     ///
     /// The IGeant4Service is an interface class to return the Geant4 geometry.
    
    class IGeant4Service : public IService {
        
    public:
        /// virtual destructor
        virtual ~IGeant4Service() = default;
        /// Access to the geant4 geometry
        /// @return G4VUserDetectorConstruction from which the Geant4 geometry is constructed
        virtual G4VUserDetectorConstruction* geant4Geometry() = 0;
        
    };
}
#endif // GEOMETRYINTERFACES_IGEANTSERVICE_H