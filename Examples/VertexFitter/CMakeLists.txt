file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWVertexFitterExample ${src_files})
target_include_directories(ACTFWVertexFitterExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexFitterExample PRIVATE ActsCore)
target_link_libraries(ACTFWVertexFitterExample PRIVATE ACTFramework ACTFWExamplesCommon ActsFrameworkGenerators ActsFrameworkPythia8 ActsFrameworkSimulation)
target_link_libraries(ACTFWVertexFitterExample PRIVATE ACTFWRootPlugin ACTFWCsvPlugin)
target_link_libraries(ACTFWVertexFitterExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWVertexFitterExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
