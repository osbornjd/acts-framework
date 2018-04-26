file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWReadHepMCExample src/ReadHepMCExample.cpp)
target_include_directories(ACTFWReadHepMCExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWReadHepMCExample PRIVATE ACTSCore)
target_link_libraries(ACTFWReadHepMCExample PRIVATE ACTFramework ACTFWPluginHepMCReader)
target_link_libraries(ACTFWReadHepMCExample PRIVATE ${Boost_LIBRARIES})
target_link_libraries(ACTFWReadHepMCExample PRIVATE HepMC)

install(TARGETS ACTFWReadHepMCExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
