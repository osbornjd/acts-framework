add_library(ACTFWPluginPythia8 SHARED src/Generator.cpp)
target_include_directories(ACTFWPluginPythia8 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWPluginPythia8 PUBLIC ${Pythia8_INCLUDE_DIR})
target_link_libraries(ACTFWPluginPythia8 PRIVATE ACTFramework ActsCore)
target_link_libraries(ACTFWPluginPythia8 PRIVATE Pythia8)

install(TARGETS ACTFWPluginPythia8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
