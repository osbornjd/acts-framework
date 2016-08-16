//
//  WhiteBoard.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_FRAMEWORK_WHITEBOARD_h
#define ACTFW_FRAMEWORK_WHITEBOARD_h

#include <map>
#include <memory>
#include <vector>
#include <string>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"


namespace FW {

    /// @class WhiteBoard
    /// Simple whit board class to read from and write to
    class WhiteBoard {
        
      public:
        /// Constructor
        /// @param cfg is the config struct for this WhiteBoard
        WhiteBoard(std::unique_ptr<Acts::Logger> logger = Acts::getDefaultLogger("WhiteBoard", Acts::Logging::INFO));
        
        /// Destructor
        virtual ~WhiteBoard();

        /// clear the white board
        ///
        /// @param name is the collection name that should be cleared
        template <class T> ProcessCode clearT(const std::string& name)
        {
            // clear the event store again
            auto sCol = m_store.find(name);
            // return if nothing to do
            if (sCol == m_store.end()) return ProcessCode::SUCCESS;
            // static cast to the concrete type
            std::vector< std::unique_ptr<T> >* coll = reinterpret_cast< std::vector< std::unique_ptr<T> >* >(sCol->second);
            // erase from the map
            m_store.erase(sCol);
            // now delete the memory
            delete coll;
            // return success
            return ProcessCode::SUCCESS;
        }
        
        /// write to the white board
        ///
        /// @paramt coll is the collection to be written
        /// @param cname is the collection to name
        template <class T> ProcessCode writeT(std::vector< std::unique_ptr<T> >* coll, const std::string& cname)
        {
            // clear the entry in the event store
            if (clearT<T>(cname) != ProcessCode::SUCCESS)return ProcessCode::ABORT;
            // record the new one
            if (coll == nullptr){
                ACTS_FATAL("Could not write collection " << cname);
                return ProcessCode::ABORT;
            }
            ACTS_VERBOSE("Writing collection " << cname << " to board");
            m_store[cname] = (void*)coll;
            return ProcessCode::SUCCESS;
        
        }

        /// read from the white board
        ///
        /// @paramt coll is the collection to be written
        /// @param cname is the collection to name
        template <class T> ProcessCode readT(std::vector< std::unique_ptr<T> >*& coll, const std::string& cname)
        {
            auto sCol = m_store.find(cname);
            if (sCol == m_store.end()){
                ACTS_FATAL("Could not read collection " << cname);
                return ProcessCode::ABORT;
            }
            // now do the static_cast
            coll = reinterpret_cast< std::vector< std::unique_ptr<T> >*  >(sCol->second);
            ACTS_VERBOSE("Reading collection " << cname << " from board");
            return ProcessCode::SUCCESS;
        }
        
      private:
        std::unique_ptr<Acts::Logger> m_logger;
        /// the internal store
        std::map<std::string, void*> m_store;
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
          return *m_logger;
        }
  
    };
}


#endif // ACTFW_FRAMEWORK_WHITEBOARD_h
