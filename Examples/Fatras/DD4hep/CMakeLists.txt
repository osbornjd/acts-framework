add_executable(ACTFWFatrasGunDD4hep FatrasGunDD4hep.cpp)
target_link_libraries(ACTFWFatrasGunDD4hep PUBLIC ACTFWFatrasCommon)
target_link_libraries(ACTFWFatrasGunDD4hep PUBLIC ACTFWDD4hepPlugin ${DD4hep_LIBRARIES})

install(TARGETS ACTFWFatrasGunDD4hep RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
