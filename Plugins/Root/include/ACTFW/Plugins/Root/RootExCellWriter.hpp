// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_ROOTEXCELLWRITER_H
#define ACTFW_PLUGINS_ROOTEXCELLWRITER_H

#include <TFile.h>
#include <TTree.h>
#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

#ifndef MAXSTEPS
#define MAXSTEPS 100
#endif

namespace FW {

namespace Root {

  /// @class ExtrapolationCellWriter
  ///
  /// A root based implementation to write out extrapolation steps.
  ///
  /// Safe to use from multiple writer threads.
  ///
  template <class T>
  class RootExCellWriter
      : public FW::WriterT<std::vector<Acts::ExtrapolationCell<T>>>
  {
  public:
    using Base = FW::WriterT<std::vector<Acts::ExtrapolationCell<T>>>;

    ///  @struct ExtrapolationStep
    ///  this holds the information to be written out
    struct ExtrapolationStep
    {
      float x, y, z;     ///< position (global)
      float px, py, pz;  ///< momentum
      float type;        ///< type of the step
    };

    // @struct Config
    //
    // The nested config class
    struct Config
    {
    public:
      std::string collection;             ///< particle collection to write
      std::string filePath;               ///< path of the output file
      std::string fileMode = "RECREATE";  ///< file access mode
      std::string treeName
          = "extrapolation_cells";  ///< name of the output tree
      bool writeSensitive;
      bool writeMaterial;
      bool writePassive;
      bool writeBoundary;
    };

    /// Constructor
    /// @param cfg is the configuration class
    RootExCellWriter(const Config&        cfg,
                     Acts::Logging::Level level = Acts::Logging::INFO);

    /// Virtual destructor
    ~RootExCellWriter() override = default;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

  protected:
    /// The protected writeT method, called by the WriterT base
    /// @param [in] ctx is the algorithm context for event consistency
    /// @param [in] ecells are the celss to be written out
    ProcessCode
    writeT(
        const FW::AlgorithmContext&                    ctx,
        const std::vector<Acts::ExtrapolationCell<T>>& ecells) final override;

    /// The config class
    Config m_cfg;
    /// Protect multi-threaded writes
    std::mutex m_writeMutex;
    /// The output file
    TFile* m_outputFile;
    /// The output tree
    TTree* m_outputTree;
    /// Global eta start
    float m_eta;
    /// Global phi start
    float m_phi;
    /// Material in X0
    float m_materialX0;
    /// Material in L0
    float m_materialL0;
    /// Global position x of the step
    std::vector<float> m_s_positionX;
    /// Global position y of the step
    std::vector<float> m_s_positionY;
    /// Global position z of the step
    std::vector<float> m_s_positionZ;
    /// Global radial position of the step
    std::vector<float> m_s_positionR;
    /// Step material X0
    std::vector<float> m_s_materialX0;
    /// Step material L0
    std::vector<float> m_s_materialL0;
    /// Type of the step: material
    std::vector<int> m_s_material;
    /// Type of the step: boundary
    std::vector<int> m_s_boundary;
    /// type of the step: sensitive
    std::vector<int> m_s_sensitive;
    /// Volume identification
    std::vector<int> m_s_volumeID;
    /// Layer identification
    std::vector<int> m_s_layerID;
    /// Surface identification
    std::vector<int> m_s_surfaceID;
    /// Local position - first coordinate
    std::vector<float> m_s_localposition0;
    /// Local position - second coordinate
    std::vector<float> m_s_localposition1;
    /// Number of hits in sensitive material
    int m_hits;
    /// The radial position of the first particle properties
    float m_r0;
    /// The radial position of the second particle properties
    float m_r1;
    /// The transverse momentum of the first particle properties
    float m_pt0;
    /// The transverse momentum of the second particle properties
    float m_pt1;
    /// The displacement in x
    float m_dx;
    /// The displacement in y
    float m_dy;
    /// The displacment in z
    float m_dz;
    /// The radial displacement
    float m_dr;
    /// The difference of the x components of the momentum
    float m_dPx;
    // The difference of the y components of the momentum
    float m_dPy;
    // The difference of the z components of the momentum
    float m_dPz;
    // The difference of the transverse momenta
    float m_dPt;
  };
}  // namespace Root
}  // namespace FW

#include "RootExCellWriter.ipp"

#endif  // ACTFW_PLUGINS_ROOTEXCELLWRITER_H
