//
//  WhiteBoardAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H
#define ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H 1

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>

namespace FWE {
    
    /// @class Algorithm 
    ///
    /// Test algorithm for the WhiteBoard writing/reading
    ///
    class WhiteBoardAlgorithm : public FW::Algorithm {
        
        public :
        /// @class Config
        /// Nested Configuration class for the WhiteBoardAlgorithm
        /// It extends the Algorithm::Config Class
        class Config : public FW::Algorithm::Config
        {
          public:
            std::string inputClassOneCollection;
            std::string outputClassOneCollection;
            std::string inputClassTwoCollection;
            std::string outputClassTwoCollection;
            
            Config(const std::string& lname = "Algorithm", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
              : Algorithm::Config(lname,lvl)
              , inputClassOneCollection("")
              , outputClassOneCollection("")
              , inputClassTwoCollection("")
              , outputClassTwoCollection("")
            {}
        };
        
        /// Constructor
        ///
        /// @param cfg is the configruation 
        WhiteBoardAlgorithm(const Config& cfg);
        
        /// Destructor
        ~WhiteBoardAlgorithm();
        
        /// Framework intialize method 
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /// Framework execode method 
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /// Framework finalize mehtod 
        FW::ProcessCode finalize() final;
        
        private :
        Config m_cfg;
        
    };
    
}


#endif