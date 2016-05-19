//
//  HelloWorldAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef HelloWorldAlgorithm_h
#define HelloWorldAlgorithm_h

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

#include <memory>

namespace FW {
    class WhiteBoard;
}

namespace FWE {

    /** @class Algorithm */
    class HelloWorldAlgorithm : public FW::Algorithm {
        
      public :
        /* Constructor*/
        HelloWorldAlgorithm(const FW::Algorithm::Config& cnf);
        
        /* Destructor*/
        ~HelloWorldAlgorithm();
        
        /** Framework intialize method */
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /** Framework execode method */
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /** Framework finalize mehtod */
        FW::ProcessCode finalize() final;
        
    };

}


#endif