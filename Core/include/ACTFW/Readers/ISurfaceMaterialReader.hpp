///////////////////////////////////////////////////////////////////
// ISurfaceMaterialReader.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_READERS_IMATERIALREADER_H
#define ACTFW_READERS_IMATERIALREADER_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"


namespace Acts {
    class SurfaceMaterial;
}

namespace FW {
    
    
    /// @class ISurfaceMaterialReader
    ///
    /// Interface class for reading in the  material 
    ///
    
    class ISurfaceMaterialReader : public IService {
        
    public:
        /// Virtual destructor
        virtual ~ISurfaceMaterialReader() = default;
        
        /// Writes out the material map of the 
        virtual ProcessCode read() = 0;
        
    };
}
#endif // ACTFW_READERS_IMATERIALREADER_H