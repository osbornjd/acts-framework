//
//  WhiteBoard.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef WhiteBoard_h
#define WhiteBoard_h

#include <map>
#include <memory>
#include <vector>
#include <string>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"


namespace FW {

    /**@class WhiteBoard 
     */
    class WhiteBoard {
        
      public:
        /** @class Config 
         Nested Config class for this WhiteBoard */
        class Config {
          public:
            std::string     name;   //!< the name of this WhiteBoard
            MessageLevel    msgLvl; //!< the messate level
            
            Config() :
             name("Anonymuous"),
             msgLvl(MessageLevel::INFO)
            {}
        };
    
        /** Constructor*/
        WhiteBoard(const Config& cfg);
        
        /** Destructor */
        virtual ~WhiteBoard();
        
        /** Framework name() method */
        const std::string& name() const;
        
        /** return the MessageLevel */
        MessageLevel messageLevel() const;
        
        /** clear the white board */
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
        
        /** write to the white board */
        template <class T> ProcessCode writeT(std::vector< std::unique_ptr<T> >* coll, const std::string& cname)
        {
            // clear the entry in the event store
            if (clearT<T>(cname) != ProcessCode::SUCCESS)return ProcessCode::ABORT;
            // record the new one
            if (coll == nullptr){
                MSG_FATAL("Could not write collection " << cname);
                return ProcessCode::ABORT;
            }
            MSG_VERBOSE("Writing collection " << cname << " to board");
            m_store[cname] = (void*)coll;
            return ProcessCode::SUCCESS;
        
        }

        /** read from the white board */
        template <class T> ProcessCode readT(std::vector< std::unique_ptr<T> >*& coll, const std::string& cname)
        {
            auto sCol = m_store.find(cname);
            if (sCol == m_store.end()){
                MSG_FATAL("Could not read collection " << cname);
                return ProcessCode::ABORT;
            }
            // now do the static_cast
            coll = reinterpret_cast< std::vector< std::unique_ptr<T> >*  >(sCol->second);
            MSG_VERBOSE("Reading collection " << cname << " from board");
            return ProcessCode::SUCCESS;
        }
        
      private:
        Config                 m_cfg; //!< the name of this WhiteBoard
        
        std::map<std::string, void*> m_store;
  
    };    
    
    inline const std::string& WhiteBoard::name() const { return m_cfg.name; }
    
    inline MessageLevel WhiteBoard::messageLevel() const { return m_cfg.msgLvl; }
    
}


#endif /* WhiteBoard_h */
