# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define library target
add_library(ACTFWExtrapolationExample SHARED ${src_files})
# define executable 
add_executable(ACTFWExtrapolationTest bin/ExtrapolationTest.cpp)

# setup include directories for ExtrapolationExample
target_include_directories(ACTFWExtrapolationExample PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWExtrapolationExample PUBLIC ACTFWRootPlugins)
target_link_libraries(ACTFWExtrapolationExample PUBLIC ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWExtrapolationExample PUBLIC ACTFramework ACTFWRootPlugins)

# setup include directories for ExtrapolationTest
target_include_directories (ACTFWExtrapolationTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTFramework ACTFWRootPlugins)
target_link_libraries(ACTFWExtrapolationTest PRIVATE ACTFWExtrapolationExample)

# set installation directories
install(TARGETS ACTFWExtrapolationExample LIBRARY DESTINATION lib)
install(TARGETS ACTFWExtrapolationTest RUNTIME DESTINATION bin)
install(DIRECTORY include/ACTFW/ExtrapolationTest DESTINATION include)




