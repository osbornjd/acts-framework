add_library(ACTFWMaterialMapping SHARED src/MaterialMapping.cpp)
target_include_directories(
  ACTFWMaterialMapping
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(
  ACTFWMaterialMapping
  PUBLIC ActsCore ACTFramework)

if(USE_DD4HEP AND USE_GEANT4)
  include(${Geant4_USE_FILE})
  target_sources(ACTFWMaterialMapping PRIVATE src/GeantinoRecording.cpp)
  target_link_libraries(
    ACTFWMaterialMapping
    PUBLIC ACTFWGeant4Plugin ACTFWDD4hepG4Plugin)
endif()

install(
  TARGETS ACTFWMaterialMapping
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
