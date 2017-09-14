add_executable(ACTFWFatrasGunDD4hep FatrasGunDD4hep.cpp)
target_include_directories(ACTFWFatrasGunDD4hep PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWFatrasGunDD4hep PUBLIC ACTFWParticleGunCommon)
target_link_libraries(ACTFWFatrasGunDD4hep PUBLIC ACTFWDD4hepPlugin ${DD4hep_LIBRARIES})
target_link_libraries(ACTFWFatrasGunDD4hep PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFatrasGunDD4hep RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
