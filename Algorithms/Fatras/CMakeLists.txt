add_library(ACTFWFatras SHARED src/FatrasOptions.cpp)

target_include_directories(
  ACTFWFatras 
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> 
  $<INSTALL_INTERFACE:include>)
target_link_libraries(
  ACTFWFatras 
  PUBLIC ActsCore FatrasCore ACTFramework)

# interface libraries do not exist in the filesystem; no installation needed
