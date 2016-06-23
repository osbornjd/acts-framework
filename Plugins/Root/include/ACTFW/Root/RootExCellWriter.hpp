//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_PLUGINS_ROOTEXCELLWRITER_H
#define ACTFW_PLUGINS_ROOTEXCELLWRITER_H 1

#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "TTree.h"

class TFile;

#ifndef MAXSTEPS
#define MAXSTEPS 100
#endif

namespace FWRoot {
    
    /// @class ExtrapolatiionCellWriter
    /// 
    /// a root based implementation to write out extrapolation steps
    /// 
    class RootExCellWriter : public FW::IExtrapolationCellWriter {
    public:
        ///  @struct ExtrapolationStep 
        ///  this holds the information to be written out
        struct ExtrapolationStep {
            float x, y, z;          ///< position (global)
            float px, py, pz;       ///< momentum
            float type;             ///< type of the step
        };
        
        
        // @class Config
        // 
        // The nested config class
        class Config {
        public:
          std::shared_ptr<Acts::Logger>   logger;     ///< the default logger 
            std::string                   treeName;   ///< the name of the output tree
            std::string                   fileName;   ///< the name of the output file
            std::string                   name;       ///< the name of the algorithm
            
            Config(const std::string& lname = "Algorithm", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
             : logger(Acts::getDefaultLogger(lname,lvl))
             , treeName("TTree")
             , fileName("TFile.root")         
             , name(lname)
            {}
        };
        
        /// Constructor
        ///
        /// @param cfg is the configuration class 
        RootExCellWriter(const Config& cfg);
        
        /// Destructor 
        virtual ~RootExCellWriter();
        
        /// Framework intialize method 
        FW::ProcessCode initialize() final;
        
        /// Framework finalize mehtod 
        FW::ProcessCode finalize() final;

        /// The write interface 
        ///
        /// @param eCell is the extrapolation cell that is parsed and written
        FW::ProcessCode write(const Acts::ExCellCharged& eCell) final;
        
        /// The write interface 
        /// 
        /// @param eCell is the extrapolation cell that is parsed and written
        FW::ProcessCode write(const Acts::ExCellNeutral& eCell) final;

        /// Framework name() method 
        const std::string& name() const final;

    private:
        Config                      m_cfg;                  //!< the config class
        TFile*                      m_outputFile;           //!< the output file name
        TTree*                      m_outputTree;           //!< the output tree name
        std::vector<float>          m_positionX;
        std::vector<float>          m_positionY;
        std::vector<float>          m_positionZ;

        /// Private helper method for actual filling
        template <class T> FW::ProcessCode writeT(const Acts::ExtrapolationCell<T>& eCell);

        /// Private access to the logging instance
        const Acts::Logger&
        logger() const
        {
          return *m_cfg.logger;
        }   
        
    };
    
    const std::string& RootExCellWriter::name() const { return m_cfg.name; }
 
#include "RootExCellWriter.ipp"
    
}


#endif // ACTFW_PLUGINS_ROOTEXCELLWRITER_H