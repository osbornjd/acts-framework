# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS)

# define library target
add_executable(ACTFWExtrapolationExample ${src_files})

# setup include directories
target_include_directories(ACTFWExtrapolationExample PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWExtrapolationExample PUBLIC ACTFWRootPlugins)
target_link_libraries(ACTFWExtrapolationExample PUBLIC ACTSCore ACTSExamples)
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTFramework ACTFWRootPlugins)

# set installation directories
install(TARGETS ACTFWExtrapolationExample RUNTIME DESTINATION bin)





