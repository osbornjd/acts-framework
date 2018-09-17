
add_subdirectory_if(Digitization USE_TGEO)
# particle gun and generated event reading 
add_subdirectory(ParticleGun)
add_subdirectory(ReadEvgen)
#add_subdirectory_if(MaterialMapping USE_GEANT4)
add_subdirectory(Propagation)

add_subdirectory(Fatras)

