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

namespace FW {

namespace Obj {

/// @class ExtrapolationCellWriter
///
/// A obj based implementation to write out extrapolation steps
/// it is used to write obj representations of tracks for displaying
///
/// Safe to use from multiple writer threads.
///
template <class T> class ObjExCellWriter
  : public WriterT<std::vector<Acts::ExtrapolationCell<T>>
{
public:

  // The nested configuration struct
  struct Config
  {
  public:
    /// output scalor
    double                              outputScalor      = 1.;
    /// precision for out
    size_t                              outputPrecision   = 4;
    /// the output stream
    std::shared_ptr<std::ofstream>      outputStream      = nullptr;
  };

  /// Constructor
  ///
  /// @param cfg is the configuration object
  /// @parm level is the output logging level
  ObjExCellWriter(const Config& cfg,
    Acts::Logging::Level lvl   = Acts::Logging::INFO);
  
protected:
  /// The protected writeT method, called by the WriterT base
  /// @param ctx is the algorithm context for event consistency   
  ProcessCode
  writeT(const FW::AlgorithmContext&                  ctx,
         const std::vector<Acts::ExtrapolationCell> & ecells) final;

private:
  Config m_cfg;
  
};

template <class T>
ObjExCellWriter<T>::ObjExCellWriter(
    const ObjExCellWriter<T>::Config& cfg)
  : FW::IWriterT<Acts::ExtrapolationCell<T> >()
  , m_cfg(cfg)
  , m_vCounter(0)
{
  if (!m_cfg.outputStream) {
    throw std::invalid_argument("Missing output stream");
  }
}


template <class T>
FW::ProcessCode
ObjExCellWriter<T>::write(const FW::AlgorithmContext&                  ctx,
                          const std::vector< const Acts::ExtrapolationCell<T> >& ecells)
{
  // loop over the cells
  for (auto& eCell : ecells)
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
  }
  // return success
  return FW::ProcessCode::SUCCESS;
}

} // namespace Obj
} // namespace FW

#endif  // ACTFW_PLUGINS_OBJEXCELLWRITER_H
