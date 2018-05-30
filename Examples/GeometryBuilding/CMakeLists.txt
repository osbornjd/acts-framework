# generic detector
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample)
target_include_directories(ACTFWGenericGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(ACTFWGenericGeometryExample PUBLIC ${DD4hep_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericGeometryExample PUBLIC Acts::Core)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ACTFramework ACTFWObjPlugin ACTFWGenericDetector)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWGenericGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# DD4hep detector
if (USE_DD4HEP)
  add_executable(ACTFWDD4hepGeometryExample src/DD4hepGeometryExample.cpp)
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${DD4hep_INCLUDE_DIRS})
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${Boost_INCLUDE_DIRS})  
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC Acts::Core)
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ACTFWDD4hepPlugin ACTFramework ACTFWObjPlugin)
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC ${DD4hep_LIBRARIES})
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ${Boost_LIBRARIES})

  install(TARGETS ACTFWDD4hepGeometryExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
