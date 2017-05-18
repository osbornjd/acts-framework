# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define executable 
add_executable(ACTFWFatrasExample src/FatrasExample.cpp)

# setup include directories for FatrasExample
target_include_directories(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_include_directories(ACTFWFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
target_include_directories(ACTFWFatrasExample PRIVATE ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)

# set installation directories
install(TARGETS ACTFWFatrasExample RUNTIME DESTINATION bin)
