# the writer Plugins
add_subdirectory_if(Csv USE_TGEO)
add_subdirectory_if(Root USE_TGEO)
add_subdirectory(Obj)
#add_subdirectory(Json)
#add_subdirectory_if(DD4hepG4 USE_DD4HEP AND USE_GEANT4)
add_subdirectory_if(Geant4 USE_GEANT4)
add_subdirectory_if(Pythia8 USE_PYTHIA8 AND USE_FATRAS)
add_subdirectory(BField)
