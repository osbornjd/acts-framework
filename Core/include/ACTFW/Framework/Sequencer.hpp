//
//  AlgorithmSequencer.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTF_FRAMEWORK_SEQUENCER_H 
#define ACTF_FRAMEWORK_SEQUENCER_H 1

#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/IService.hpp"
#include <memory>
#include <vector>
#include <string>

namespace FW {
    
    class WhiteBoard;
    
    /// @class  Sequencer
    ///    
    /// This is the backbone of the mini framework, it initializes all algorithms,
    /// calls execute per event and deals with the event store */
    /// 
    class Sequencer {
        
      public:
        /// @class Config 
        ///
        /// Nested, public configuration class for the algorithm sequences
        class Config {
          public:
            std::shared_ptr<Acts::Logger>               logger;          ///< default logging instance
            std::vector< std::shared_ptr<IService> >    services;        ///< the serivces
            std::vector< std::shared_ptr<IOAlgorithm> > ioAlgorithms;    ///< i/o algorithms
            std::vector< std::shared_ptr<IAlgorithm> >  eventAlgorithms; ///< algorithms
            std::shared_ptr<WhiteBoard>                 eventBoard;      ///< event board
            std::shared_ptr<WhiteBoard>                 jobBoard;        ///< the job board
            std::string                                 name;            ///< the name of this sequencer
            
            Config(const std::string& lname = "Sequencer", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
             : logger(Acts::getDefaultLogger(lname,lvl))
             , ioAlgorithms()
             , eventAlgorithms()
             , eventBoard(nullptr)
             , jobBoard(nullptr)
             , name(lname)
            {}
            
        };
        
        /// Constructor
        ///
        /// @param cfg is the configuration object
        Sequencer(const Config& cfg);
        
        /// Destructor
        ~Sequencer();

        /// Add the algorithms - for Services
        ///
        /// @param iservices is the vector of services to be added
        ProcessCode addServices(std::vector< std::shared_ptr<IService> > iservices);

        /// Add the algorithms - for IO
        ///
        /// @param ioalgs is the vector of I/O algorithms to be added
        ProcessCode addIOAlgorithms(std::vector< std::shared_ptr<IOAlgorithm> > ioalgs);

        /// Prepend algorithms 
        ///
        /// @param ialgs is the vector of algorithms to be prepended
        ProcessCode prependEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs);

        /// Append algorithms 
        ///
        /// @param ialgs is the vector of algorithms to be appended
        ProcessCode appendEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs);

        /// Event loop initialization method
        ProcessCode initializeEventLoop();
        
        /// Event loop process method
        ProcessCode processEventLoop(size_t nEvents, size_t skipEvents=0);

        /// Event loop finalization method
        ProcessCode finalizeEventLoop();
        
        /// Framework name() method 
        const std::string& name() const;

      private:
        Config          m_cfg; ///< configuration object
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
          return *m_cfg.logger;
        }        
        
    };
    
    inline ProcessCode Sequencer::addServices(std::vector< std::shared_ptr<IService> > iservices)
    {
        for (auto& isvc : iservices){
            if (!isvc){
                ACTS_FATAL("Trying to add empty Service to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            ACTS_INFO("Sucessfully added IO Algorithm " << isvc->name() << " to Seqeuencer.");
            m_cfg.services.push_back(std::move(isvc));
        }
        return ProcessCode::SUCCESS;
    }
    
    inline ProcessCode Sequencer::addIOAlgorithms(std::vector< std::shared_ptr<IOAlgorithm> > ioalgs)
    {
        for (auto& ioalg : ioalgs){
            if (!ioalg){
                ACTS_FATAL("Trying to add empty IO Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            ACTS_INFO("Sucessfully added IO Algorithm " << ioalg->name() << " to Seqeuencer.");
            m_cfg.ioAlgorithms.push_back(std::move(ioalg));
        }
        return ProcessCode::SUCCESS;
    }

    inline ProcessCode Sequencer::prependEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs)
    {
        for (auto& ialg: ialgs){
            if (!ialg){
                ACTS_FATAL("Trying to prepend empty Event Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            ACTS_INFO("Sucessfully prepended Event Algorithm " << ialg->name() << " to Seqeuencer.");
            m_cfg.eventAlgorithms.insert(m_cfg.eventAlgorithms.begin(),std::move(ialg));
        }
        return ProcessCode::SUCCESS;
    }
    
    inline ProcessCode Sequencer::appendEventAlgorithms(std::vector< std::shared_ptr<IAlgorithm> > ialgs)
    {
        for (auto& ialg: ialgs){
            if (!ialg){
                ACTS_FATAL("Trying to append empty Event Algorithm to Sequencer. Abort.");
                return ProcessCode::ABORT;
            }
            m_cfg.eventAlgorithms.push_back(ialg);
            ACTS_INFO("Sucessfully appended Event Algorithm " << ialg->name() << " to Seqeuencer.");
        }
        return ProcessCode::SUCCESS;
    }

    inline const std::string& Sequencer::name() const { return m_cfg.name; }
        
}

#endif // ACTF_FRAMEWORK_SEQUENCER_H
