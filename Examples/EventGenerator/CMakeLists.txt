add_executable(ActsParticleGun ParticleGun.cpp)
target_include_directories(ActsParticleGun PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ActsParticleGun
  PRIVATE ActsFrameworkGenerators ACTFWExamplesCommon ACTFWRootPlugin
    ACTFWCsvPlugin ${Boost_LIBRARIES})

install(TARGETS ActsParticleGun RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
