# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core TGeoPlugin)

# define library target
add_executable(ACTFWGdmlDetectorExample ${src_files})

# setup include directories
target_include_directories(ACTFWGdmlDetectorExample PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWGdmlDetectorExample PUBLIC ACTS::ACTSCore ACTS::ACTSTGeoPlugin  ACTSGeant4Plugin)

#target_include_directories(ACTFWGdmlDetectorExample PUBLIC ACTFWRootPlugins)
#target_link_libraries(ACTFWGdmlDetectorExample PRIVATE ACTFramework ACTFWRootPlugins)

# set installation directories
install(TARGETS ACTFWGdmlDetectorExample RUNTIME DESTINATION bin)





