set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ActsFrameworkGenerators
  ActsFrameworkPythia8
  ActsFrameworkTruthTracking
  ACTFWCsvPlugin
  ACTFWRootPlugin
  ${Boost_LIBRARIES})

# Fitter example
add_executable(ACTFWVertexFitterExample src/VertexFitterExample.cpp src/VertexFitAlgorithm.cpp)
target_include_directories(ACTFWVertexFitterExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexFitterExample PRIVATE
  ${_common_libraries})
install(
  TARGETS ACTFWVertexFitterExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


# Finder example
add_executable(ACTFWVertexFinderExample src/VertexFinderExample.cpp src/VertexFindingAlgorithm.cpp)
target_include_directories(ACTFWVertexFinderExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexFinderExample PRIVATE
  ${_common_libraries})
install(
  TARGETS ACTFWVertexFinderExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Writer example
add_executable(ACTFWVertexWriterExample src/VertexAndTrackWriterExample.cpp)
target_include_directories(ACTFWVertexWriterExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexWriterExample PRIVATE
  ${_common_libraries})
install(
  TARGETS ACTFWVertexWriterExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Reader example
add_executable(ACTFWVertexReaderExample src/VertexAndTracksReaderAndFinderExample.cpp src/VertexFindingAlgorithm.cpp)
target_include_directories(ACTFWVertexReaderExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexReaderExample PRIVATE
  ${_common_libraries})
install(
  TARGETS ACTFWVertexReaderExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
