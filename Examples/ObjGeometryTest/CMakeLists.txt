# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)
find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

# define library target
add_executable(ACTFWGeometryObjTest ${src_files})

# setup linker dependencies
target_include_directories(ACTFWGeometryObjTest PUBLIC ${DD4hep_INCLUDE_DIRS})  
target_link_libraries(ACTFWGeometryObjTest PUBLIC ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWGeometryObjTest PRIVATE ACTFramework ACTFWDD4hepPlugin ACTFWObjPlugins)
target_link_libraries(ACTFWGeometryObjTest PUBLIC ${DD4hep_LIBRARIES})

# set installation directories
install(TARGETS ACTFWGeometryObjTest RUNTIME DESTINATION bin)
