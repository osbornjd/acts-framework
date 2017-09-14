///////////////////////////////////////////////////////////////////
// RootInterpolatedBFieldWriter.cpp
///////////////////////////////////////////////////////////////////
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "TFile.h"

FW::BField::RootInterpolatedBFieldWriter::RootInterpolatedBFieldWriter(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg)
  , m_logger(std::move(logger))
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
}

std::string
FW::BField::RootInterpolatedBFieldWriter::name() const
{
  return "RootInterpolatedBFieldWriter";
}

FW::ProcessCode
FW::BField::RootInterpolatedBFieldWriter::initialize()
{
  if (!m_cfg.bField) {
    ACTS_ERROR("No magnetic field to be written out handed over!");
    return FW::ProcessCode::ABORT;
  }

  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), m_cfg.fileMode.c_str());
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  // initial parameters
  m_outputTree->Branch("x", &m_x);
  m_outputTree->Branch("y", &m_y);
  m_outputTree->Branch("z", &m_z);
  m_outputTree->Branch("r", &m_r);
  m_outputTree->Branch("Bx", &m_Bx);
  m_outputTree->Branch("By", &m_By);
  m_outputTree->Branch("Bz", &m_Bz);
  m_outputTree->Branch("Br", &m_Br);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::BField::RootInterpolatedBFieldWriter::finalize()
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
  // Get the underlying mapper of the InterpolatedBFieldMap
  auto mapper = m_cfg.bField->getMapper();
  // Access the minima and maxima of all axes
  auto minima = mapper.getMin();
  auto maxima = mapper.getMax();
  auto nBins  = mapper.getNBins();

  double stepX = 0., stepY = 0., stepZ = 0.;
  double minX = 0., minY = 0., minZ = 0.;
  double maxX = 0., maxY = 0., maxZ = 0.;
  size_t nBinsX = 0, nBinsY = 0, nBinsZ = 0;

  if (m_cfg.gridType == GridType::xyz) {
    if (minima.size() != 3 || maxima.size() != 3) {
      ACTS_ERROR("Wrong number of axes for given gridType: "
                 << minima.size()
                 << ", axes given - but 3 are required! Please check gridType, "
                    "current gridType(0 = rz, 1 = xyz) is : "
                 << m_cfg.gridType);
      return FW::ProcessCode::ABORT;
    }
    stepX = fabs(minima.at(0) - maxima.at(0)) / nBins.at(0);
    stepY = fabs(minima.at(1) - maxima.at(1)) / nBins.at(1);
    stepZ = fabs(minima.at(2) - maxima.at(2)) / nBins.at(2);

    minX = minima.at(0);
    minY = minima.at(1);
    minZ = minima.at(2);

    nBinsX = nBins.at(0);
    nBinsY = nBins.at(1);
    nBinsZ = nBins.at(2);

    for (int i = 0; i < nBinsX; i++) {
      double x = minX + i * stepX;
      for (int j = 0; j < nBinsY; j++) {
        double y = minY + j * stepY;
        for (int k = 0; k < nBinsZ; k++) {
          double         z = minZ + k * stepZ;
          Acts::Vector3D position(x, y, z);
          auto           bField = m_cfg.bField->getField(position);

          m_x  = x;
          m_y  = y;
          m_z  = z;
          m_r  = position.perp();
          m_Bx = bField.x();
          m_By = bField.y();
          m_Bz = bField.z();
          m_Br = bField.perp();

          m_outputTree->Fill();
        }  // for z
      }    // for y
    }      // for x

  } else {
    if (minima.size() != 2 || maxima.size() != 2) {
      ACTS_ERROR("Wrong number of axes for given gridType: "
                 << minima.size()
                 << ", axes given - but 2 are required! Please check "
                    "gridType, current gridType(0 = rz, 1 = xyz) is : "
                 << m_cfg.gridType);
      return FW::ProcessCode::ABORT;
    }

    double stepR   = fabs(minima.at(0) - maxima.at(0)) / nBins.at(0);
    stepZ          = fabs(minima.at(1) - maxima.at(1)) / nBins.at(1);
    double stepPhi = (2 * M_PI) / 100.;

    double minR   = minima.at(0);
    minZ          = minima.at(1);
    double minPhi = -M_PI;

    double nBinsR   = nBins.at(0);
    nBinsZ          = nBins.at(1);
    double nBinsPhi = 100.;

    for (int i = 0; i < nBinsPhi; i++) {
      double phi = minPhi + i * stepPhi;
      for (int j = 0; j < nBinsR; j++) {
        double r = minR + j * stepR;
        for (int k = 0; k < nBinsZ; k++) {
          double         z = minZ + k * stepZ;
          Acts::Vector3D position(r * cos(phi), r * sin(phi), z);
          auto           bField = m_cfg.bField->getField(position);
          m_x                   = position.x();
          m_y                   = position.y();
          m_z                   = z;
          m_r                   = r;
          m_Bx                  = bField.x();
          m_By                  = bField.y();
          m_Bz                  = bField.z();
          m_Br                  = bField.perp();
          m_outputTree->Fill();
        }
      }
    }  // for
  }

  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return FW::ProcessCode::SUCCESS;
}
