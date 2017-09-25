#include "ACTFW/Fatras/FatrasAlgorithm.hpp"

template <class MaterialEngine>
FW::FatrasAlgorithm<MaterialEngine>::FatrasAlgorithm(
                                                      const Config& cfg, Acts::Logging::Level loglevel)
: FW::BareAlgorithm("FatrasAlgorithm", loglevel), m_cfg(cfg), m_exAlgorithm(std::make_unique<const FW::ExtrapolationAlgorithm>(m_cfg.exConfig,loglevel))
{
}

template <class MaterialEngine>
FW::ProcessCode
FW::FatrasAlgorithm<MaterialEngine>::execute(
                                              const FW::AlgorithmContext context) const
{
    // Create an algorithm local random number generator
    FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);
    // Set the random generator of the material interaction
    m_cfg.materialInteractionEngine->setRandomGenerator(rng);
    // call the extrapolation algorithm
    m_exAlgorithm->execute(context);
    
    return FW::ProcessCode::SUCCESS;
}
