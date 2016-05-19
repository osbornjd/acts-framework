//
//  AlgorithmSequencer.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef Sequencer_h
#define Sequencer_h

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Framework/IAlgorithm.hpp"
#include <memory>
#include <vector>
#include <string>

namespace FW {
    
    class WhiteBoard;
    
    /** @class  Sequencer
        
     This is the backbone of the mini framework, it initializes all algorithms,
     calls execute per event and deals with the event store */
     
    class Sequencer {
        
      public:
        /** @class Config 
         Nested, public configuration class */
        class Config {
          public:
            
            std::vector< std::shared_ptr<IOAlgorithm> > ioAlgorithms;    //!< i/o algorithms
            std::vector< std::shared_ptr<IAlgorithm> >  eventAlgorithms; //!< algorithms
            std::shared_ptr<WhiteBoard>                 eventBoard;      //!< event board
            std::shared_ptr<WhiteBoard>                 jobBoard;        //!< the job board
            std::string                                 name;            //!< the name of this sequencer
            MessageLevel                                msgLvl;          //!< the message level of this algorithm

            
            Config():
              ioAlgorithms(),
              eventAlgorithms(),
              eventBoard(nullptr),
              jobBoard(nullptr),
              name("Anonymous"),
              msgLvl(MessageLevel::INFO)
            {}
            
        };
        
        /** Constructor*/
        Sequencer(const Config& cnf);
        
        /** Destructor*/
        ~Sequencer();

        /** Add the algorithms - for IO */
        ProcessCode addIOAlgorithms(std::vector< std::shared_ptr<IOAlgorithm> > ioalgs);

        /** Add an event algorithm : prepend */
        ProcessCode prependEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs);

        /** Add an event algorithm : append */
        ProcessCode appendEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs);

        /** initialize event loop */
        ProcessCode initializeEventLoop();
        
        /** execute event loop */
        ProcessCode processEventLoop(size_t nEvents, size_t skipEvents=0);

        /** finalize event loop */
        ProcessCode finalizeEventLoop();
        
        /** Framework name() method */
        const std::string& name() const;
        
        /** return the MessageLevel */
        MessageLevel messageLevel() const;

      private:
        Config  m_cfg; //!< configuration object
        
    };
    
    inline ProcessCode Sequencer::addIOAlgorithms(std::vector< std::shared_ptr<IOAlgorithm> > ioalgs)
    {
        for (auto& ioalg : ioalgs){
            if (!ioalg){
                MSG_FATAL("Trying to add empty IO Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            MSG_INFO("Sucessfully added IO Algorithm " << ioalg->name() << " to Seqeuencer.");
            m_cfg.ioAlgorithms.push_back(std::move(ioalg));
        }
        return ProcessCode::SUCCESS;
    }

    inline ProcessCode Sequencer::prependEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs)
    {
        for (auto& ialg: ialgs){
            if (!ialg){
                MSG_FATAL("Trying to prepend empty Event Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            MSG_INFO("Sucessfully prepended Event Algorithm " << ialg->name() << " to Seqeuencer.");
            m_cfg.eventAlgorithms.insert(m_cfg.eventAlgorithms.begin(),std::move(ialg));
        }
        return ProcessCode::SUCCESS;
    }
    
    inline ProcessCode Sequencer::appendEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs)
    {
        for (auto& ialg: ialgs){
            if (!ialg){
                MSG_FATAL("Trying to append empty Event Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            m_cfg.eventAlgorithms.push_back(ialg);
            MSG_INFO("Sucessfully appended Event Algorithm " << ialg->name() << " to Seqeuencer.");
        }
        return ProcessCode::SUCCESS;
    }

    inline const std::string& Sequencer::name() const { return m_cfg.name; }
    
    inline MessageLevel Sequencer::messageLevel() const { return m_cfg.msgLvl; }
    
}


#endif /* AlgorithmSequencer_h */
