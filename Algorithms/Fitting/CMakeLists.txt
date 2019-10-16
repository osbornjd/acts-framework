add_library(
  ACTFWFitting SHARED 
  src/FittingOptions.cpp 
  src/FittingAlgorithm.cpp)
target_include_directories(
  ACTFWFitting 
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> 
  $<INSTALL_INTERFACE:include>)
target_link_libraries(
  ACTFWFitting 
  PUBLIC ActsCore ACTFramework)

# interface libraries do not exist in the filesystem; no installation needed
