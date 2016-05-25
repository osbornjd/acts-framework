//
//  IService.hpp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef IService_h
#define IService_h

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include <string>

namespace FW {
    
    /** @class IService */
    class IService {
        
        public :
        /** Virutal Destructor */
        virtual ~IService(){}
        
        /** Framework intialize method */
        virtual ProcessCode initialize() = 0;
        
        /** Framework finalize mehtod */
        virtual ProcessCode finalize() = 0;
        
        /** Framework name() method */
        virtual const std::string& name() const = 0;
        
        /** return the MessageLevel */
        virtual MessageLevel messageLevel() const = 0;
        
    };
    
}

#endif /* IService_h */
