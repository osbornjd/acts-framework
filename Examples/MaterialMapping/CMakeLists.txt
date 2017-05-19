# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp")

find_package(DD4hep COMPONENTS DDCore DDG4)
if (DD4hep_FOUND)

  # find the ACTS package
  find_package(ACTS COMPONENTS Core MaterialPlugin)
  find_package(Geant4 REQUIRED)

  add_executable(ACTFWGeantinoRecordingExample src/GeantinoRecordingExample.cpp)
  add_executable(ACTFWMaterialMappingExample src/MaterialMappingExample.cpp)

  set(ACTFWMaterialMappingExampleList ACTFWGeantinoRecordingExample ACTFWMaterialMappingExample)
  foreach (ACTFWMMExample ${ACTFWMaterialMappingExampleList})
  target_include_directories(${ACTFWMMExample} PRIVATE ${DD4hep_INCLUDE_DIRS}) 
  target_link_libraries(${ACTFWMMExample} PRIVATE ACTFWMaterialMapping ACTFWExtrapolation)
  target_link_libraries(${ACTFWMMExample} PRIVATE ACTFWDD4hepPlugin ACTFWDD4hepG4Plugin ACTFWRootPlugin)
  target_link_libraries(${ACTFWMMExample} PUBLIC ${DD4hep_LIBRARIES})
  install(TARGETS ${ACTFWMMExample} RUNTIME DESTINATION bin)
  endforeach(ACTFWMMExample)

else (DD4hep_FOUND)
  message (STATUS "${Red}ACTFWMaterialMapping: DD4hep was not found -> skip build of Plugin.${ColorReset}")
endif (DD4hep_FOUND)
