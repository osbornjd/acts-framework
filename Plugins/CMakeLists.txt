add_subdirectory(BField)
add_subdirectory(Csv)
<<<<<<< HEAD
#add_subdirectory_if(DD4hepG4 USE_DD4HEP AND USE_GEANT4)
=======
add_subdirectory(Root)
add_subdirectory(Obj)
#add_subdirectory(Json)
add_subdirectory_if(DD4hepG4 USE_DD4HEP AND USE_GEANT4)
>>>>>>> 8d9e995a... adding OpenData detector, re-establish DD4HepG4 example
add_subdirectory_if(Geant4 USE_GEANT4)
#add_subdirectory(Json)
add_subdirectory(Obj)
add_subdirectory_if(Pythia8 USE_PYTHIA8)
add_subdirectory(Root)
