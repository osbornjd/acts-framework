//
//  IService.hpp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_FRAMEWORK_ISERVICE_H
#define ACTFW_FRAMEWORK_ISERVICE_H

#include <string>

#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {
    
    /// @class IService
    class IService {
        
        public :
        /// Virutal Destructor
        virtual ~IService(){}
        
        /// Framework intialize method
        virtual ProcessCode initialize() = 0;
        
        /// Framework finalize mehtod
        virtual ProcessCode finalize() = 0;
        
        /// Framework name() method
        virtual const std::string& name() const = 0;
        
    };
    
}

#endif // ACTFW_FRAMEWORK_ISERVICE_H
