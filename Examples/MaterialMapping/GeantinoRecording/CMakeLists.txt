include(${Geant4_USE_FILE})

add_executable(
  ACTFWGeantinoRecordingExample
  GeantinoRecordingExample.cpp)
target_include_directories(
  ACTFWGeantinoRecordingExample
  PRIVATE ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWGeantinoRecordingExample
  PRIVATE ${_common_libraries} ACTFWMaterialMapping ACTFWDD4hepG4Plugin
    ACTFWRootPlugin ACTFWDD4hepDetector ${DD4hep_LIBRARIES} ${Geant4_LIBRARIES})

install(
  TARGETS ACTFWGeantinoRecordingExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
