///////////////////////////////////////////////////////////////////
// MaterialMapping.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H
#define ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H

#include <memory>
#include <climits>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Material/SurfaceMaterial.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
  class TrackingGeometry;
}

namespace FWA {

/// @class MaterialMapping
///
/// @brief Initiates material mapping
///
/// The MaterialMapping reads in the MaterialTrack with a dedicated
/// reader and uses the material mapper to project the material onto
/// the tracking geometry 
///
/// In a final step, the material maps are written out for further usage
  
class MaterialMapping : public FW::Algorithm
{
public:
  /// @class nested Config class 
  /// of the MaterialMapping algorithm
  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The reader to read in the MaterialTrack entities
    std::shared_ptr< FW::IReaderT<Acts::MaterialTrack > >  
        materialTrackReader;
    /// The ACTS material mapper
    std::shared_ptr<Acts::MaterialMapper>  
        materialMapper;
    /// The validation writer of the material 
    std::shared_ptr< FW::IWriterT<Acts::MaterialTrack> >
      materialTrackWriter;
    /// The writer of the material
    std::shared_ptr<FW::IWriterT<Acts::IndexedSurfaceMaterial> > 
        indexedMaterialWriter;
    /// The TrackingGeometry to be mapped on
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry;
    /// mapping conditions
    size_t                                        maximumTrackRecords;  

    Config()
      : FW::Algorithm::Config("MaterialMapping")
      , materialTrackReader(nullptr)
      , materialMapper(nullptr)
      , materialTrackWriter(nullptr) 
      , indexedMaterialWriter(nullptr)
      , trackingGeometry(nullptr)
      , maximumTrackRecords(std::numeric_limits<size_t>::infinity())    
    {}
  };

  /// Constructor
  MaterialMapping(const Config&                       cfg,
                  std::unique_ptr<const Acts::Logger> logger
                           = Acts::getDefaultLogger("MaterialMapping",
                             Acts::Logging::INFO));

  /// Destructor
  ~MaterialMapping();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) 
  override final;

  /// Framework execute method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const 
  override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

private:
  /// The config object
  Config                        m_cfg;

};

}

#endif // ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H
