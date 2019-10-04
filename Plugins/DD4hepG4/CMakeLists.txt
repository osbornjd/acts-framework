include(${Geant4_USE_FILE})

add_library(
  ACTFWDD4hepG4Plugin SHARED
  src/DD4hepToG4Svc.cpp
  src/GeoConstruction.cpp)
target_include_directories(
  ACTFWDD4hepG4Plugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWDD4hepG4Plugin
  PUBLIC ACTFramework ACTFWDD4hepDetector ${DD4hep_LIBRARIES}
    ${DD4hep_DDG4_LIBRARY} ${Geant4_LIBRARIES})

install(
  TARGETS ACTFWDD4hepG4Plugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
