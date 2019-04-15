// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// RootIndexedMaterialWriter.h
///////////////////////////////////////////////////////////////////

#pragma once

#include <mutex>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "Acts/Material/ISurfaceMaterial.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "TFile.h"

namespace Acts {
using IndexedSurfaceMaterial
    = std::pair<GeometryID, std::unique_ptr<const ISurfaceMaterial>>;
}

namespace FW {

namespace Root {

  /// @class RootMaterialTrackWriter
  ///
  /// @brief Writes out MaterialTrack entities from a root file
  ///
  /// This service is the root implementation of the IWriterT.
  /// It writes out a MaterialTrack which is usually generated from
  /// Geant4 material mapping

  class RootIndexedMaterialWriter
      : public FW::IWriterT<Acts::IndexedSurfaceMaterial>
  {

  public:
    /// @class Config
    /// Configuration of the Writer
    struct Config
    {
      /// The name of the output tree
      std::string folderNameBase = "Material";
      /// The volume identification string
      std::string voltag = "_vol";
      /// The layer identification string
      std::string laytag = "_lay";
      /// The approach identification string
      std::string apptag = "_app";
      /// The sensitive identification string
      std::string sentag = "_sen";
      /// The bin number tag
      std::string ntag = "n";
      /// The value tag -> binning values: binZ, binR, binPhi, etc.
      std::string vtag = "v";
      /// The option tag -> binning options: open, closed
      std::string otag = "o";
      /// The range min tag: min value
      std::string mintag = "min";
      /// The range max tag: max value
      std::string maxtag = "max";
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
      /// The name of the output file
      std::string fileName = "material-maps.root";
      /// The default logger
      std::shared_ptr<const Acts::Logger> logger;
      // The name of the writer
      std::string name = "";

      /// Constructor
      ///
      /// @param lname Name of the writer tool
      /// @param lvl The output logging level
      Config(const std::string&   lname = "MaterialWriter",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
      {
      }
    };

    /// Constructor
    ///
    /// @param cfg The configuration struct
    RootIndexedMaterialWriter(const Config& cfg);

    /// Virtual destructor
    ~RootIndexedMaterialWriter() override;

    /// Framework name() method
    std::string
    name() const final override;

    /// Interface method which writes out the MaterialTrack entities
    ///
    /// @param context is the algorithm context in case it is contextual
    /// @param ism is the indexed surface material
    FW::ProcessCode
    write(const AlgorithmContext&             context,
          const Acts::IndexedSurfaceMaterial& ism) final override;

  private:
    /// The config class
    Config m_cfg;
    /// mutex used to protect multi-threaded writes
    std::mutex m_write_mutex;
    /// The output file name
    TFile* m_outputFile;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

  inline std::string
  RootIndexedMaterialWriter::name() const
  {
    return m_cfg.name;
  }

}  // namespace Root
}  // namespace FW
