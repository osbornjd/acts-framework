//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef RootExCellWriter_h
#define RootExCellWriter_h

#include "ACTS/Extrapolation/ExtrapolationCell.hpp"

#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"

#include "TTree.h"

class TFile;

#ifndef MAXSTEPS
#define MAXSTEPS 100
#endif

/** namespace Framework Root Plugins */
namespace FWRoot {
    
    /** @class IExtrapolatiionCellWriter
     Interface class for extrapolation cell writers*/
    class RootExCellWriter : public FW::IExtrapolationCellWriter {
    public:
        /** @struct */
        struct ExtrapolationStep {
            float x, y, z;          // position (global)
            float px, py, pz;       // momentum
            float type;             // type of the step
        };
        
        
        /** @class Config
         nested config file */
        class Config {
        public:
            std::string                 treeName;           //!< the name of the output tree
            std::string                 fileName;           //!< the name of the output file
            std::string                 name;               //!< the name of the algorithm
            FW::MessageLevel            msgLvl;             //!< the message level of this algorithm
            
            Config() :
            name("Anonymous"),
            msgLvl(FW::MessageLevel::INFO)
            {}
        };
        
        /** Constructor */
        RootExCellWriter(const Config& cfg);
        
        /** Destructor */
        virtual ~RootExCellWriter();
        
        /** Framework intialize method */
        FW::ProcessCode initialize() final;
        
        /** Framework finalize mehtod */
        FW::ProcessCode finalize() final;

        /** The write interface */
        FW::ProcessCode write(const Acts::ExCellCharged& eCell) final;
        
        /** The write interface */
        FW::ProcessCode write(const Acts::ExCellNeutral& eCell) final;

        /** Framework name() method */
        const std::string& name() const final;
        
        /** return the MessageLevel */
        FW::MessageLevel messageLevel()const final;

    private:
        Config                      m_cfg;                  //!< the config class
        TFile*                      m_outputFile;           //!< the output file name
        TTree*                      m_outputTree;           //!< the output tree name
        std::vector<float>          m_positionX;
        std::vector<float>          m_positionY;
        std::vector<float>          m_positionZ;
        
        
        template <class T> FW::ProcessCode writeT(const Acts::ExtrapolationCell<T>& eCell);
        
    };
    
    const std::string& RootExCellWriter::name() const { return m_cfg.name; }
    
    FW::MessageLevel RootExCellWriter::messageLevel() const { return m_cfg.msgLvl; }
 
#include "RootExCellWriter.ipp"
    
}


#endif /* RootExCellWriter_h */