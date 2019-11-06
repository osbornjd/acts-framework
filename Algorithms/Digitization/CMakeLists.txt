add_library(
  ACTFWDigitization SHARED
  src/DigitizationAlgorithm.cpp
  src/DigitizationOptions.cpp
  src/HitSmearing.cpp)
target_include_directories(
  ACTFWDigitization
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWDigitization
  PRIVATE ACTFramework ActsCore ActsDigitizationPlugin IdentificationPlugin Boost::program_options)

install(
  TARGETS ACTFWDigitization
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
