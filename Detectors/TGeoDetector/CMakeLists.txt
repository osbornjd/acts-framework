add_library(
  ACTFWTGeoDetector SHARED
  src/TGeoDetector.cpp)
target_include_directories(
  ACTFWTGeoDetector
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWTGeoDetector
  PUBLIC ActsCore ActsIdentificationPlugin ActsDigitizationPlugin ACTFramework
    ACTFWGenericDetector ACTFWBFieldPlugin ACTFWDetectorsCommon ActsTGeoPlugin)

install(
  TARGETS ACTFWTGeoDetector
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
