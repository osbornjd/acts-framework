add_executable(
  ActsGenParticleGun
  ParticleGun.cpp)
target_link_libraries(
  ActsGenParticleGun
  PRIVATE
    ActsFrameworkGenerators ACTFWExamplesCommon ActsFrameworkIoRoot
  ActsFrameworkIoCsv ActsFrameworkPrinters Boost::program_options)

install(
  TARGETS ActsGenParticleGun
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(Pythia8 USE_PYTHIA8)
