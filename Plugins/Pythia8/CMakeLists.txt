add_library(ActsFrameworkPythia8 SHARED
  ACTFW/Generators/Pythia8ProcessGenerator.cpp
  ACTFW/Options/Pythia8Options.cpp)

target_include_directories(ActsFrameworkPythia8
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_include_directories(ActsFrameworkPythia8 PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(ActsFrameworkPythia8 PUBLIC ActsCore ActsFrameworkGenerators ACTFramework Pythia8)
target_link_libraries(ActsFrameworkPythia8 PUBLIC ${Boost_LIBRARIES})

install(TARGETS ActsFrameworkGenerators LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
  DIRECTORY ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp")

add_library(ACTFWPluginPythia8 SHARED src/Generator.cpp)
target_include_directories(ACTFWPluginPythia8 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWPluginPythia8 PUBLIC ${Pythia8_INCLUDE_DIR})
target_link_libraries(ACTFWPluginPythia8 PRIVATE ACTFramework ActsCore)
target_link_libraries(ACTFWPluginPythia8 PRIVATE Pythia8)

install(TARGETS ACTFWPluginPythia8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
