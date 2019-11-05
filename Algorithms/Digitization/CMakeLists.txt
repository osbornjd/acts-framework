add_library(
  ACTFWDigitization SHARED
  src/DigitizationAlgorithm.cpp)
target_include_directories(
  ACTFWDigitization
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWDigitization
  PRIVATE ACTFramework ActsCore ActsDigitizationPlugin IdentificationPlugin)

install(
  TARGETS ACTFWDigitization
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
