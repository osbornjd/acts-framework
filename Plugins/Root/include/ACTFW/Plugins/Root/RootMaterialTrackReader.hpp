///////////////////////////////////////////////////////////////////
// RootMaterialStepReader.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_ROOT_RootMaterialTrackReader_H
#define ACTFW_PLUGINS_ROOT_RootMaterialTrackReader_H

#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TChain;

namespace FW {
  
namespace Root {

/// @class RootMaterialTrackReader
///
/// @brief Reads in MaterialTrack entities from a root file
///
/// This service is the root implementation of the ImaterialTrackReader.
/// It reads in a vector of MaterialTrack entities from a given root tree
/// of a given root file. The input file and tree are set over the configuration
/// object.
class RootMaterialTrackReader : public FW::IReaderT<Acts::MaterialTrack>
{
public:
  /// @class Config
  /// Configuration of the Reader
  class Config
  {
  public:
    /// The name of the input tree
    std::string treeName;
    /// The name of the input file
    std::vector<std::string> fileList;
    /// The default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// The name of the service
    std::string name;

    Config(const std::string&   lname = "MaterialReader",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
    {
    }
  };

  /// Constructor
  RootMaterialTrackReader(const Config& cfg);

  /// Destructor
  virtual ~RootMaterialTrackReader() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize method
  FW::ProcessCode
  finalize() override final;

  /// Framework name() method
  std::string
  name() const override final;

  // clang-format off
  /// @copydoc FW::IReaderT::read(std::vector<Acts::ParticleProperties>&,size_t,const FW::AlgorithmContext*)
  // clang-format on
  FW::ProcessCode
  read(Acts::MaterialTrack&        mtrc,
       size_t                      skip    = 0,
       const FW::AlgorithmContext* context = nullptr) override final;

private:
  /// The config class
  Config m_cfg;
  /// mutex used to protect multi-threaded reads
  std::mutex m_read_mutex;
  /// The input tree name
  TChain* m_inputChain;
  // The MaterialTrack to be written out
  Acts::MaterialTrack* m_trackRecord;
  /// the event
  int m_event;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

inline std::string
RootMaterialTrackReader::name() const
{
  return m_cfg.name;
}

}  // namespace Root
}  // namespace FW

#endif  // ACTFW_PLUGINS_ROOT_RootMaterialTrackReader_H
