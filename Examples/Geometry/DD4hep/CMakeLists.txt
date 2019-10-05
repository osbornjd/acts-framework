add_executable(ACTFWDD4hepGeometryExample DD4hepGeometryExample.cpp)
target_include_directories(
  ACTFWDD4hepGeometryExample
  PRIVATE ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWDD4hepGeometryExample
  PRIVATE ${_common_libraries} ACTFWDD4hepDetector ${DD4hep_LIBRARIES})

install(
  TARGETS ACTFWDD4hepGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
