///////////////////////////////////////////////////////////////////
// FullMaterialTest.cpp
///////////////////////////////////////////////////////////////////

#include "FullMaterialTest.hpp"
#include <iostream>
#include "ACTFW/Root/RootMaterialTrackRecReader.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Material/MaterialProperties.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"

FWE::FullMaterialTest::FullMaterialTest(
    const FWE::FullMaterialTest::Config& cnf,
    std::unique_ptr<Acts::Logger>        log)
  : FW::Algorithm(cnf, std::move(log)), m_cnf(cnf)
{
}

FWE::FullMaterialTest::~FullMaterialTest()
{
}

FW::ProcessCode
FWE::FullMaterialTest::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the writer
  if (!m_cnf.materialTrackRecWriter) {
    ACTS_ERROR("Algorithm::MaterialWriter not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  // set up the random numbers service
  if (!m_cnf.randomNumbers) {
    ACTS_ERROR("RandomNumbers service not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  // check extrapolation engine
  if (!m_cnf.extrapolationEngine) {
    ACTS_ERROR("ExtrapolationEngine not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::FullMaterialTest::execute(const FW::AlgorithmContext context) const
{
  // propagate through the detector and collect the material

  // create random direction
  double x = -1 + m_cnf.randomNumbers->draw(FW::Distribution::uniform) * 2;
  double y = -1 + m_cnf.randomNumbers->draw(FW::Distribution::uniform) * 2;
  double z = -1 + m_cnf.randomNumbers->draw(FW::Distribution::uniform) * 2;
  Acts::Vector3D       direction(x, y, z);
  const Acts::Vector3D startPos(0., 0., 0.);
  // create the beginning neutral parameters to extrapolate through the
  // geometry
  std::unique_ptr<Acts::ActsSymMatrixD<5>> cov;
  Acts::NeutralCurvilinearParameters       startParameters(
      std::move(cov), startPos, direction);
  // create a neutral extrapolation cell and configure it to only collect
  // material
  Acts::ExtrapolationCell<Acts::NeutralParameters> ecc(startParameters);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
  ecc.addConfigurationMode(
      Acts::ExtrapolationMode::CollectSensitive);  //@TODO check if needed
  ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);

  // call the extrapolation engine
  // screen output
  ACTS_DEBUG("===> forward extrapolation - collecting material <<===");
  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cnf.extrapolationEngine->extrapolate(ecc);
  ACTS_DEBUG("===> finished extrapolation <<===");
  // find all the intersected material
  std::vector<Acts::MaterialStep> msteps;
  // end parameter, if there
  if (eCode.isSuccess()) {
    ACTS_DEBUG("===> extrapolation was successful - now collecting "
               "materialsteps <<===");
    // loop over the collected information
    for (auto& es : ecc.extrapolationSteps) {
      if (es.stepConfiguration.checkMode(
              Acts::ExtrapolationMode::CollectMaterial)) {
        // create the material steps
        const Acts::MaterialStep::Position matPos(es.materialPosition.x(),
                                                  es.materialPosition.y(),
                                                  es.materialPosition.z());

        const Acts::MaterialProperties material(es.material->thickness(),
                                                es.material->x0(),
                                                es.material->l0(),
                                                es.material->averageA(),
                                                es.material->averageZ(),
                                                es.material->dEdX());

        msteps.push_back(Acts::MaterialStep(material, matPos));
      }
      // continue if we have parameters
    }  // loop over extrapolationsteps
  }    // extrapolation success
  ACTS_DEBUG(
      "===> collected materialsteps -- writing out MaterialTrackRecord <<===");
  const Acts::MaterialTrackRecord mtrecord(
      Acts::MaterialStep::Position(startPos.x(), startPos.y(), startPos.z()),
      direction.theta(),
      direction.phi(),
      msteps);
  m_cnf.materialTrackRecWriter->write(mtrecord);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::FullMaterialTest::finalize()
{
  ACTS_INFO("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}
