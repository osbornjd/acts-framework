# Common steering of the events
add_subdirectory(Common)

# Framework test section - without dependencies additional dependencies
add_subdirectory(HelloWorld)
add_subdirectory(RandomNumbers)
add_subdirectory(WhiteBoard)

# Geometry, material and magnetic field
add_subdirectory(Geometry)
#add_subdirectory_if(MaterialMapping USE_DD4HEP AND USE_GEANT4)
add_subdirectory(BField)

# Particle gun and Event reading
add_subdirectory_if(ParticleGun USE_FATRAS)
add_subdirectory_if(ReadEvgen USE_FATRAS AND USE_PYTHIA8)

# Propagation (new style) and Extrapolation (legacy)
add_subdirectory(Propagation)
add_subdirectory_if(Extrapolation USE_LEGACY)

# Fatras example for fast simulation, digitization truth/full tracking
add_subdirectory_if(Fatras USE_FATRAS AND USE_PYTHIA8)

