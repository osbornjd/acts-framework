add_subdirectory(Digitization)
add_subdirectory(Extrapolation)
add_subdirectory(Fatras)
if (USE_GEANT4)
  add_subdirectory(MaterialMapping)
endif()
add_subdirectory(ReadEvgen)

