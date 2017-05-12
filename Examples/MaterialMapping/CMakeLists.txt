# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS COMPONENTS Core MaterialPlugin)
find_package(Geant4 REQUIRED)
find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

# define library target
#add_executable(ACTFWMaterialMappingTest ${src_files})

# setup linker dependencies

# define library target
add_library(ACTFWMaterialMapping SHARED ${src_files})
target_include_directories(ACTFWMaterialMapping PUBLIC ${DD4hep_INCLUDE_DIRS})  
target_link_libraries(ACTFWMaterialMapping PUBLIC ${DD4hep_LIBRARIES} ACTFWDD4hepG4)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFramework ACTFWRootMaterialMapping ACTFWGeant4MaterialMapping ACTFWDD4hepG4 ACTFWDD4hepPlugin)
include(${Geant4_USE_FILE})
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWMaterialMapping PUBLIC ${DD4hep_LIBRARIES})
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFWExtrapolation)

add_executable(ACTFWGeantinoRecordingTest bin/GeantinoRecordingTest.cpp)
add_executable(ACTFWMaterialMappingTest bin/MaterialMappingTest.cpp)

set(ACTFWMaterialMappingTestList ACTFWGeantinoRecordingTest ACTFWMaterialMappingTest)
foreach (ACTFWMaterialMappingTests ${ACTFWMaterialMappingTestList})
target_include_directories(${ACTFWMaterialMappingTests} PRIVATE ${DD4hep_INCLUDE_DIRS}) 
target_link_libraries(${ACTFWMaterialMappingTests} PRIVATE ACTFWMaterialMapping)
install(TARGETS ${ACTFWMaterialMappingTests} RUNTIME DESTINATION bin)
endforeach(ACTFWMaterialMappingTests)
