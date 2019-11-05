add_library(
  ActsFrameworkTruthTracking SHARED
  ACTFW/TruthTracking/TrackSelector.cpp
  ACTFW/TruthTracking/TruthVerticesToTracks.cpp)
target_include_directories(
  ActsFrameworkTruthTracking
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  ActsFrameworkTruthTracking
  PUBLIC ActsCore ACTFramework Boost::program_options)

install(
  TARGETS ActsFrameworkTruthTracking
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
