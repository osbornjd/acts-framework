///////////////////////////////////////////////////////////////////
// IMaterialTrackRecReader.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALTRACKRECREADER_H
#define WRITERS_IMATERIALTRACKRECREADER_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <vector>


namespace Acts {
    class MaterialTrackRecord;
}

namespace FW {
    
    
    /// @class IMaterialTrackRecReader
    ///
    /// Interface class for reading MaterialTrackRecord entities
    /// 
    
    class IMaterialTrackRecReader : public IService {
        
    public:
        /// Virtual destructor
        virtual ~IMaterialTrackRecReader() = default;
        
        /// Reads in the MaterialTrackRecord entities
        virtual ProcessCode read() = 0;
        
        /// Returning the MaterialTrackRecord entities which have been read in
        virtual std::vector<Acts::MaterialTrackRecord> materialTrackRecords() = 0;
        
    };
}
#endif // WRITERS_IMATERIALTRACKRECREADER_H