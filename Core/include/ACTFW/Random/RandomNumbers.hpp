//
//  RandomNumbers.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef RandomNumbers_hpp
#define RandomNumbers_hpp

#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

#include <stdio.h>
#include <string>
#include <array>
#include <memory>
#include <random>

namespace FW {
    
    /** @enum class Distribution */
    enum class Distribution { uniform, gauss, landau, gamma };
    
    
    /** @class RandomNumbers
     
     An implementation of the std random numbers
     
     */
    
    typedef std::mt19937                                      RandomEngine;                 // Mersenne Twister
    typedef std::normal_distribution<double>                  GaussDist;                    // Normal Distribution
    typedef std::uniform_real_distribution<double>            UniformDist;                  // Uniform Distribution
    typedef std::gamma_distribution<double>                   GammaDist;                    // Gamma Distribution
    
    
    class RandomNumbers {
    public:
        
        /** @class Config
         Nested Configuration class */
        class Config {
        public:
            unsigned int            seed;
            std::array<double, 2>   uniform_parameters;
            std::array<double, 2>   gauss_parameters;
            std::array<double, 2>   landau_parameters;
            std::array<double, 2>   gamma_parameters;
            std::string             name;
            MessageLevel            msgLvl;
            
            Config():
            seed(1234567890),
            name("Anonymous"),
            msgLvl(MessageLevel::INFO)
            {}
        
        };
        
        /** Constructor */
        RandomNumbers(const Config& cfg);
    
        /** Destructor */
        ~RandomNumbers(){}
        
        /** Draw the random number */
        double draw(Distribution);
        
        /** Ask for the seed */
        unsigned int seed() const { return m_cfg.seed; }
        
        /** Draw random numbers */
        const std::string& name() const { return m_cfg.name; }
        
        /** Message Level */
        FW::MessageLevel messageLevel() const { return m_cfg.msgLvl; }
        
        /** Set the configuration */
        ProcessCode setConfiguration(const Config& sfg);
        
    private:
        
        Config           m_cfg;
        RandomEngine     m_engine;
        GaussDist        m_gauss;
        UniformDist      m_uniform;
        GammaDist        m_gamma;
        
    };
    
    
    
    

}


#endif /* RandomNumbers_hpp */
