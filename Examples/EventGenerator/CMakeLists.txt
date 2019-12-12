add_executable(
  ActsParticleGun
  ParticleGun.cpp)
target_link_libraries(
  ActsParticleGun
  PRIVATE ActsFrameworkGenerators ACTFWExamplesCommon ACTFWRootPlugin
    ActsFrameworkIoCsv Boost::program_options)

install(
  TARGETS ActsParticleGun
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(Pythia8 USE_PYTHIA8)
