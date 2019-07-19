set(Simulation_SOURCES
  ACTFW/Simulation/EventToTrackConverter.cpp
  ACTFW/Simulation/TrackSelector.cpp
  )

add_library(ActsFrameworkSimulation SHARED ${Simulation_SOURCES})
target_include_directories(ActsFrameworkSimulation
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(ActsFrameworkSimulation
  PUBLIC ActsCore ACTFramework ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS ActsFrameworkSimulation LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

