//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef IExtrapolationCellWriter_h
#define IExtrapolationCellWriter_h

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"

namespace Acts {
    
    typedef ExtrapolationCell<TrackParameters>   ExCellCharged;
    typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;
}

namespace FW {
    /** @class IExtrapolatiionCellWriter 
     Interface class for extrapolation cell writers*/
    class IExtrapolationCellWriter : public IService {
    public:
        /** Virtual Destructor */
        virtual ~IExtrapolationCellWriter() {}
        
        /** The write interface */
        virtual ProcessCode write(const Acts::ExCellCharged& eCell) = 0;

        /** The write interface */
        virtual ProcessCode write(const Acts::ExCellNeutral& eCell) = 0;

    };
    
}


#endif /* IExtrapolationCellWriter_h */
