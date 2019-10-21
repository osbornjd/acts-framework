set(Simulation_SOURCES
  ACTFW/TruthTracking/TruthVerticesToTracks.cpp
  ACTFW/TruthTracking/TrackSelector.cpp
  )

add_library(ActsFrameworkTruthTracking SHARED ${Simulation_SOURCES})
target_include_directories(ActsFrameworkTruthTracking
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(ActsFrameworkTruthTracking
  PRIVATE ActsCore ACTFramework Boost::program_options)

install(TARGETS ActsFrameworkTruthTracking LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

