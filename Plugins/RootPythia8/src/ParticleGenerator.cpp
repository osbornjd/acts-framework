
#include "ACTS/Utilities/Units.hpp"
#include "ACTFW/RootPythia8/ParticleGenerator.hpp"
#include "TSystem.h"
#include "TPythia8.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TClonesArray.h"

FWRootPythia8::ParticleGenerator::ParticleGenerator(
    const FWRootPythia8::ParticleGenerator::Config& cfg,
    std::unique_ptr<Acts::Logger>                   mlogger)
  : FW::IParticleReader()
  , m_cfg(cfg)
  , m_pythia8(nullptr)
  , m_logger(std::move(mlogger))
{

  // loading the libraries
  gSystem->Load("$PYTHIA8/lib/libpythia8");
  gSystem->Load("libEG");
  gSystem->Load("libEGPythia8");
  // loading the data
  const char* p8dataenv = gSystem->Getenv("PYTHIA8DATA");
  if (!p8dataenv) {
    const char* path = gSystem->ExpandPathName("$PYTHIA8DATA");
    // throw a warning
    if (gSystem->AccessPathName(path))
      ACTS_WARNING("$PYTHIA8DATA is not correctly set.");
  }

  // Create pythia8 object
  m_pythia8 = new TPythia8();

  // Configure
  for (auto& pString : m_cfg.processStrings) {
    ACTS_VERBOSE("Setting string " << pString << " to Pythia8");
    m_pythia8->ReadString(pString.c_str());
  }

  // Initialize
  m_pythia8->Initialize(m_cfg.pdgBeam0 /* beam0 */,
                        m_cfg.pdgBeam1 /* beam1 */,
                        m_cfg.cmsEnergy /* TeV */);
}

FWRootPythia8::ParticleGenerator::~ParticleGenerator()
{
  delete m_pythia8;
}

std::vector<Acts::ParticleProperties>
FWRootPythia8::ParticleGenerator::particles() const
{
  // the return particles
  std::vector<Acts::ParticleProperties> particleProperties;
  /// the pythia particles
  TClonesArray* particles = new TClonesArray("TParticle", 1000);

  m_pythia8->GenerateEvent();

  // what is that ?
  // pythia8->EventListing();

  m_pythia8->ImportParticles(particles, "All");
  // get the entries
  Int_t np = particles->GetEntriesFast();
  // Particle loop
  for (Int_t ip = 0; ip < np; ip++) {
    // loop through the particles
    TParticle* part = (TParticle*)particles->At(ip);
    Int_t      ist  = part->GetStatusCode();
    // Positive codes are final particles.
    if (ist <= 0) continue;
    /// get parameters
    Int_t   pdg    = part->GetPdgCode();
    Float_t charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
    Float_t mass   = part->GetMass();
    // and now create a particle
    Acts::Vector3D vertex(part->Vx(), part->Vy(), part->Vz());
    Acts::Vector3D momentum(part->Px()*1000.,
                            part->Py()*1000.,
                            part->Pz()*1000.);
    // the particle should be ready now
    particleProperties.push_back(
        Acts::ParticleProperties(vertex, momentum, mass, charge, pdg));
  }
  // clear
  delete particles;
  // return
  return particleProperties;
}

void
FWRootPythia8::ParticleGenerator::skip(size_t nEvents) const
{
  for (size_t is = 0; is < nEvents; ++is) m_pythia8->GenerateEvent();
  return;
}
