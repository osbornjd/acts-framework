set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ActsFrameworkGenerators
  ActsFrameworkPythia8
  ActsFrameworkTruthTracking
  ACTFWVertexing
  ACTFWCsvPlugin
  ACTFWRootPlugin
  Boost::program_options)

# Fitter example
add_executable(ACTFWVertexFitterExample src/VertexFitterExample.cpp)
target_include_directories(
  ACTFWVertexFitterExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWVertexFitterExample
  PRIVATE ${_common_libraries})

# Finder example
add_executable(ACTFWVertexFinderExample src/VertexFinderExample.cpp)
target_include_directories(
  ACTFWVertexFinderExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWVertexFinderExample
  PRIVATE ${_common_libraries})

# Writer example
add_executable(ACTFWVertexWriterExample src/VertexAndTracksWriterExample.cpp)
target_include_directories(ACTFWVertexWriterExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexWriterExample
  PRIVATE ${_common_libraries})

# Reader example
add_executable(ACTFWVertexReaderExample src/VertexAndTracksReaderAndFinderExample.cpp)
target_include_directories(ACTFWVertexReaderExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWVertexReaderExample
  PRIVATE ${_common_libraries})

install(
  TARGETS
    ACTFWVertexFitterExample
    ACTFWVertexFinderExample
    ACTFWVertexWriterExample
    ACTFWVertexReaderExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
