//
//  WhiteBoardAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef WhiteBoardAlgorithm_h
#define WhiteBoardAlgorithm_h

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>

namespace FWE {
    
    /** @class Algorithm */
    class WhiteBoardAlgorithm : public FW::Algorithm {
        
        public :
        /** @class Config
         Nested Configuration class for the WhiteBoardAlgorithm
         It extends the Algorithm::Config Class*/
        class Config : public FW::Algorithm::Config
        {
          public:
            std::string inputClassOneCollection;
            std::string outputClassOneCollection;
            std::string inputClassTwoCollection;
            std::string outputClassTwoCollection;
        };
        
        /* Constructor*/
        WhiteBoardAlgorithm(const Config& cnf);
        
        /* Destructor*/
        ~WhiteBoardAlgorithm();
        
        /** Framework intialize method */
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /** Framework execode method */
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /** Framework finalize mehtod */
        FW::ProcessCode finalize() final;
        
        private :
        Config m_cfg;
        
    };
    
}


#endif