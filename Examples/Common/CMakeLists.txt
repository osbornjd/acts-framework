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
    ActsCore
    ACTFramework
    ACTFWBFieldPlugin ACTFWDetectorsCommon
    ACTFWPropagation
    ACTFWMaterialMapping
    ActsFrameworkIoCsv ACTFWJsonPlugin ActsFrameworkIoRoot ACTFWObjPlugin)

install(
  TARGETS ACTFWExamplesCommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
