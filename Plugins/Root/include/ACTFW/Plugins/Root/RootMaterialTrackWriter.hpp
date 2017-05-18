///////////////////////////////////////////////////////////////////
// RootMaterialTrackWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_ROOT_RootMaterialTrackWriter_H
#define ACTFW_PLUGINS_ROOT_RootMaterialTrackWriter_H

#include <mutex>
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "TTree.h"


namespace FWRoot {
    
    /// @class RootMaterialTrackWriter
    ///
    /// @brief Writes out MaterialTrack entities from a root file
    ///
    /// This service is the root implementation of the IWriterT.
    /// It writes out a MaterialTrack which is usually generated from
    /// Geant4 material mapping
    
    class RootMaterialTrackWriter : public FW::IWriterT<Acts::MaterialTrack> {
    
    public:
        
        /// @class Config
        /// Configuration of the Writer
        class Config {
        public:
            /// The name of the output tree
            std::string                 treeName;
            /// The name of the output file
            std::string                 fileName;
            /// The default logger
            std::shared_ptr<Acts::Logger>   logger;
            /// The name of the service
            std::string                   name;
            
            Config(const std::string& lname = "MaterialWriter",
                   Acts::Logging::Level lvl = Acts::Logging::INFO) :
            logger(Acts::getDefaultLogger(lname,lvl)),
            name(lname)
            {}
        };
        
        /// Constructor
        RootMaterialTrackWriter(const Config& cfg);
        
        /// Destructor
        virtual ~RootMaterialTrackWriter() = default;
        
        /// Framework intialize method
        FW::ProcessCode 
        initialize() override final;
        
        /// Framework finalize method
        FW::ProcessCode 
        finalize() override final;
        
        /// Interface method which writes out the MaterialTrack entities
        /// @param mtrecord is the material track record to be written out
        FW::ProcessCode 
        write(const Acts::MaterialTrack& mtrecord) override final;

        /// Interface method which writes out an additional string
        /// @param mtrecord is the material track record to be written out
        FW::ProcessCode 
        write(const std::string& sinfo) override final;
        
        /// Framework name() method
        const std::string& name() const override final;
        
    private:
        /// The config class
        Config                    m_cfg;
        /// mutex used to protect multi-threaded writes
        std::mutex                m_write_mutex;       
        /// The output file name
        TFile*                    m_outputFile;
        /// The output tree name
        TTree*                    m_outputTree;
        // The MaterialTrack to be written out
        Acts::MaterialTrack m_trackRecord;
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
            return *m_cfg.logger;
        }
    };
    
    inline const std::string& RootMaterialTrackWriter::name() const { return m_cfg.name; }


}

#endif // ACTFW_PLUGINS_ROOT_RootMaterialTrackWriter_H
