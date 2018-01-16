// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H
#define ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H 1

#include <TTree.h>
#include <mutex>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FW {
namespace Root {

  /// @class MaterialEffectsWriter
  ///
  /// A root based implementation to compare two different particle properties,
  /// by
  /// writing out the displacement and the difference of the momentum and
  /// energy.
  ///
  class MaterialEffectsWriter final
      : public FW::
            WriterT<std::vector<std::pair<std::pair<Acts::ParticleProperties,
                                                    Acts::Vector3D>,
                                          std::pair<Acts::ParticleProperties,
                                                    Acts::Vector3D>>>>
  {
  public:
    using Base
        = WriterT<std::vector<std::pair<std::pair<Acts::ParticleProperties,
                                                  Acts::Vector3D>,
                                        std::pair<Acts::ParticleProperties,
                                                  Acts::Vector3D>>>>;
    // @class Config
    //
    // The nested config class
    class Config
    {
    public:
      /// particle properties collection to write
      std::string collection;
      /// The name of the output tree
      std::string treeName = "TTree";
      /// The name of the output file
      std::string filePath = "TFile.root";
      /// The mode of the file
      std::string fileMode = "RECREATE";
    };

    /// Constructor
    ///
    /// @param cfg is the configuration class
    MaterialEffectsWriter(const Config&        cfg,
                          Acts::Logging::Level level = Acts::Logging::INFO);

    /// Destructor
    virtual ~MaterialEffectsWriter() = default;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

    /// The write interface
    /// @param pProperties is the vector of pairs of particle properties to be
    /// compared
    FW::ProcessCode
    writeT(const AlgorithmContext& ctx,
           const std::vector<std::pair<std::pair<Acts::ParticleProperties,
                                                 Acts::Vector3D>,
                                       std::pair<Acts::ParticleProperties,
                                                 Acts::Vector3D>>>& pProperties)
        override final;

  private:
    /// The config class
    Config m_cfg;
    /// Mutex used to protect multi-threaded writes
    std::mutex m_write_mutex;
    /// The output file name
    TFile* m_outputFile;
    // This is the main tree for outputting
    TTree* m_outputTree;
    /// The radial position of the first particle properties
    std::vector<float> m_r0;
    /// The radial position of the second particle properties
    std::vector<float> m_r1;
    /// The transverse momentum of the first particle properties
    std::vector<float> m_pt0;
    /// The transverse momentum of the second particle properties
    std::vector<float> m_pt1;
    /// The displacement in x
    std::vector<float> m_dx;
    /// The displacement in y
    std::vector<float> m_dy;
    /// The displacment in z
    std::vector<float> m_dz;
    /// The radial displacement
    std::vector<float> m_dr;
    /// The difference of the x components of the momentum
    std::vector<float> m_dPx;
    // The difference of the y components of the momentum
    std::vector<float> m_dPy;
    // The difference of the z components of the momentum
    std::vector<float> m_dPz;
    // The difference of the transverse momenta
    std::vector<float> m_dPt;
  };
}  // end of namespace Root
}  // end of namespace Root

#endif  // ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H
