set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ActsFrameworkIoRoot
  ACTFWJsonPlugin
  ACTFWBFieldPlugin
  Boost::program_options)

add_executable(
  ACTFWGenericMaterialValidationExample
  GenericMaterialValidation.cpp)
target_link_libraries(
  ACTFWGenericMaterialValidationExample
  PRIVATE ${_common_libraries} ACTFWPropagation ACTFWGenericDetector)

add_executable(
  ACTFWGenericMaterialMappingExample
  GenericMaterialMapping.cpp)
target_link_libraries(
  ACTFWGenericMaterialMappingExample
  PRIVATE ${_common_libraries} ACTFWMaterialMapping ACTFWGenericDetector)

install(
  TARGETS
    ACTFWGenericMaterialValidationExample
    ACTFWGenericMaterialMappingExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(DD4hep USE_DD4HEP)
add_subdirectory_if(GeantinoRecording USE_GEANT4 AND USE_DD4HEP)
