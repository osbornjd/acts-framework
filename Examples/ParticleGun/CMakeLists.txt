file(GLOB_RECURSE src_files "src/*.cpp")

add_executable(ACTFWParticleGunExample ${src_files})
target_include_directories(ACTFWParticleGunExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWParticleGunExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWParticleGun ACTFWReadEvgen)
target_link_libraries(ACTFWParticleGunExample PRIVATE ACTFWRootPlugin ACTFWCsvPlugin)
target_link_libraries(ACTFWParticleGunExample PRIVATE ActsCore)
target_link_libraries(ACTFWParticleGunExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWParticleGunExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
