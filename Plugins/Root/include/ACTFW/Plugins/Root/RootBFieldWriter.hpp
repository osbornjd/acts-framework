///////////////////////////////////////////////////////////////////
// RootBFieldWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_ROOT_ROOTBFIELDWRITER_H
#define ACTFW_PLUGINS_ROOT_ROOTBFIELDWRITER_H

#include <mutex>
#include "ACTFW/Framework/IWriter.hpp"
#include "TTree.h"

namespace FWRoot {

/// @class RootBFieldWriter
///
/// @brief

class RootBFieldWriter : public FW::IWriter
{
public:
  /// @class Config
  /// Configuration of the Writer
  class Config
  {
  public:
    /// The name of the writer
    std::string name;
    /// The default logger
    std::shared_ptr<const Acts::Logger> logger;
    // The bField to be written out
    Acts::InterpolatedBFieldMap bField;
    /// The name of the output file
    std::string fileName;
    /// The name of the output tree
    std::string treeName;

    Config(const std::string&   lname = "BFieldWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
    {
    }
  };

  /// Constructor
  RootBFieldWriter(const Config& cfg);

  /// Destructor
  virtual ~RootBFieldWriter() = default;

  /// Framework name() method
  std::string
  name() const override final;

  /// Framework intialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize method
  FW::ProcessCode
  finalize() override final;

  /// Interface method which writes out the MagneticFieldmap
  FW::ProcessCode
  write(const AlgorithmContext& context) override final;

private:
  /// The config class
  Config m_cfg;
  /// mutex used to protect multi-threaded writes
  std::mutex m_write_mutex;
  /// The output file name
  TFile* m_outputFile;
  /// The output tree name
  TTree* m_outputTree;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

inline std::string
RootBFieldWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_PLUGINS_ROOT_ROOTBFIELDWRITER_H
