
add_subdirectory(Digitization)
# particle gun and generated event reading 
add_subdirectory(ParticleGun)
add_subdirectory(ReadEvgen)
#add_subdirectory_if(MaterialMapping USE_GEANT4)
add_subdirectory(Propagation)

add_subdirectory_if(Fatras USE_FATRAS)

#add_subdirectory(TruthTracking)

