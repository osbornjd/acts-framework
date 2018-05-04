file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWHepMC3Example src/HepMC3Example.cpp)
target_include_directories(ACTFWHepMC3Example PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWHepMC3Example PRIVATE ACTSCore)
target_link_libraries(ACTFWHepMC3Example PRIVATE ACTFramework ACTFWPluginHepMC3)
target_link_libraries(ACTFWHepMC3Example PRIVATE ${Boost_LIBRARIES})
target_link_libraries(ACTFWHepMC3Example PRIVATE HepMC3Lib)
target_link_libraries(ACTFWHepMC3Example PRIVATE HepPID)

install(TARGETS ACTFWHepMC3Example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
