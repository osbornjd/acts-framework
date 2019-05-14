add_executable(
  ACTFWHepMC3Example
  src/HepMC3Example.cpp)
target_include_directories(
  ACTFWHepMC3Example
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWHepMC3Example
  PRIVATE ActsCore ACTFramework ACTFWPluginHepMC3 ${Boost_LIBRARIES})

install(TARGETS ACTFWHepMC3Example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES test.hepmc3 DESTINATION ${CMAKE_INSTALL_BINDIR})
