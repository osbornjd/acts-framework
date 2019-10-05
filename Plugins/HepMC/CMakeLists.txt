add_library(
  ACTFWPluginHepMC3 SHARED
  src/HepMC3Event.cpp
  src/HepMC3Particle.cpp
  src/HepMC3Reader.cpp
  src/HepMC3Vertex.cpp
  src/HepMC3Writer.cpp)
target_include_directories(
  ACTFWPluginHepMC3
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> ${HEPMC3_INCLUDE_DIR})
target_link_libraries(
  ACTFWPluginHepMC3
  PUBLIC ActsCore ACTFramework ${HEPMC3_LIBRARIES} HepPID)

install(
  TARGETS ACTFWPluginHepMC3
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
