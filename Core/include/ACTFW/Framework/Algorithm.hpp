//
//  Algorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef Algorithm_h
#define Algorithm_h

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <string>
#include <memory>

namespace FW {
        
    /** @class Algorithm */
    class Algorithm : public IAlgorithm {
      public :

      /**@class Config
          nested class definition */
        class Config {
          public:
            std::shared_ptr<WhiteBoard> eBoard; //!< the event WhiteBoard
            std::shared_ptr<WhiteBoard> jBoard; //!< the event JobBoard
            std::string                 name;   //!< the name of the algorithm
            MessageLevel                msgLvl; //!< the message level of this algorithm
            
            Config() :
             eBoard(nullptr),
             jBoard(nullptr),
             name("Anonymous"),
             msgLvl(MessageLevel::INFO)
            {}
            
        };
        
        /* Constructor*/
        Algorithm(const Config& cnf);
        
        /* Destructor*/
        ~Algorithm();
        
        /** Framework intialize method */
        virtual ProcessCode initialize(std::shared_ptr<WhiteBoard> eventStore = nullptr,
                                       std::shared_ptr<WhiteBoard> jobStore = nullptr) override;
        
        /** Framework execode method */
        virtual ProcessCode execute(size_t eventNumber) override;
        
        /** Framework finalize mehtod */
        virtual ProcessCode finalize() override;
        
        /** Framework name() method */
        const std::string& name() const final;
        
        /** return the MessageLevel */
        MessageLevel messageLevel() const final;
        
        /** return the eventStore - things that live per event*/
        std::shared_ptr<WhiteBoard> eventStore() const final;
        
        /** return the jobStore - things that live for the full job */
        std::shared_ptr<WhiteBoard> jobStore() const final;
        
      protected:
        Config m_cfg;   //!< the config object
        
    };
    
    inline std::shared_ptr<WhiteBoard> Algorithm::eventStore() const { return m_cfg.eBoard; }

    inline std::shared_ptr<WhiteBoard> Algorithm::jobStore() const { return m_cfg.jBoard; }
    
    inline const std::string& Algorithm::name() const { return m_cfg.name; }
    
    inline MessageLevel Algorithm::messageLevel() const { return m_cfg.msgLvl; }
    

}


#endif /* Algorithm_h */
