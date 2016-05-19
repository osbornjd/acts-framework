//
//  ExtrapolationTestAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ExtrapolationTestAlgorithm_h
#define ExtrapolationTestAlgorithm_h

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>

namespace Acts {
    class IExtrapolationEngine;
}

namespace FW {
    class WhiteBoard;
    class IRandomNumbers;
}

namespace FWE {

    /** @class Algorithm */
    class ExtrapolationTestAlgorithm : public FW::Algorithm {
        
      public :
        /** @class Config */
        class Config : public FW::Algorithm::Config {
        public:
            std::shared_ptr<FW::RandomNumbers>          randomNumbers;
            std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine;
            size_t                                      drawsPerEvent;
        };
        
        /* Constructor*/
        ExtrapolationTestAlgorithm(const Config& cnf);
        
        /* Destructor*/
        ~ExtrapolationTestAlgorithm();
        
        /** Framework intialize method */
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /** Framework execode method */
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /** Framework finalize mehtod */
        FW::ProcessCode finalize() final;
    private:
        Config m_cfg; //!< the config class
        
    };

}


#endif