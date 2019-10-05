add_executable(ACTFWHepMC3Example HepMC3Example.cpp)
target_link_libraries(
  ACTFWHepMC3Example
  PRIVATE ActsCore ACTFramework ACTFWPluginHepMC3 Boost::program_options)

install(
  TARGETS ACTFWHepMC3Example
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
