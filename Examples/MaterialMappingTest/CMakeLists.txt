# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS COMPONENTS Core MaterialPlugin)
find_package(Geant4 REQUIRED)
find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

# define library target
add_executable(ACTFWMaterialMappingTest ${src_files})

# setup linker dependencies
target_include_directories(ACTFWMaterialMappingTest PRIVATE ${DD4hep_INCLUDE_DIRS})

target_link_libraries(ACTFWMaterialMappingTest PRIVATE ACTFramework ACTFWRootMaterialMapping ACTFWGeant4MaterialMapping ACTFWDD4hepG4 ACTFWDD4hepPlugin)
include(${Geant4_USE_FILE})
target_link_libraries(ACTFWMaterialMappingTest PRIVATE ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWMaterialMappingTest PRIVATE ${DD4hep_LIBRARIES})
target_link_libraries(ACTFWMaterialMappingTest PRIVATE ACTFWExtrapolation)


set(CMAKE_DEBUG_TARGET_PROPERTIES COMPILE_OPTIONS)
# set installation directories
install(TARGETS ACTFWMaterialMappingTest RUNTIME DESTINATION bin)

