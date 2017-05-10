//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_PLUGINS_OBJEXCELLWRITER_H
#define ACTFW_PLUGINS_OBJEXCELLWRITER_H

#include <mutex>

#include <TTree.h>

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWObj {

/// @class ExtrapolationCellWriter
///
/// A root based implementation to write out extrapolation steps.
///
/// Safe to use from multiple writer threads.
///
class ObjExCellWriter : public FW::IExtrapolationCellWriter
{
public:

  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;                      ///< the default logger
    std::string                    name;                        ///< the name of the algorithm
    double                         outputScalor      = 1.;      ///< output scalor
    size_t                         outputPrecision   = 4;       ///< precision for out
    std::shared_ptr<std::ofstream> outputStream      = nullptr; ///< the output stream

    Config(const std::string&   lname = "ObjExCellWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {
    }
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  ObjExCellWriter(const Config& cfg);

  /// Destructor
  virtual ~ObjExCellWriter() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  ///
  /// @param eCell is the extrapolation cell that is parsed and written
  FW::ProcessCode
  write(const Acts::ExCellCharged& eCell) final;

  /// The write interface
  ///
  /// @param eCell is the extrapolation cell that is parsed and written
  FW::ProcessCode
  write(const Acts::ExCellNeutral& eCell) final;

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config             m_cfg;               ///< the config class
  size_t             m_vCounter;          ///< the vertex counter

  std::mutex         m_write_mutex;       ///< mutex used to protect multi-threaded writes
    
  /// Private helper method for actual filling
  template <class T>
  FW::ProcessCode
  writeT(const Acts::ExtrapolationCell<T>& eCell);

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};


inline ObjExCellWriter::ObjExCellWriter(
    const ObjExCellWriter::Config& cfg)
  : FW::IExtrapolationCellWriter()
  , m_cfg(cfg)
  , m_vCounter(0)    
{}

inline FW::ProcessCode
ObjExCellWriter::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

inline FW::ProcessCode
ObjExCellWriter::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

const std::string&
ObjExCellWriter::name() const
{
  return m_cfg.name;
}

inline FW::ProcessCode
ObjExCellWriter::write(const Acts::ExCellCharged& eCell)
{
  return writeT<Acts::TrackParameters>(eCell);
}

inline FW::ProcessCode
ObjExCellWriter::write(const Acts::ExCellNeutral& eCell)
{
  return writeT<Acts::NeutralParameters>(eCell);
}


template <class T>
FW::ProcessCode
ObjExCellWriter::writeT(const Acts::ExtrapolationCell<T>& eCell)
{
  std::lock_guard<std::mutex> lock(m_write_mutex);
  // remember the first counter
  size_t fCounter = m_vCounter;
  // increase the vertex counter
  ++m_vCounter;
  // the event paramters
  auto sPosition = eCell.startParameters.position();
  // write the space point
  (*(m_cfg.outputStream)) << "v " << m_cfg.outputScalor*sPosition.x() 
                          << ", " << m_cfg.outputScalor*sPosition.y() 
                          << ", " << m_cfg.outputScalor*sPosition.z() << '\n';
  
  // loop over extrapolation steps
  for (auto& es : eCell.extrapolationSteps) {
    if (es.parameters) {      
      /// step parameters
      const T& pars  = (*es.parameters);
      auto tPosition = pars.position();
      // increase the counter
      ++m_vCounter; 
      // write the space point
      (*(m_cfg.outputStream)) << "v " << m_cfg.outputScalor*tPosition.x() 
                              << ", " << m_cfg.outputScalor*tPosition.y() 
                              << ", " << m_cfg.outputScalor*tPosition.z() << '\n';
    }
  }
  // write out the line
  (*(m_cfg.outputStream)) << "l ";
  for (size_t iv = fCounter; iv < m_vCounter; ++iv)
    (*(m_cfg.outputStream)) << iv << " ";
  (*(m_cfg.outputStream)) << '\n';
  (*(m_cfg.outputStream)) << '\n';

  // return success 
  return FW::ProcessCode::SUCCESS;
}


} // end of namespace

#endif  // ACTFW_PLUGINS_OBJEXCELLWRITER_H
