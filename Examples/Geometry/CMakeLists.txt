set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ACTFWObjPlugin
  ACTFWCsvPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  ${Boost_LIBRARIES})

# Generic detector
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample.cpp)
target_include_directories(ACTFWGenericGeometryExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWGenericDetector)
install(
  TARGETS ACTFWGenericGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedGeometryExample src/AlignedGeometryExample.cpp)
target_include_directories(ACTFWAlignedGeometryExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWAlignedGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadGeometryExample src/PayloadGeometryExample.cpp)
target_include_directories(ACTFWPayloadGeometryExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWPayloadGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# TGEO based detector
if (USE_TGEO)
  add_executable(ACTFWTGeoGeometryExample src/TGeoGeometryExample.cpp)
  target_include_directories(ACTFWTGeoGeometryExample PRIVATE
    ${Boost_INCLUDE_DIRS})
  target_link_libraries(ACTFWTGeoGeometryExample PRIVATE
    ${_common_libraries}
    ACTFWTGeoDetector)
  install(
    TARGETS ACTFWTGeoGeometryExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# DD4hep detector
if (USE_DD4HEP)
  add_executable(ACTFWDD4hepGeometryExample src/DD4hepGeometryExample.cpp)
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE
    ${_common_libraries}
    ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})
  install(
    TARGETS ACTFWDD4hepGeometryExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
