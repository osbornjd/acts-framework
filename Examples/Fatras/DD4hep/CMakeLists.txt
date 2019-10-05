add_executable(ACTFWDD4hepFatrasExample DD4hepFatrasExample.cpp)
target_include_directories(
  ACTFWDD4hepFatrasExample
  PRIVATE ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWDD4hepFatrasExample
  PRIVATE ${_common_libraries} ACTFWDD4hepDetector ${DD4hep_LIBRARIES})

install(
  TARGETS ACTFWDD4hepFatrasExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
