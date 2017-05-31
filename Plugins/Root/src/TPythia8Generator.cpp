#include "ACTS/Utilities/Units.hpp"
#include "ACTFW/Plugins/Root/TPythia8Generator.hpp"
#include "TSystem.h"
#include "TPythia8.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TClonesArray.h"

FWRoot::TPythia8Generator::TPythia8Generator(
    const FWRoot::TPythia8Generator::Config& cfg,
    std::unique_ptr<const Acts::Logger> mlogger)
  : FW::IReaderT< std::vector<Acts::ParticleProperties> >()
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

FWRoot::TPythia8Generator::~TPythia8Generator()
{
  delete m_pythia8;
}

FW::ProcessCode
FWRoot::TPythia8Generator::read(
  std::vector<Acts::ParticleProperties>& particleProperties, size_t skip) 
{
  
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_read_mutex);
  
  /// the pythia particles
  TClonesArray* particles = new TClonesArray("TParticle", 1000);

  // skip if needed
  if (skip){
    for (size_t is = 0; is < skip; ++is)
         m_pythia8->GenerateEvent();
    return FW::ProcessCode::SUCCESS;
  }
  // the actual event
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
    // pythia returns charge in units of 1/3 
    Float_t charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
    Float_t mass   = part->GetMass();
    // and now create a particle
    Acts::Vector3D vertex(part->Vx(), part->Vy(), part->Vz());
    // make th nuit nicer ... 
    Acts::Vector3D momentum(part->Px()*1000.,
                            part->Py()*1000.,
                            part->Pz()*1000.);
    // the particle should be ready now
    particleProperties.push_back(
        Acts::ParticleProperties(vertex, momentum, mass, charge, pdg));
  }
  // clear
  delete particles;
  // return success
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::TPythia8Generator::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::TPythia8Generator::finalize()
{
  return FW::ProcessCode::SUCCESS;
}
