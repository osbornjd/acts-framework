add_library(
  ActsFrameworkIoRoot SHARED
  src/RootMaterialDecorator.cpp
  src/RootMaterialWriter.cpp
  src/RootMaterialTrackReader.cpp
  src/RootMaterialTrackWriter.cpp
  src/RootPlanarClusterWriter.cpp
  src/RootParticleWriter.cpp
  src/RootPropagationStepsWriter.cpp
  src/RootSimHitWriter.cpp
  src/RootTrackParameterWriter.cpp
  src/RootVertexAndTracksWriter.cpp
  src/RootVertexAndTracksReader.cpp
  src/RootTrajectoryWriter.cpp)
target_include_directories(
  ActsFrameworkIoRoot
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ActsFrameworkIoRoot
  PUBLIC
    ActsCore ActsDigitizationPlugin ActsIdentificationPlugin ACTFramework
    ACTFWPropagation ActsFrameworkTruthTracking Threads::Threads
  PRIVATE ROOT::Core ROOT::Hist ROOT::Tree)

install(
  TARGETS ActsFrameworkIoRoot
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
