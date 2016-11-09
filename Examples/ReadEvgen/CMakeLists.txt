# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# define library target
add_library(ACTFWReadEvgen SHARED ${src_files})
# define executable 
#add_executable(ACTFWExtrapolationTest bin/ExtrapolationTest.cpp)

# setup include directories for ExtrapolationExample
target_include_directories(ACTFWReadEvgen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWReadEvgen PUBLIC ACTFWRootPlugins)
target_link_libraries(ACTFWReadEvgen PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWReadEvgen PUBLIC ACTFramework ACTFWRootPlugins)

# setup include directories for ExtrapolationTest
#target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
#target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTFramework ACTFWRootPlugins)
#target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTFWReadEvgen)

# set installation directories
install(TARGETS ACTFWReadEvgen LIBRARY DESTINATION lib)
#install(TARGETS ACTFWExtrapolationTest RUNTIME DESTINATION bin)
#install(DIRECTORY include/ACTFW/ReadEvgen DESTINATION include)