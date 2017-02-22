///////////////////////////////////////////////////////////////////
// IMaterialTrackRecWriter.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALTRACKRECWRITER_H
#define WRITERS_IMATERIALTRACKRECWRITER_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <vector>

namespace Acts {
    class MaterialTrackRecord;
}
namespace FW {
    
    /// @class IMaterialTrackRecWriter
    /// Interface to write out a vector of MaterialTrackRecord entities
    ///
    
    class IMaterialTrackRecWriter : public IService {
        
    public:
        /// Virtual destructor
        virtual ~IMaterialTrackRecWriter() = default;
        
        /// Writes out the MaterialTrackRecord entities
        virtual ProcessCode write(const Acts::MaterialTrackRecord& mtrecord) = 0;
        
    };
}
#endif // WRITERS_IMATERIALTRACKRECWRITER_H