# generic detector
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample)
target_include_directories(ACTFWGenericGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(ACTFWGenericGeometryExample PUBLIC ${DD4hep_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericGeometryExample PUBLIC ACTSCore)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ACTFramework ACTFWObjPlugin ACTFWGenericDetector)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWGenericGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# DD4hep detector
if (USE_DD4HEP)
  add_executable(ACTFWDD4hepGeometryExample src/DD4hepGeometryExample.cpp)
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${DD4hep_INCLUDE_DIRS})
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})  
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC ACTSCore)
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ACTFWDD4hepPlugin ACTFramework ACTFWObjPlugin)
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC ${DD4hep_LIBRARIES})
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ${Boost_LIBRARIES})

  install(TARGETS ACTFWDD4hepGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

add_library(ACTFWParticleDigitization STATIC FatrasCommon.cpp FatrasCommon.cpp)

target_include_directories(ACTFWParticleDigitization PUBLIC ${ROOT_INCLUDE_DIRS})
target_include_directories(ACTFWParticleDigitization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ACTFWParticleDigitization PUBLIC ACTSCore)
target_link_libraries(ACTFWParticleDigitization PUBLIC ACTFramework ACTFWExtrapolation ACTFWDigitization ACTFWParticleGun)
target_link_libraries(ACTFWParticleDigitization PUBLIC ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWRootPlugin ACTFWBFieldPlugin)

# generic detector test
add_executable(ACTFWFASER src/FASER)
target_include_directories(ACTFWFASER PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(ACTFWFASER PUBLIC ${DD4hep_INCLUDE_DIRS})
target_link_libraries(ACTFWFASER PUBLIC ACTSCore)
target_link_libraries(ACTFWFASER PRIVATE ACTFramework ACTFWObjPlugin ACTFWGenericDetector ACTFWParticleGun ACTFWDigitization ACTFWParticleDigitization ACTFWExtrapolation)
target_link_libraries(ACTFWFASER PRIVATE ACTFWRootPlugin ACTFWBFieldPlugin)
target_link_libraries(ACTFWFASER PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFASER RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

