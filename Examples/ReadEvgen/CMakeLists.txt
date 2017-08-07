file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWReadEvgenExample src/ReadEvgenExample.cpp)
target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTS::ACTSCore)
target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTFramework ACTFWReadEvgen ACTFWPluginPythia8 ACTFWRootPlugin)

install(TARGETS ACTFWReadEvgenExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
