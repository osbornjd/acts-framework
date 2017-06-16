file (GLOB_RECURSE src_files "src/*.cpp")

add_executable(ACTFWGeantinoRecordingExample src/GeantinoRecordingExample.cpp)
add_executable(ACTFWMaterialMappingExample src/MaterialMappingExample.cpp)

set(ACTFWMaterialMappingExampleList ACTFWGeantinoRecordingExample ACTFWMaterialMappingExample)
foreach(ACTFWMMExample ${ACTFWMaterialMappingExampleList})
  target_include_directories(${ACTFWMMExample} PRIVATE ${DD4hep_INCLUDE_DIRS})
  target_link_libraries(${ACTFWMMExample} PRIVATE ACTFWMaterialMapping ACTFWExtrapolation)
  target_link_libraries(${ACTFWMMExample} PRIVATE ACTFWDD4hepPlugin ACTFWDD4hepG4Plugin ACTFWRootPlugin)
  target_link_libraries(${ACTFWMMExample} PUBLIC ${DD4hep_LIBRARIES})
  install(TARGETS ${ACTFWMMExample} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
