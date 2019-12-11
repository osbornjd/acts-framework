add_library(
  ACTFWJsonPlugin SHARED
  src/JsonMaterialWriter.cpp)
target_include_directories(
  ACTFWJsonPlugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWJsonPlugin
  PUBLIC ActsCore ActsJsonPlugin ACTFramework)

install(
  TARGETS ACTFWJsonPlugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
