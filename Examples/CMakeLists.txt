# shared code comes first
add_subdirectory(Common)

add_subdirectory(BField)
add_subdirectory_if(Fatras USE_PYTHIA8)
add_subdirectory(Geometry)
add_subdirectory(HelloWorld)
#add_subdirectory_if(MaterialMapping USE_DD4HEP AND USE_GEANT4)
add_subdirectory(ParticleGun)
add_subdirectory(Propagation)
add_subdirectory(RandomNumbers)
add_subdirectory_if(ReadEvgen USE_PYTHIA8)
add_subdirectory(WhiteBoard)
add_subdirectory_if(ReadHepMC USE_HEPMC)