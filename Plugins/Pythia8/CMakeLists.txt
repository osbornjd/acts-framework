add_library(ACTFWPluginPythia8 SHARED ACTFW/Plugins/Pythia8/Generator.cpp)
target_include_directories(ACTFWPluginPythia8 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWPluginPythia8 PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWPluginPythia8 PUBLIC ACTSCore ACTFramework)
target_link_libraries(ACTFWPluginPythia8 PUBLIC ${ROOT_LIBRARIES} Pythia8)

install(TARGETS ACTFWPluginPythia8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
