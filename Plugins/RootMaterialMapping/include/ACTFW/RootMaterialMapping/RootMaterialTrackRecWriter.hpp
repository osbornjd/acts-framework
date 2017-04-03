///////////////////////////////////////////////////////////////////
// RootMaterialTrackRecWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ROOT_ROOTMATERIALTRACKRECWRITER_H
#define ROOT_ROOTMATERIALTRACKRECWRITER_H 1

#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "ACTFW/Writers/IMaterialTrackRecWriter.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "TTree.h"
#include "ACTS/Utilities/Logger.hpp"


namespace FWRoot {
    
    /// @class RootMaterialTrackWriter
    ///
    /// @brief Writes out MaterialTrackRecord entities from a root file
    ///
    /// This service is the root implementation of the IMaterialTrackRecWriter.
    /// It writes out a vector of MaterialTrackRecord entities on a given root tree
    /// on a given root file. The output file and tree are set over the configuration object.
    
    class RootMaterialTrackRecWriter : public FW::IMaterialTrackRecWriter {
    
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
        RootMaterialTrackRecWriter(const Config& cfg);
        
        /// Destructor
        virtual ~RootMaterialTrackRecWriter() = default;
        
        /// Framework intialize method
        FW::ProcessCode initialize() final;
        
        /// Framework finalize method
        FW::ProcessCode finalize() final;
        
        /// Interface method which writes out the MaterialTrackRecord entities
        FW::ProcessCode write(const Acts::MaterialTrackRecord& mtrecord) final;
        
        /// Framework name() method
        const std::string& name() const final;
        
    private:
        /// The config class
        Config                                                      m_cfg;
        /// The output file name
        TFile*                                                      m_outputFile;
        /// The output tree name
        TTree*                                                      m_outputTree;
        // The MaterialTrackrecord to be written out
        Acts::MaterialTrackRecord                                   m_trackRecords;
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
            return *m_cfg.logger;
        }
    };
    
    inline const std::string& RootMaterialTrackRecWriter::name() const { return m_cfg.name; }

}


#endif //ROOT_ROOTMATERIALTRACKRECWRITER_H