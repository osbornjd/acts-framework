set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  Boost::program_options)

set(_plugin_libraries
  ACTFWRootPlugin
  ACTFWJsonPlugin
  ACTFWBFieldPlugin)

add_executable(ACTFWGenericMaterialValidationExample src/GenericMaterialValidation.cpp)
target_link_libraries(ACTFWGenericMaterialValidationExample PRIVATE 
  ${_common_libraries} ${_plugin_libraries})
target_link_libraries(ACTFWGenericMaterialValidationExample PUBLIC 
  ACTFWPropagation ACTFWGenericDetector)
install(TARGETS ACTFWGenericMaterialValidationExample RUNTIME DESTINATION 
  ${CMAKE_INSTALL_BINDIR})

add_executable(ACTFWGenericMaterialMappingExample src/GenericMaterialMapping.cpp)
target_link_libraries(ACTFWGenericMaterialMappingExample PRIVATE 
  ${_common_libraries} ${_plugin_libraries})
target_link_libraries(ACTFWGenericMaterialMappingExample PRIVATE 
  ACTFWMaterialMapping ACTFWGenericDetector)
install(TARGETS ACTFWGenericMaterialMappingExample RUNTIME DESTINATION 
  ${CMAKE_INSTALL_BINDIR})


if (USE_GEANT4 AND USE_DD4HEP)
  include(${Geant4_USE_FILE})
  
  add_executable(ACTFWGeantinoRecordingExample src/GeantinoRecordingExample.cpp)
  target_include_directories(ACTFWGeantinoRecordingExample PRIVATE 
    ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(ACTFWGeantinoRecordingExample PRIVATE 
   ACTFWExamplesCommon ACTFWMaterialMapping ACTFWDD4hepG4Plugin ACTFWRootPlugin)
  target_link_libraries(ACTFWGeantinoRecordingExample PRIVATE 
   ACTFWDD4hepDetector ${DD4hep_LIBRARIES} ${Geant4_LIBRARIES})
   
  install(TARGETS ACTFWGeantinoRecordingExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# DD4hep detector
if (USE_DD4HEP)  
  add_executable(ACTFWDD4hepMaterialValidationExample src/DD4hepMaterialValidation.cpp)
  target_link_libraries(ACTFWDD4hepMaterialValidationExample PRIVATE 
    ${_common_libraries} ${_plugin_libraries})
  target_link_libraries(ACTFWDD4hepMaterialValidationExample PRIVATE 
    ACTFWPropagation ACTFWDD4hepDetector)
  install(TARGETS ACTFWDD4hepMaterialValidationExample RUNTIME DESTINATION 
    ${CMAKE_INSTALL_BINDIR})
  
  add_executable(ACTFWDD4hepMaterialMappingExample src/DD4hepMaterialMapping.cpp)
  target_link_libraries(ACTFWDD4hepMaterialMappingExample PRIVATE 
    ${_common_libraries} ${_plugin_libraries})
  target_link_libraries(ACTFWDD4hepMaterialMappingExample PRIVATE 
    ACTFWMaterialMapping ACTFWDD4hepDetector)
  install(TARGETS ACTFWDD4hepMaterialMappingExample RUNTIME DESTINATION 
    ${CMAKE_INSTALL_BINDIR})
endif()

