add_executable(ACTFWTGeoGeometryExample TGeoGeometryExample.cpp)
target_link_libraries(
  ACTFWTGeoGeometryExample
  PRIVATE ${_common_libraries} ACTFWTGeoDetector)

install(
  TARGETS ACTFWTGeoGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
