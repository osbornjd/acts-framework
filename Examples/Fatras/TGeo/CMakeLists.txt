add_executable(
  ActsSimFatrasTGeo
  TGeoFatrasExample.cpp)
target_link_libraries(
  ActsSimFatrasTGeo
  PRIVATE ActsFrameworkFatrasCommon ACTFWTGeoDetector)

install(
  TARGETS ActsSimFatrasTGeo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
