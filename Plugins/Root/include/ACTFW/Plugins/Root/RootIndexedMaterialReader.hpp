// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "Acts/Material/SurfaceMaterial.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"

class TFile;

namespace Acts {
using IndexedSurfaceMaterial
    = std::pair<GeometryID, std::unique_ptr<const SurfaceMaterial>>;
}

namespace FW {

namespace Root {

  /// @class RootMaterialTrackReader
  ///
  /// @brief Reads in MaterialTrack entities from a root file
  ///
  /// This service is the root implementation of the ImaterialTrackReader.
  /// It reads in a vector of MaterialTrack entities from a given root tree
  /// of a given root file. The input file and tree are set over the
  /// configuration
  /// object.
  class RootIndexedMaterialReader
      : public FW::IReaderT<Acts::IndexedSurfaceMaterial>
  {
  public:
    /// @class Config
    /// Configuration of the Reader
    class Config
    {
    public:
      /// The name of the input tree
      std::string folderNameBase = "Material";
      /// The volume identification string
      std::string voltag = "_vol";
      /// The layer identification string
      std::string laytag = "_lay";
      /// The approach identification string
      std::string apptag = "_app";
      /// The sensitive identification string
      std::string sentag = "_sen";
      /// The binning tag
      std::string btag = "b";
      /// The value tag
      std::string vtag = "v";
      /// The thickness tag
      std::string ttag = "t";
      /// The x0 tag
      std::string x0tag = "x0";
      /// The l0 tag
      std::string l0tag = "l0";
      /// The A tag
      std::string atag = "A";
      /// The Z tag
      std::string ztag = "Z";
      /// The rho tag
      std::string rhotag = "rho";
      /// The name of the input file
      std::string fileName = "";
      /// The default logger
      std::shared_ptr<const Acts::Logger> logger;
      /// The name of the service
      std::string name;

      /// Constructor
      ///
      /// @param lname Name of the writer tool
      /// @param lvl The output logging level
      Config(const std::string&   lname = "MaterialReader",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
      {
      }
    };

    /// Constructor
    ///
    /// @param cfg configuration struct for the reader
    RootIndexedMaterialReader(const Config& cfg);

    /// Virtual destructor
    ~RootIndexedMaterialReader() override;

    /// Framework name() method
    std::string
    name() const final override;

    /// Read method
    ///
    /// @param ism The indexted material map
    FW::ProcessCode
    read(Acts::IndexedSurfaceMaterial& ism,
         size_t                        skip    = 0,
         const FW::AlgorithmContext*   context = nullptr) final override;

  private:
    /// The config class
    Config m_cfg;

    /// The input file
    TFile* m_inputFile;

    /// mutex used to protect multi-threaded reads
    std::mutex m_read_mutex;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

  inline std::string
  RootIndexedMaterialReader::name() const
  {
    return m_cfg.name;
  }

}  // namespace Root
}  // namespace FW
