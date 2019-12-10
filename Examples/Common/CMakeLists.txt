add_library(
  ACTFWExamplesCommon SHARED
  src/CommonGeometry.cpp
  src/CommonOptions.cpp
  src/GeometryExampleBase.cpp
  src/MaterialMappingBase.cpp
  src/MaterialValidationBase.cpp
  src/PropagationExampleBase.cpp)
target_include_directories(
  ACTFWExamplesCommon
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWExamplesCommon
  PUBLIC
    ActsCore FatrasCore ACTFramework ACTFWObjPlugin ActsFrameworkIoCsv
    ACTFWJsonPlugin ActsFrameworkIoRoot ACTFWDetectorsCommon ACTFWBFieldPlugin
    ACTFWDigitization ACTFWPropagation ACTFWFatras ActsFrameworkGenerators
    ACTFWMaterialMapping ACTFWFitting)

if(USE_PYTHIA8)
  target_sources(
    ACTFWExamplesCommon
    PRIVATE
      src/FatrasDigitizationBase.cpp
      src/FatrasEvgenBase.cpp
      src/FatrasExampleBase.cpp
      src/FatrasSimulationBase.cpp)
  target_link_libraries(
    ACTFWExamplesCommon
    PUBLIC ActsFrameworkPythia8)
endif()

install(
  TARGETS ACTFWExamplesCommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
