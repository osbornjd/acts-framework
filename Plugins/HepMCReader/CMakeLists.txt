file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWPluginHepMCReader SHARED ${src_files})
target_include_directories(ACTFWPluginHepMCReader PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWPluginHepMCReader PUBLIC ACTSCore ACTFramework)
target_link_libraries(ACTFWPluginHepMCReader PUBLIC HepMC)

install(TARGETS ACTFWPluginHepMCReader LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
