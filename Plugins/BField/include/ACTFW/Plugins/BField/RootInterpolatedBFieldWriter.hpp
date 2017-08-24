///////////////////////////////////////////////////////////////////
// RootInterpolatedBFieldWriter.hpp
///////////////////////////////////////////////////////////////////
#ifndef ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H
#define ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H

#include <TTree.h>
#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class InterpolatedBFieldMap;
}

namespace FW {

namespace BField {

  /// @enum gridType
  /// Describes the axes definition of the grid of the magnetic field map
  enum GridType { rz = 0, xyz = 1 };

  /// @class RootInterpolatedBFieldWriter
  ///
  /// Writes out the Acts::InterpolatedbFieldMap. Currently implemented for 'rz'
  /// and 'xyz' field maps.

  class RootInterpolatedBFieldWriter : public IService
  {
  public:
    struct Config
    {
      /// The name of the output tree
      std::string treeName = "TTree";
      /// The name of the output file
      std::string fileName = "TFile.root";
      /// the file access mode (recreate by default)
      std::string fileMode = "recreate";
      /// The magnetic field to be written out
      std::shared_ptr<const Acts::InterpolatedBFieldMap> bField = nullptr;
      /// The axes definition of the grid of the magnetic field map
      GridType gridType = xyz;
    };

    /// Constructor
    RootInterpolatedBFieldWriter(
        const Config&                       cfg,
        std::unique_ptr<const Acts::Logger> logger
        = Acts::getDefaultLogger("RootInterpolatedBFieldWriter",
                                 Acts::Logging::INFO));

    /// Framework name() method
    std::string
    name() const override final;

    // Framework initialize method
    FW::ProcessCode
    initialize() override final;

    /// Framework finalize mehtod
    FW::ProcessCode
    finalize() override final;

  private:
    /// The configuration class
    Config m_cfg;
    /// The loger object
    std::unique_ptr<const Acts::Logger> m_logger;
    /// Mutex used to protect multi-threaded writes
    std::mutex m_write_mutex;
    /// The root output file
    TFile* m_outputFile;
    /// The root output tree
    TTree* m_outputTree;
    /// The position values
    double m_x;
    double m_y;
    double m_z;
    double m_r;
    /// The BField values
    double m_Bx;
    double m_By;
    double m_Bz;
    double m_Br;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };

}  // end of namespace BField

}  // end of namespace FW

#endif  // ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H
