add_library(
  ACTFWFitting SHARED
  src/TrkrClusterFittingAlgorithmFitterFunction.cpp
  src/FittingAlgorithm.cpp
  src/FittingAlgorithmFitterFunction.cpp)
target_include_directories(
  ACTFWFitting
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(
  ACTFWFitting
  PUBLIC ActsCore ACTFramework ACTFWBFieldPlugin Boost::program_options)

install(
  TARGETS ACTFWFitting
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
