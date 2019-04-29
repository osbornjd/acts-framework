# generic detector
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample)
target_include_directories(ACTFWGenericGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ActsCore)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ACTFramework ACTFWExamplesCommon)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ACTFWObjPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWGenericDetector)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWGenericGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# alignable detector - with iov
add_executable(ACTFWAlignedGeometryExample src/AlignedGeometryExample)
target_include_directories(ACTFWAlignedGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE ActsCore)
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE ACTFramework ACTFWExamplesCommon)
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE ACTFWObjPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWGenericDetector ACTFWContextualDetector)
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWAlignedGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# alignable detector - with payload
add_executable(ACTFWPayloadGeometryExample src/PayloadGeometryExample)
target_include_directories(ACTFWPayloadGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE ActsCore)
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE ACTFramework ACTFWExamplesCommon)
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE ACTFWObjPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWGenericDetector ACTFWContextualDetector)
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWPayloadGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# TGEO based detector
if (USE_TGEO)
  add_executable(ACTFWTGeoGeometryExample src/TGeoGeometryExample)
  target_include_directories(ACTFWTGeoGeometryExample PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(ACTFWTGeoGeometryExample PRIVATE ActsCore)
  target_link_libraries(ACTFWTGeoGeometryExample PRIVATE ACTFramework ACTFWExamplesCommon)
  target_link_libraries(ACTFWTGeoGeometryExample PRIVATE ACTFWObjPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWTGeoDetector)
  target_link_libraries(ACTFWTGeoGeometryExample PRIVATE ${Boost_LIBRARIES})

  install(TARGETS ACTFWTGeoGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# DD4hep detector
if (USE_DD4HEP)
  add_executable(ACTFWDD4hepGeometryExample src/DD4hepGeometryExample.cpp)
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${DD4hep_INCLUDE_DIRS})
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})  
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ActsCore)
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ACTFramework ACTFWExamplesCommon)
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ACTFWCsvPlugin ACTFWObjPlugin ACTFWJsonPlugin ACTFWDD4hepDetector)
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ${DD4hep_LIBRARIES})
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ${Boost_LIBRARIES})

  install(TARGETS ACTFWDD4hepGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
