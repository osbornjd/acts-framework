// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_ROOTPROPAGATIONRESULTWRITER_H
#define ACTFW_PLUGINS_ROOTPROPAGATIONRESULTWRITER_H

#include <TFile.h>
#include <TTree.h>
#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FW {

namespace Root {

  /// @class TrackParametersWriter
  ///
  /// A root based implementation to write out track parameters
  ///
  /// Safe to use from multiple writer threads.
  template <class T>
  class RootTrackParametersWriter : public FW::WriterT<std::vector<T>>
  {
  public:
    using Base = FW::WriterT<std::vector<T>>;

    // The nested config class
    struct Config
    {
    public:
      std::string collection;             ///< particle collection to write
      std::string filePath;               ///< path of the output file
      std::string fileMode = "RECREATE";  ///< file access mode
      std::string treeName
          = "propagation_results";  ///< name of the output tree
    };

    /// Constructor
    /// @param [in] cfg is the configuration class
    /// @param [in] level is the logging level
    RootTrackParametersWriter(const Config&        cfg,
                              Acts::Logging::Level level = Acts::Logging::INFO);

    /// Destructor
    virtual ~RootTrackParametersWriter()
    {
      if (m_outputFile) {
        m_outputFile->cd();
        m_outputTree->Write();
        m_outputFile->Close();
      }
    };

  protected:
    /// The protected writeT method, called by the WriterT base
    /// @param [in] ctx is the algorithm context for event consistency
    /// @param [in] tparameters is thevector of track parameters
    ProcessCode
    writeT(const FW::AlgorithmContext& ctx,
           const std::vector<T>&       tparameters) final;

    Config     m_cfg;         ///< the config class
    std::mutex m_writeMutex;  ///< protect multi-threaded writes
    TFile*     m_outputFile;  ///< the output file
    TTree*     m_outputTree;  ///< the output tree

    std::vector<int>
        m_tag;  ///< the tag which one is present (in case of failure)

    std::vector<float> m_par0;  ///< parameter valication
    std::vector<float> m_par1;  ///< parameter valication
    std::vector<float> m_par2;  ///< parameter valication
    std::vector<float> m_par3;  ///< parameter valication
    std::vector<float> m_par4;  ///< parameter valication

    std::vector<float> m_cov00;  ///< covariance validation
    std::vector<float> m_cov01;  ///< covariance validation
    std::vector<float> m_cov02;  ///< covariance validation
    std::vector<float> m_cov03;  ///< covariance validation
    std::vector<float> m_cov04;  ///< covariance validation
    std::vector<float> m_cov11;  ///< covariance validation
    std::vector<float> m_cov12;  ///< covariance validation
    std::vector<float> m_cov13;  ///< covariance validation
    std::vector<float> m_cov14;  ///< covariance validation
    std::vector<float> m_cov22;  ///< covariance validation
    std::vector<float> m_cov23;  ///< covariance validation
    std::vector<float> m_cov24;  ///< covariance validation
    std::vector<float> m_cov33;  ///< covariance validation
    std::vector<float> m_cov34;  ///< covariance validation
    std::vector<float> m_cov44;  ///< covariance validation

    std::vector<float> m_positionX;  ///< global position x at the destination
    std::vector<float> m_positionY;  ///< global position y at the destination
    std::vector<float> m_positionZ;  ///< global position z at the destination
    std::vector<float> m_positionR;  ///< global position r at the destination

    std::vector<float> m_momentumPhi;  ///< momentum phi
    std::vector<float> m_momentumEta;  ///< momentum eta
    std::vector<float> m_momentumPt;   ///< momentum pT
  };

}  // namespace Root
}  // namespace FW

#include "RootTrackParametersWriter.ipp"

#endif  // ACTFW_PLUGINS_ROOTPROPAGATIONRESULTWRITER_H
