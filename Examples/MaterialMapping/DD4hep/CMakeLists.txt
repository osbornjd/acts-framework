add_executable(
  ACTFWDD4hepMaterialValidationExample
  DD4hepMaterialValidation.cpp)
target_link_libraries(
  ACTFWDD4hepMaterialValidationExample
  PRIVATE ${_common_libraries} ACTFWPropagation ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})

add_executable(
  ACTFWDD4hepMaterialMappingExample
  DD4hepMaterialMapping.cpp)
target_link_libraries(
  ACTFWDD4hepMaterialMappingExample
  PRIVATE ${_common_libraries} ACTFWMaterialMapping ACTFWDD4hepDetector
    ${DD4hep_LIBRARIES})

install(
  TARGETS
    ACTFWDD4hepMaterialValidationExample
    ACTFWDD4hepMaterialMappingExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
