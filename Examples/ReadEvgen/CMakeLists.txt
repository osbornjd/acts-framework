file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWReadEvgenExample src/ReadEvgenExample.cpp)
target_include_directories(ACTFWReadEvgenExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWReadEvgenExample PRIVATE Acts::Core)
target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWReadEvgen ACTFWPluginPythia8)
target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTFWRootPlugin ACTFWCsvPlugin)
target_link_libraries(ACTFWReadEvgenExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWReadEvgenExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
