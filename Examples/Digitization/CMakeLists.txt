#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define library target
add_library(ACTFWDigitization SHARED ${src_files})
# define executable 
#add_executable(ACTFWDigitizationExample bin/DigitizationExample.cpp)

# setup include directories for ACTFWDigitization library
target_include_directories(ACTFWDigitization PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWDigitization PUBLIC ACTFWRootPlugins)
target_link_libraries(ACTFWDigitization PUBLIC ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWDigitization PUBLIC ACTFramework ACTFWRootPlugins)

# setup include directories for ACTFWDigitizationExample
#target_link_libraries(ACTFWDigitizationExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
#target_link_libraries(ACTFWDigitizationExample PRIVATE ACTFramework ACTFWRootPlugins)
#target_link_libraries(ACTFWDigitizationExample PRIVATE ACTFWDigitization)

# set installation directories
install(TARGETS ACTFWDigitization LIBRARY DESTINATION lib)
#install(TARGETS ACTFWDigitizationExample RUNTIME DESTINATION bin)
install(DIRECTORY include/ACTFW DESTINATION include)

