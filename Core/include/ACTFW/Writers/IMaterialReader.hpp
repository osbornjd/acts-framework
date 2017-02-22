///////////////////////////////////////////////////////////////////
// IMaterialReader.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALREADER_H
#define WRITERS_IMATERIALREADER_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"


namespace Acts {
    class SurfaceMaterial;
}

namespace FW {
    
    
    /// @class IMaterialReader
    ///
    /// Interface class for reading in the  material 
    ///
    
    class IMaterialReader : public IService {
        
    public:
        /// Virtual destructor
        virtual ~IMaterialReader() = default;
        
        /// Writes out the material map of the 
        virtual ProcessCode read() = 0;
        
    };
}
#endif // WRITERS_IMATERIALREADER_H