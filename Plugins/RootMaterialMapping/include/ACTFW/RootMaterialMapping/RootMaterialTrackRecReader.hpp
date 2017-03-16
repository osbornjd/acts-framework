///////////////////////////////////////////////////////////////////
// RootMaterialStepWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ROOT_ROOTMATERIALTRACKRECREADER_H
#define ROOT_ROOTMATERIALTRACKRECREADER_H


#include "ACTS/Utilities/Definitions.hpp"
#include "ACTFW/Writers/IMaterialTrackRecReader.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "TTree.h"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "ACTS/Utilities/Logger.hpp"


namespace FWRoot {
    
    /// @class RootMaterialTrackReader
    ///
    /// @brief Reads in MaterialTrackRecord entities from a root file
    ///
    /// This service is the root implementation of the IMaterialTrackRecReader.
    /// It reads in a vector of MaterialTrackRecord entities from a given root tree
    /// of a given root file. The input file and tree are set over the configuration object.
    
    class RootMaterialTrackRecReader : public FW::IMaterialTrackRecReader {
        
    public:
        
        /// @class Config
        /// Configuration of the Reader
        class Config {
        public:
            /// The name of the input tree
            std::string                 treeName;
            /// The name of the input file
            std::string                 fileName;
            /// The default logger
            std::shared_ptr<Acts::Logger>   logger;
            /// The name of the service
            std::string                   name;
            
            Config(const std::string& lname = "MaterialReader",
                   Acts::Logging::Level lvl = Acts::Logging::INFO) :
            logger(Acts::getDefaultLogger(lname,lvl)),
            name(lname)
            {}
        };
        
        /// Constructor
        RootMaterialTrackRecReader(const Config& cfg);
        
        /// Destructor
        virtual ~RootMaterialTrackRecReader() = default;
        
        /// Framework intialize method
        FW::ProcessCode initialize() final;
        
        /// Framework finalize method
        FW::ProcessCode finalize() final;
        
        /// Interface method reads in the MaterialTrackRecord entities
        FW::ProcessCode read() final;
        
        /// Framework name() method
        const std::string& name() const final;
        
        /// Interface method returning the MaterialTrackRecord entities which have been read in
        std::vector<Acts::MaterialTrackRecord> materialTrackRecords() final;
        
    private:
        /// The config class
        Config                                                      m_cfg;
        /// The input file name
        TFile*                                                      m_inputFile;
        /// The input tree name
        TTree*                                                      m_inputTree;
        /// The MaterialTrackRecord entities to be read in
        std::vector<Acts::MaterialTrackRecord>                      m_records;
        
        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
            return *m_cfg.logger;
        }
        
    };
    
    inline const std::string& RootMaterialTrackRecReader::name() const { return m_cfg.name; }
    
    inline std::vector<Acts::MaterialTrackRecord> RootMaterialTrackRecReader::materialTrackRecords()
    {
        if (m_records.empty()) read();
        return m_records;
    }
}


#endif //ROOT_ROOTMATERIALTRACKRECREADER_H