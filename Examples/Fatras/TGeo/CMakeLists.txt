add_executable(ACTFWTGeoFatrasExample TGeoFatrasExample.cpp)
target_link_libraries(
  ACTFWTGeoFatrasExample
  PRIVATE ${_common_libraries} ACTFWTGeoDetector)

install(
  TARGETS ACTFWTGeoFatrasExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
