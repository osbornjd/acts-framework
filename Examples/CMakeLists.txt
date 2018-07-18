# common steering of the events
add_subdirectory(Common)

# framework test section - without dependencies
add_subdirectory(HelloWorld)
add_subdirectory(RandomNumbers)
add_subdirectory(WhiteBoard)

# geometry, material and magnetic field
add_subdirectory(Geometry)
#add_subdirectory_if(MaterialMapping USE_DD4HEP AND USE_GEANT4)
add_subdirectory(BField)

# particle gun and event reading
add_subdirectory_if(ParticleGun USE_FATRAS)
add_subdirectory_if(ReadEvgen USE_FATRAS AND USE_PYTHIA8)

# propagation and extrapolation
add_subdirectory(Propagation)
add_subdirectory(Extrapolation) # create a new USE_LEGACY flag

# Fatras example for fast simulation
add_subdirectory_if(Fatras USE_FATRAS AND USE_PYTHIA8)

