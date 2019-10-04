set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ACTFWObjPlugin
  ACTFWCsvPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  Boost::program_options)

# Empty detector
add_executable(ACTFWEmptyGeometryExample src/EmptyGeometryExample.cpp)
target_include_directories(ACTFWEmptyGeometryExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWEmptyGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWEmptyDetector)
install(
  TARGETS ACTFWEmptyGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample.cpp)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWGenericDetector)

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedGeometryExample src/AlignedGeometryExample.cpp)
target_link_libraries(ACTFWAlignedGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadGeometryExample src/PayloadGeometryExample.cpp)
target_link_libraries(ACTFWPayloadGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)

install(
  TARGETS
    ACTFWGenericGeometryExample
    ACTFWAlignedGeometryExample
    ACTFWPayloadGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# TGEO based detector
if (USE_TGEO)
  add_executable(ACTFWTGeoGeometryExample src/TGeoGeometryExample.cpp)
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
  ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE
    ${_common_libraries}
    ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})
  install(
    TARGETS ACTFWDD4hepGeometryExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
