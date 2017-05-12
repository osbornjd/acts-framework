///////////////////////////////////////////////////////////////////
// MaterialRootification.cpp
///////////////////////////////////////////////////////////////////

#include "MaterialRootification.hpp"
#include <iostream>
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecWriter.hpp"
#include "ACTS/Material/MaterialProperties.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"

FWE::MaterialRootification::MaterialRootification(
    const FWE::MaterialRootification::Config& cnf,
    std::unique_ptr<Acts::Logger>             log)
  : FW::Algorithm(cnf, std::move(log)), m_cfg(cnf)
{
}

FWE::MaterialRootification::~MaterialRootification()
{
}

FW::ProcessCode
FWE::MaterialRootification::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialRootification::execute(const FW::AlgorithmContext context) const
{
  // creating some random steps and writing them out
  std::vector<Acts::MaterialStep> steps;
  Acts::MaterialStep              step1(Acts::MaterialProperties(),
                           Acts::MaterialStep::Position(0., 1., 2.));
  Acts::MaterialStep step2(Acts::MaterialProperties(1., 1., 1., 1., 1., 1.),
                           Acts::MaterialStep::Position(6., 7., 8.));
  Acts::MaterialStep step3(Acts::MaterialProperties(2., 2., 2., 2., 2., 2.),
                           Acts::MaterialStep::Position(4., 3., 2.));
  steps.push_back(step1);
  steps.push_back(step2);
  steps.push_back(step3);
  Acts::MaterialTrackRecord mtrecord1(
      Acts::MaterialStep::Position(0., 0., 0.), 4., 5., steps);
  Acts::MaterialTrackRecord mtrecord2(
      Acts::MaterialStep::Position(0., 0., 0.), 1., 2., steps);
  Acts::MaterialTrackRecord mtrecord3(
      Acts::MaterialStep::Position(0., 0., 0.), 2., 3., steps);
  m_cfg.materialTrackRecWriter->write(mtrecord1);
  m_cfg.materialTrackRecWriter->write(mtrecord2);
  m_cfg.materialTrackRecWriter->write(mtrecord3);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialRootification::finalize()
{
  // reading them in
  ACTS_VERBOSE("finalize successful.");
  m_cfg.materialTrackRecReader->read();
  return FW::ProcessCode::SUCCESS;
}
