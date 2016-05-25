//  IAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef IAlgorithm_h
#define IAlgorithm_h

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include <string>
#include <memory>

namespace FW {
    
    
    class WhiteBoard;
    
    /** @class IAlgorithm */
    class IAlgorithm {
  
      public :
        /** Virutal Destructor */
        virtual ~IAlgorithm(){}
        
        /** Framework intialize method */
        virtual ProcessCode initialize(std::shared_ptr<WhiteBoard> eventStore = nullptr,
                                       std::shared_ptr<WhiteBoard> jobStore = nullptr) = 0;
        
        /** Framework execode method */
        virtual ProcessCode execute(size_t eventNumnber) = 0;
        
        /** Framework finalize mehtod */
        virtual ProcessCode finalize() = 0;
        
        /** Framework name() method */
        virtual const std::string& name() const = 0;
        
        /** return the MessageLevel */
        virtual MessageLevel messageLevel() const = 0;

        /** return the eventStore - things that live per event*/
        virtual std::shared_ptr<WhiteBoard> eventStore() const = 0;
        
        /** return the jobStore - things that live for the full job */
        virtual std::shared_ptr<WhiteBoard> jobStore() const = 0;
        
    };
         
}

#endif /* IAlgorithm_h */
