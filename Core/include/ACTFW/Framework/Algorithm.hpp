//
//  Algorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_FRAMEWORK_ALGORITHM_H
#define ACTFW_FRAMEWORK_ALGORITHM_H 1

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include <string>
#include <memory>

namespace FW {
        
    /// @class Algorithm 
    ///
    ///  Base implementation of an algorithm
    ///  
    class Algorithm : public IAlgorithm {
      public :

        /// @class Config
        ///  nested class definition */
        class Config {
          public:
            std::shared_ptr<Acts::Logger>   logger; ///< the default logger 
            std::shared_ptr<WhiteBoard>     eBoard; ///< the event WhiteBoard
            std::shared_ptr<WhiteBoard>     jBoard; ///< the event JobBoard
            std::string                     name;   ///< the name of the algorithm
            
            Config(const std::string& lname = "Algorithm", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
             : logger(Acts::getDefaultLogger(lname,lvl))
             , eBoard(nullptr)
             , jBoard(nullptr)
             , name(lname)
            {}
            
        };
        
        /// Constructor
        ///
        /// @param cfg is the configuration struct
        Algorithm(const Config& cfg);
        
        /// Destructor
        ~Algorithm();
        
        /// Framework intialize method 
        ///
        /// @param eventStore is the WhiteBoard that gets cleared every event
        /// @param jobStore is the WhiteBoard that gets cleared at the end of the job
        virtual ProcessCode initialize(std::shared_ptr<WhiteBoard> eventStore = nullptr,
                                       std::shared_ptr<WhiteBoard> jobStore = nullptr) override;
        
        /// Framework execode method 
        virtual ProcessCode execute(size_t eventNumber) override;
        
        /// Framework finalize mehtod 
        virtual ProcessCode finalize() override;
        
        /// Framework name() method 
        const std::string& name() const final;
        
        /// return the eventStore - things that live per event
        std::shared_ptr<WhiteBoard> eventStore() const final;
        
        /// return the jobStore - things that live for the full job 
        std::shared_ptr<WhiteBoard> jobStore() const final;
        
      protected:
        Config                      m_cfg;   ///< the config object
      
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
          return *m_cfg.logger;
        }   
        
    };
    
    inline std::shared_ptr<WhiteBoard> Algorithm::eventStore() const { return m_cfg.eBoard; }

    inline std::shared_ptr<WhiteBoard> Algorithm::jobStore() const { return m_cfg.jBoard; }
    
    inline const std::string& Algorithm::name() const { return m_cfg.name; }
    
}


#endif // ACTFW_FRAMEWORK_ALGORITHM_H
