add_library(
  ACTFWVertexing SHARED
  src/VertexFitAlgorithm.cpp
  src/VertexFindingAlgorithm.cpp)
target_include_directories(
  ACTFWVertexing
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(
  ACTFWVertexing
  PUBLIC ActsCore ACTFramework
  PRIVATE ActsFrameworkTruthTracking)

install(
  TARGETS ACTFWVertexing
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
