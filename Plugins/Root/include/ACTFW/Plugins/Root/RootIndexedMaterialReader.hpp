///////////////////////////////////////////////////////////////////
// RootMaterialStepReader.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_ROOT_INDEXEDMATERIALREADER_H
#define ACTFW_PLUGINS_ROOT_INDEXEDMATERIALREADER_H

#include <mutex>
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Material/SurfaceMaterial.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWRoot {
    
    /// @class RootMaterialTrackReader
    ///
    /// @brief Reads in MaterialTrack entities from a root file
    ///
    /// This service is the root implementation of the ImaterialTrackReader.
    /// It reads in a vector of MaterialTrack entities from a given root tree
    /// of a given root file. The input file and tree are set over the configuration object.
  class RootIndexedMaterialReader : public FW::IReaderT<Acts::IndexedSurfaceMaterial> {
        
    public:
        
        /// @class Config
        /// Configuration of the Reader
        class Config {
        public:
            /// The name of the input tree
            std::string                   folderNameBase = "Material";
            /// The name of the input file
            std::string                   fileName;
            /// The default logger
            std::shared_ptr<const Acts::Logger> logger;
            /// The name of the service
            std::string                   name;
            
            Config(const std::string& lname = "MaterialReader",
                   Acts::Logging::Level lvl = Acts::Logging::INFO) :
            logger(Acts::getDefaultLogger(lname,lvl)),
            name(lname)
            {}
        };
        
        /// Constructor
        RootIndexedMaterialReader(const Config& cfg);
        
        /// Destructor
        virtual ~RootIndexedMaterialReader() = default;
        
        /// Framework intialize method
        FW::ProcessCode initialize() override final;
        
        /// Framework finalize method
        FW::ProcessCode finalize() override final;
        
        /// Framework name() method
        const std::string& name() const override final;
        
        /// Interface method returning the MaterialTrack 
        /// entities which have been read in
        /// @param mtrc is the material track record vector
        FW::ProcessCode 
        read(Acts::IndexedSurfaceMaterial& mtrc, size_t skip = 0) override final;
        
    private:
        /// The config class
        Config                                                      m_cfg;
        
        /// The input file
        TFile*                                                      m_inputFile;
        
        /// mutex used to protect multi-threaded reads
        std::mutex                                                  m_read_mutex;     

        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
            return *m_cfg.logger;
        }
        
    };
    
    inline const std::string& RootIndexedMaterialReader::name() const { return m_cfg.name; }

}

#endif // ACTFW_PLUGINS_ROOT_INDEXEDMATERIALREADER_H
