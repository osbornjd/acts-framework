# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp")

# find the ACTS package
find_package(ACTS COMPONENTS Core MaterialPlugin)
find_package(Geant4 REQUIRED)
find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

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


