#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define executable 
add_executable(ACTFWExtrapolationExample src/ExtrapolationExample.cpp)

# setup include directories for ACTFWExtrapolationExample
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWExtrapolation)

# set installation directories
install(TARGETS ACTFWExtrapolationExample RUNTIME DESTINATION bin)

