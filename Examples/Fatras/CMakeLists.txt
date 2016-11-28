# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define executable 
add_executable(ACTFWFatrasExample bin/FatrasExample.cpp)

# setup include directories for ExtrapolationTest
target_include_directories(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_include_directories(ACTFWFatrasExample PRIVATE ACTFramework ACTFWRootPythia8Plugins ACTFWRootPlugins ACTWReadEvgen ACTFWExtrapolation)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFramework ACTFWRootPlugins  ACTFWRootPythia8Plugins ACTFWReadEvgen ACTFWExtrapolation)
#target_link_libraries(ACTFWFatrasExample PRIVATE ACTFWFatras)

# set installation directories
install(TARGETS ACTFWFatrasExample RUNTIME DESTINATION bin)
#install(DIRECTORY include/ACTFW/ReadEvgen DESTINATION include)
