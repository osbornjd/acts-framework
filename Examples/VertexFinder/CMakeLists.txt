file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWVertexFinderExample ${src_files})
target_include_directories(ACTFWVertexFinderExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexFinderExample PRIVATE ActsCore)
target_link_libraries(ACTFWVertexFinderExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWReadEvgen ACTFWPluginPythia8)
target_link_libraries(ACTFWVertexFinderExample PRIVATE ACTFWRootPlugin ACTFWCsvPlugin)
target_link_libraries(ACTFWVertexFinderExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWVertexFinderExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
