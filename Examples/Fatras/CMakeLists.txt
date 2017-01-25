# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define library target
add_library(ACTFWFatras SHARED ${src_files})
# define executable 
add_executable(ACTFWFatrasExample bin/FatrasExample.cpp)

# setup include directories for FATRAS library
target_include_directories(ACTFWFatras PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWFatras PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWFatras PUBLIC ACTFramework)

# setup include directories for FatrasExample
target_include_directories(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_include_directories(ACTFWFatrasExample PRIVATE ACTFramework ACTFWRootPythia8Plugins ACTFWCsvPlugins ACTFWRootPlugins ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFramework ACTFWCsvPlugins ACTFWRootPlugins ACTFWRootPythia8Plugins ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFWFatras)

# set installation directories
install(TARGETS ACTFWFatras LIBRARY DESTINATION lib)
install(TARGETS ACTFWFatrasExample RUNTIME DESTINATION bin)
install(DIRECTORY include/ACTFW DESTINATION include)
