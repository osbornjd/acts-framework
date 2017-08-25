//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H
#define ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H 1

#include <mutex>

#include <TTree.h>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
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
  class MaterialEffectsWriter
      : public FW::IWriterT<std::pair<Acts::ParticleProperties,
                                      Acts::ParticleProperties>>
  {
  public:
    // @class Config
    //
    // The nested config class
    class Config
    {
    public:
      /// The name of the output tree
      std::string treeName = "TTree";
      /// The name of the output file
      std::string fileName = "TFile.root";
      /// The mode of the file
      std::string fileMode = "RECREATE";
    };

    /// Constructor
    ///
    /// @param cfg is the configuration class
    MaterialEffectsWriter(const Config& cfg);

    /// Destructor
    virtual ~MaterialEffectsWriter();

    /// Framework intialize method
    FW::ProcessCode
    initialize() override final;

    /// Framework finalize mehtod
    FW::ProcessCode
    finalize() override final;

    /// The write interface
    /// @param pProperties is the vector of pairs of particle properties to be
    /// compared
    FW::ProcessCode
    write(const std::pair<Acts::ParticleProperties, Acts::ParticleProperties>&
              pProperties) override final;

    /// write a bit of string
    /// @param sinfo is some string info to be written
    /// @return is a ProcessCode indicating return/failure
    FW::ProcessCode
    write(const std::string& sinfo) override final;

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
}  // end of namespace Root
}  // end of namespace Root

#endif  // ACTFW_PLUGINS_MATERIALEFFECTSWRITER_H
