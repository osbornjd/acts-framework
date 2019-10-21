add_executable(ActsParticleGun ParticleGun.cpp)
target_link_libraries(ActsParticleGun
  PRIVATE ACTFramework ActsFrameworkGenerators ACTFWExamplesCommon ACTFWRootPlugin
    ACTFWCsvPlugin Boost::program_options)

install(TARGETS ActsParticleGun RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(USE_PYTHIA8)
  add_executable(ActsPythia8 Pythia8.cpp)
  target_link_libraries(ActsPythia8
    PRIVATE ACTFramework ActsFrameworkGenerators ActsFrameworkPythia8 ACTFWExamplesCommon
      ACTFWRootPlugin ACTFWCsvPlugin Boost::program_options)

  install(TARGETS ActsPythia8 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
