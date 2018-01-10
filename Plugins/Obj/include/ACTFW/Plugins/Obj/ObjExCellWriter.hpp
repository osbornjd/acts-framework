// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_OBJEXCELLWRITER_H
#define ACTFW_PLUGINS_OBJEXCELLWRITER_H

#include <TTree.h>
#include <mutex>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

namespace Obj {

  /// @class ExtrapolationCellWriter
  ///
  /// An obj based implementation to write out extrapolation steps.
  ///
  /// Safe to use from multiple writer threads. (soon)
  ///
  template <class T>
  class ObjExCellWriter
      : public FW::WriterT<std::vector<Acts::ExtrapolationCell<T>>>
  {
  public:
    // The nested configuration struct
    struct Config
    {
    public:
      /// input collection
      std::string collection = "";

      /// output directory
      std::string outputDir = "";

      /// output scalor
      double outputScalor = 1.;

      /// precision for out
      size_t outputPrecision = 4;

      /// the size of the bezier segment
      double outputBezierSegment = 50 * Acts::units::_mm;
    };

    /// Constructor
    ///
    /// @param cfg is the configuration object
    /// @parm level is the output logging level
    ObjExCellWriter(const Config&        cfg,
                    Acts::Logging::Level level = Acts::Logging::INFO);

    /// End-of-run hook
    ProcessCode
    endRun() final override
    {
      return ProcessCode::SUCCESS;
    }

  protected:
    /// The protected writeT method, called by the WriterT base
    /// @param [in] ctx is the algorithm context for event consistency
    /// @param [in] ecells are the celss to be written out
    ProcessCode
    writeT(
        const FW::AlgorithmContext&                    ctx,
        const std::vector<Acts::ExtrapolationCell<T>>& ecells) final override;

    /// helper method for bezier line interpolation
    /// @param t is the step parameter along the line
    /// @param p0 anker point
    /// @param p1 is p0 + direction@p0
    /// @param p2 is p2 - direction@p2
    /// @param p3 is second anker poit
    /// @return the bezier point
    Acts::Vector3D
    calculateBezierPoint(double                t,
                         const Acts::Vector3D& p0,
                         const Acts::Vector3D& p1,
                         const Acts::Vector3D& p2,
                         const Acts::Vector3D& p3) const;

  private:
    Config     m_cfg;          ///< the configuration class of this writer
    size_t     m_vCounter;     ///< the vertex counter needed for Obj objects
    std::mutex m_write_mutex;  ///< mutex used to protect multi-threaded writes
  };

  template <class T>
  ObjExCellWriter<T>::ObjExCellWriter(const ObjExCellWriter<T>::Config& cfg,
                                      Acts::Logging::Level              level)
    : FW::WriterT<std::vector<Acts::ExtrapolationCell<T>>>(cfg.collection,
                                                           "ObjExCellWriter",
                                                           level)
    , m_cfg(cfg)
    , m_vCounter(0)
  {
    // Validate the configuration
    if (m_cfg.collection.empty()) {
      throw std::invalid_argument("Missing input collection");
    }
  }

  template <class T>
  FW::ProcessCode
  ObjExCellWriter<T>::writeT(
      const FW::AlgorithmContext&                    ctx,
      const std::vector<Acts::ExtrapolationCell<T>>& ecells)
  {

    // lock the mutex for writing
    std::lock_guard<std::mutex> lock(m_write_mutex);

    std::string path
        = FW::perEventFilepath(m_cfg.outputDir, "tracks.obj", ctx.eventNumber);
    std::ofstream os(path, std::ofstream::out | std::ofstream::trunc);
    if (!os) {
      throw std::ios_base::failure("Could not open '" + path + "' to write");
    }

    os << std::setprecision(m_cfg.outputPrecision);

    // loop over the cells
    for (auto& eCell : ecells) {
      // the event paramters
      auto sPosition = eCell.startParameters->position();
      // remember the first counter
      size_t fCounter = m_vCounter;

      // loop over extrapolation steps - add bezier points
      auto lPosition  = eCell.startParameters->position();
      auto lDirection = eCell.startParameters->momentum().unit();
      for (auto& es : eCell.extrapolationSteps) {
        if (es.parameters) {
          /// step parameters
          const T& pars       = (*es.parameters);
          auto     tPosition  = pars.position();
          auto     tDirection = pars.momentum().unit();
          // don't write the start position another time
          if (tPosition == sPosition) continue;
          // write the start parameters because
          // at least one other space point is here
          if (m_vCounter == fCounter) {
            // increase the vertex counter
            ++m_vCounter;
            // write the actual point
            os << "v " << m_cfg.outputScalor * sPosition.x() << ", "
               << m_cfg.outputScalor * sPosition.y() << ", "
               << m_cfg.outputScalor * sPosition.z() << " # initial point "
               << '\n';
          }
          if (m_cfg.outputBezierSegment > 0.) {
            // construct P1 and P2
            // we take the nominal distance divided by segments
            double nDist = (tPosition - lPosition).mag();
            // calculate the number of segments
            size_t segments = size_t(nDist / m_cfg.outputBezierSegment);
            if (segments < 2) continue;
            // rescale
            nDist /= (double)segments;
            Acts::Vector3D p1
                = lPosition + m_cfg.outputBezierSegment * lDirection;
            Acts::Vector3D p2
                = tPosition - m_cfg.outputBezierSegment * tDirection;
            // loop over the bezier segments
            for (size_t ib = 1; ib <= size_t(segments - 1); ib++) {
              double t = ib / (double)segments;
              auto   bPoint
                  = calculateBezierPoint(t, lPosition, p1, p2, tPosition);
              ++m_vCounter;
              // write the space point
              os << "v " << m_cfg.outputScalor * bPoint.x() << ", "
                 << m_cfg.outputScalor * bPoint.y() << ", "
                 << m_cfg.outputScalor * bPoint.z() << " # bezier point "
                 << '\n';
            }  // end of bezier segent writing
          }    // end of bezier loop
          // increase the counter
          ++m_vCounter;
          // write the actual point
          os << "v " << m_cfg.outputScalor * tPosition.x() << ", "
             << m_cfg.outputScalor * tPosition.y() << ", "
             << m_cfg.outputScalor * tPosition.z() << " # excell point "
             << '\n';

          // set for the next bezier loop
          lPosition  = tPosition;
          lDirection = tDirection;
        }  // end of parameter check

      }  // end of extrapolation step loop

      // write out the line
      for (size_t iv = fCounter + 1; iv < m_vCounter; ++iv)
        os << "l " << iv << " " << iv + 1 << '\n';

    }  // end of eCells loop

    os << '\n' << '\n';
    // return success
    return FW::ProcessCode::SUCCESS;
  }

  template <class T>
  Acts::Vector3D
  ObjExCellWriter<T>::calculateBezierPoint(double                t,
                                           const Acts::Vector3D& p0,
                                           const Acts::Vector3D& p1,
                                           const Acts::Vector3D& p2,
                                           const Acts::Vector3D& p3) const
  {
    double u   = 1. - t;
    double tt  = t * t;
    double uu  = u * u;
    double uuu = uu * u;
    double ttt = tt * t;

    Acts::Vector3D p = uuu * p0;  // first term
    p += 3 * uu * t * p1;         // second term
    p += 3 * u * tt * p2;         // third term
    p += ttt * p3;                // fourth term
    return p;
  }

}  // namespace Obj
}  // namespace FW

#endif  // ACTFW_PLUGINS_OBJEXCELLWRITER_H
