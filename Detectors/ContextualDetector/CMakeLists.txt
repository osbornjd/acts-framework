add_library(
  ACTFWContextualDetector SHARED
  src/AlignedDetector.cpp
  src/AlignmentDecorator.cpp
  src/PayloadDecorator.cpp
  src/PayloadDetector.cpp)
target_include_directories(
  ACTFWContextualDetector
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWContextualDetector
  PUBLIC ActsCore IdentificationPlugin ActsDigitizationPlugin ACTFramework
    ACTFWGenericDetector ACTFWBFieldPlugin ACTFWDetectorsCommon)

install(
  TARGETS ACTFWContextualDetector
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
