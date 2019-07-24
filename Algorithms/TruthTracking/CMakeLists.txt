set(Simulation_SOURCES
  ACTFW/TruthTracking/TruthVerticesToTracks.cpp
  ACTFW/TruthTracking/TrackSelector.cpp
  )

add_library(ActsFrameworkTruthTracking SHARED ${Simulation_SOURCES})
target_include_directories(ActsFrameworkTruthTracking
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(ActsFrameworkTruthTracking
  PUBLIC ActsCore ACTFramework ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS ActsFrameworkTruthTracking LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

