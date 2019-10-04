include(${Geant4_USE_FILE})

add_library(
  ACTFWGeant4Plugin SHARED
  src/MMDetectorConstruction.cpp
  src/MMEventAction.cpp
  src/MMPrimaryGeneratorAction.cpp
  src/MMRunAction.cpp
  src/MMSteppingAction.cpp)
target_include_directories(
  ACTFWGeant4Plugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    ${Geant4_INCLUDE_DIRS})
target_link_libraries(
  ACTFWGeant4Plugin
  PUBLIC ActsCore ACTFramework ${Geant4_LIBRARIES}
  PRIVATE ROOT::Geom)

install(
  TARGETS ACTFWGeant4Plugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
