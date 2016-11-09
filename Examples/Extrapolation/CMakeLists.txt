#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package

find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define library target
add_library(ACTFWExtrapolation SHARED ${src_files})
# define executable 
add_executable(ACTFWExtrapolationExample bin/ExtrapolationTest.cpp)

# setup include directories for ACTFWExtrapolation library
target_include_directories(ACTFWExtrapolation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWExtrapolation PUBLIC ACTFWRootPlugins)
target_link_libraries(ACTFWExtrapolation PUBLIC ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWExtrapolation PUBLIC ACTFramework ACTFWRootPlugins)

# setup include directories for ACTFWExtrapolationExample
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTFramework ACTFWRootPlugins)
target_link_libraries(ACTFWExtrapolationExample PRIVATE ACTFWExtrapolation)

# set installation directories
install(TARGETS ACTFWExtrapolation LIBRARY DESTINATION lib)
install(TARGETS ACTFWExtrapolationExample RUNTIME DESTINATION bin)
install(DIRECTORY include/ACTFW/Extrapolation DESTINATION include)

