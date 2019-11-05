add_executable(ACTFWDD4hepPropagationExample DD4hepPropagationExample.cpp)
target_include_directories(
  ACTFWDD4hepPropagationExample
  PRIVATE ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWDD4hepPropagationExample
  PRIVATE ${_common_libraries} ACTFWDD4hepDetector ${DD4hep_LIBRARIES})

install(
  TARGETS ACTFWDD4hepPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
