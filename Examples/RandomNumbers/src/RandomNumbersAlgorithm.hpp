//
//  RandomNumbersAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H
#define ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H 1

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>

namespace FW {
    class WhiteBoard;
    class IRandomNumbers;
}

namespace FWE {

    /// @class Algorithm 
    class RandomNumbersAlgorithm : public FW::Algorithm {
        
      public :
        /// @class Config 
        class Config : public FW::Algorithm::Config {
        public:
            std::shared_ptr<FW::RandomNumbers> randomNumbers;
            size_t                             drawsPerEvent;
            
            Config(const std::string& lname = "Algorithm", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
              : Algorithm::Config(lname,lvl)
              , randomNumbers(nullptr)
              , drawsPerEvent(0)
            {}
            
        };
        
        /// Constructor
        RandomNumbersAlgorithm(const Config& cnf);
        
        /// Destructor
        ~RandomNumbersAlgorithm();
        
        /// Framework intialize method 
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /// Framework execode method 
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /// Framework finalize mehtod 
        FW::ProcessCode finalize() final;
        
    private:
        Config m_cfg; ///< the config class
        
    };

} 

#endif // ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H