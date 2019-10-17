set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWPropagation
  ACTFWExamplesCommon
  ACTFWBFieldPlugin
  ACTFWObjPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  Boost::program_options)


# Generic detector
add_executable(ACTFWEmptyDetectorPropagationExample src/EmptyDetectorPropagationExample.cpp)
target_include_directories(ACTFWEmptyDetectorPropagationExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWEmptyDetectorPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWEmptyDetector)
install(
  TARGETS ACTFWEmptyDetectorPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector
add_executable(ACTFWGenericPropagationExample src/GenericPropagationExample.cpp)
target_link_libraries(ACTFWGenericPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWGenericDetector)
install(
  TARGETS ACTFWGenericPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedPropagationExample src/AlignedPropagationExample.cpp)
target_link_libraries(ACTFWAlignedPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWAlignedPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadPropagationExample src/PayloadPropagationExample.cpp)
target_link_libraries(ACTFWPayloadPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWPayloadPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# TGEO based detector
if (USE_TGEO)
  add_executable(ACTFWTGeoPropagationExample src/TGeoPropagationExample.cpp)
  target_link_libraries(ACTFWTGeoPropagationExample PRIVATE
    ${_common_libraries}
    ACTFWTGeoDetector)
  install(
    TARGETS ACTFWTGeoPropagationExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# DD4hep detector
if (USE_DD4HEP)
  add_executable(ACTFWDD4hepPropagationExample src/DD4hepPropagationExample.cpp)
  target_include_directories(ACTFWDD4hepPropagationExample PRIVATE
  ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(ACTFWDD4hepPropagationExample PRIVATE
    ${_common_libraries}
    ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})
  install(
    TARGETS ACTFWDD4hepPropagationExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
