file(GLOB_RECURSE src_files "src/MaterialMapping.cpp")
if(USE_DD4HEP AND USE_GEANT4)
  SET(src_files "src/GeantinoRecording.cpp" ${src_files})
  include(${Geant4_USE_FILE})
endif()

add_library(ACTFWMaterialMapping SHARED ${src_files})
target_include_directories(ACTFWMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(ACTFWMaterialMapping PUBLIC ActsCore)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFramework)
if(USE_DD4HEP AND USE_GEANT4)
  target_link_libraries(ACTFWMaterialMapping PRIVATE ACTFWGeant4Plugin ACTFWDD4hepG4Plugin)
endif()

install(
  TARGETS ACTFWMaterialMapping
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
