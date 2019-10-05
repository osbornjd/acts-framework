add_executable(ACTFWTGeoPropagationExample TGeoPropagationExample.cpp)
target_link_libraries(
  ACTFWTGeoPropagationExample
  PRIVATE ${_common_libraries} ACTFWTGeoDetector)

install(
  TARGETS ACTFWTGeoPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
