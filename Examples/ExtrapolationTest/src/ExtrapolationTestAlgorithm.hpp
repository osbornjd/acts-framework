//
//  ExtrapolationTestAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_EXTRAPOLATIONTESTALGORITHM_H
#define ACTFW_EXAMPLES_EXTRAPOLATIONTESTALGORITHM_H 1

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include <memory>

namespace Acts {
    class IExtrapolationEngine;
}

namespace FW {
    class WhiteBoard;
    class IRandomNumbers;
    class IEExtrapolationCellWriter;
}

namespace FWE {

    /// @class Algorithm 
    class ExtrapolationTestAlgorithm : public FW::Algorithm {
        
      public :
        /// @class Config 
        class Config : public FW::Algorithm::Config {
        public:
            std::shared_ptr<FW::RandomNumbers>            randomNumbers;       ///< FW random number service
            std::shared_ptr<Acts::IExtrapolationEngine>   extrapolationEngine; ///< the extrapolation engine
            std::shared_ptr<FW::IExtrapolationCellWriter> extrapolationCellWriter; ///< output writer
            size_t                                        testsPerEvent;       ///< number of tests per event
            int                                           parameterType;       ///< parameter type : 0 = neutral | 1 = charged
            std::array<double,2>                          d0Defs;              ///< mean, sigma for d0 range
            std::array<double,2>                          z0Defs;              ///< mean, sigma for z0 range
            std::array<double,2>                          etaRange;            ///< low, high for eta range
            std::array<double,2>                          phiRange;            ///< low, high for phi range
            std::array<double,2>                          ptRange;             ///< low, high for pt range
            bool                                          particleType;        ///< particle type definition
            bool                                          collectSensitive;    ///< configuration: sensitive collection
            bool                                          collectPassive;      ///< configuration: collect passive
            bool                                          collectBoundary;     ///< configuration: collect boundary
            bool                                          collectMaterial;     ///< configuration: collect material
            bool                                          sensitiveCurvilinear;///< configuration: don't collapse
            int                                           searchMode;          ///< define how robust the search mode is
            double                                        pathLimit;           ///< set the patch limit of the extrapolation

            Config(const std::string& lname = "Algorithm", 
                   Acts::Logging::Level lvl = Acts::Logging::INFO)
          : Algorithm::Config(lname,lvl),
            randomNumbers(nullptr),
            extrapolationEngine(nullptr),
            extrapolationCellWriter(nullptr),
            testsPerEvent(1),
            parameterType(1),
            d0Defs({{0.,2.}}),
            z0Defs({{0.,50.}}),
            etaRange({{-3.,3.}}),
            phiRange({{-M_PI,M_PI}}),
            ptRange({{100.,100000.}}),
            particleType(3),
            collectSensitive(true),
            collectPassive(true),
            collectBoundary(true),
            collectMaterial(true),
            sensitiveCurvilinear(false),
            searchMode(0),
            pathLimit(-1.)
            {}
            
        };
        
        /// Constructor
        ExtrapolationTestAlgorithm(const Config& cnf);
        
        /// Destructor
        ~ExtrapolationTestAlgorithm();
        
        /// Framework intialize method 
        FW::ProcessCode initialize(std::shared_ptr<FW::WhiteBoard> eventStore = nullptr,
                                   std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;
        
        /// Framework execode method 
        FW::ProcessCode execute(size_t eventNumber) final;
        
        /// Framework finalize mehtod 
        FW::ProcessCode finalize() final;

    private:
        Config m_cfg; //!< the config class
        
        double drawGauss(const std::array<double,2>& range) const;
        double drawUniform(const std::array<double,2>& range) const;
        
        template <class T> FW::ProcessCode executeTestT(const T& startParameters);
        
    };

#include "ExtrapolationTestAlgorithm.ipp"
    
}

#endif