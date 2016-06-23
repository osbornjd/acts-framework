//
//  RandomNumbers.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef ACTFW_RANDOM_RANDOMNUMBERS_H
#define ACTFW_RANDOM_RANDOMNUMBERS_H 1

#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <stdio.h>
#include <string>
#include <array>
#include <memory>
#include <random>

namespace FW {
    
    ///  @enum class Distribution 
    enum class Distribution { uniform, gauss, landau, gamma };
    
    
    /// @class RandomNumbers
    ///
    ///  An implementation of the std random numbers
    ///
    typedef std::mt19937                                      RandomEngine;                 // Mersenne Twister
    typedef std::normal_distribution<double>                  GaussDist;                    // Normal Distribution
    typedef std::uniform_real_distribution<double>            UniformDist;                  // Uniform Distribution
    typedef std::gamma_distribution<double>                   GammaDist;                    // Gamma Distribution
    
    
    class RandomNumbers {
    public:
        
        /// @class Config
        /// Nested Configuration class 
        class Config {
        public:
            std::shared_ptr<Acts::Logger>   logger;             ///< the default logger 
            unsigned int                    seed;               ///< random seed
            std::array<double, 2>           uniform_parameters; ///< configuration uniform
            std::array<double, 2>           gauss_parameters;   ///< configuration gauss
            std::array<double, 2>           landau_parameters;  ///< configuration landau
            std::array<double, 2>           gamma_parameters;   ///< configuration gamma 
            std::string                     name;               ///< name identifier
            
            Config(const std::string& lname = "Algorithm",
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
             : logger(Acts::getDefaultLogger(lname,lvl))
             , seed(1234567890)
             , name(lname)
            {}
        };
        
        /// Constructor 
        RandomNumbers(const Config& cfg);
    
        /// Destructor 
        ~RandomNumbers(){}
        
        /// Draw the random number 
        ///
        /// @param dPar is the indicator which distrubtion to be drawn
        double draw(Distribution dPar);
        
        /// Ask for the seed 
        unsigned int seed() const { return m_cfg.seed; }
        
        /// Draw random numbers 
        const std::string& name() const { return m_cfg.name; }
        
        /// Set the configuration 
        ProcessCode setConfiguration(const Config& sfg);
        
    private:
        
        Config           m_cfg;        ///< the configuration class
        RandomEngine     m_engine;     ///< the random engine
        GaussDist        m_gauss;      ///< gauss distribution
        UniformDist      m_uniform;    ///< uniform distribution
        GammaDist        m_gamma;      ///< gamma distribution
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
          return *m_cfg.logger;
        }   
        
        
    };
    
    
    
    

}


#endif // ACTFW_RANDOM_RANDOMNUMBERS_H
