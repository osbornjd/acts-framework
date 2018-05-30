# framework test section
add_subdirectory(HelloWorld)
add_subdirectory(RandomNumbers)
add_subdirectory(WhiteBoard)

# geometry, material and magnetic field
#add_subdirectory(GeometryBuilding)
#add_subdirectory_if(MaterialMapping USE_DD4HEP AND USE_GEANT4)
add_subdirectory(BField)

# particle gun and event reading
add_subdirectory(ParticleGun)

#add_subdirectory(Extrapolation)
#add_subdirectory(Fatras)
#add_subdirectory_if(ReadEvgen USE_PYTHIA8)
#add_subdirectory(Propagation)
#add_subdirectory(WhiteBoard)
