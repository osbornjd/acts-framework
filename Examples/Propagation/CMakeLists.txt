set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWPropagation
  ACTFWExamplesCommon
  ACTFWBFieldPlugin
  ACTFWObjPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  ${Boost_LIBRARIES})

# Generic detector
add_executable(ACTFWGenericPropagationExample src/GenericPropagationExample.cpp)
target_include_directories(ACTFWGenericPropagationExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWGenericDetector)
install(
  TARGETS ACTFWGenericPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedPropagationExample src/AlignedPropagationExample.cpp)
target_include_directories(ACTFWAlignedPropagationExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWAlignedPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWAlignedPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadPropagationExample src/PayloadPropagationExample.cpp)
target_include_directories(ACTFWPayloadPropagationExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWPayloadPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWContextualDetector)
install(
  TARGETS ACTFWPayloadPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# TGEO based detector
if (USE_TGEO)
  add_executable(ACTFWTGeoPropagationExample src/TGeoPropagationExample.cpp)
  target_include_directories(ACTFWTGeoPropagationExample PRIVATE
    ${Boost_INCLUDE_DIRS})
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
    ${Boost_INCLUDE_DIRS}
    ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(ACTFWDD4hepPropagationExample PRIVATE
    ${_common_libraries}
    ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})
  install(
    TARGETS ACTFWDD4hepPropagationExample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
