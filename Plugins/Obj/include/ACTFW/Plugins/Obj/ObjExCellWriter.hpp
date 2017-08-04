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
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWObj {

/// @class ExtrapolationCellWriter
///
/// A root based implementation to write out extrapolation steps.
///
/// Safe to use from multiple writer threads.
///
template <class T> class ObjExCellWriter
  : public FW::IWriterT<Acts::ExtrapolationCell<T> >
{
public:

  // @class Config
  //
  // The nested configuration class
  class Config
  {
  public:
    /// the default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// the name of the algorithm
    std::string                         name;
    /// output scalor
    double                              outputScalor      = 1.;
    /// precision for out
    size_t                              outputPrecision   = 4;
    /// the output stream
    std::shared_ptr<std::ofstream>      outputStream      = nullptr;

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
  
  /// Framework name() method
  std::string
  name() const final;

  /// Framework intialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

  /// The write interface
  /// @param eCell is the extrapolation cell that is parsed and written
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const Acts::ExtrapolationCell<T>& eCell) override final;

  /// write a bit of string
  /// @param sinfo is some string info to be written
  /// @return is a ProcessCode indicating return/failure
  FW::ProcessCode
  write(const std::string& sinfo) override final;

private:
  Config             m_cfg;               ///< the config class
  size_t             m_vCounter;          ///< the vertex counter
  std::mutex         m_write_mutex;       ///< mutex used to protect multi-threaded writes

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

template <class T>
ObjExCellWriter<T>::ObjExCellWriter(
    const ObjExCellWriter<T>::Config& cfg)
  : FW::IWriterT<Acts::ExtrapolationCell<T> >()
  , m_cfg(cfg)
  , m_vCounter(0)
{}

template <class T> FW::ProcessCode
ObjExCellWriter<T>::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
ObjExCellWriter<T>::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T>
std::string
ObjExCellWriter<T>::name() const
{
  return m_cfg.name;
}

template <class T>
FW::ProcessCode
ObjExCellWriter<T>::write(const Acts::ExtrapolationCell<T>& eCell)
{
  
   // abort if you don't have a stream
  if (!m_cfg.outputStream)   return FW::ProcessCode::ABORT;
  // lock the mutex for writing
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
  // new line
  (*(m_cfg.outputStream)) << '\n';
  // return success
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
ObjExCellWriter<T>::write(const std::string& sinfo)
{
  // abort if you don't have a stream
  if (!m_cfg.outputStream)   return FW::ProcessCode::ABORT;
  // lock the mutex for writing
  std::lock_guard<std::mutex> lock(m_write_mutex);
  
  (*(m_cfg.outputStream)) << sinfo;
  return FW::ProcessCode::SUCCESS;
}

} // end of namespace

#endif  // ACTFW_PLUGINS_OBJEXCELLWRITER_H
