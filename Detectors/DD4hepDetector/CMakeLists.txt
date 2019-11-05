add_library(
  ACTFWDD4hepDetector SHARED
  src/Demonstrator/DemonstratorBarrel_geo.cpp
  src/Demonstrator/DemonstratorBeamPipe_geo.cpp
  src/FCChh/FCChhTrackerTkLayout_Barrel.cpp
  src/FCChh/FCChhTrackerTkLayout_Beampipe.cpp
  src/FCChh/FCChhTrackerTkLayout_Endcap.cpp
  src/IBL/IBLSimpleBarrel_geo.cpp
  src/OpenDataDetector/ODDCylinder_geo.cpp
  src/OpenDataDetector/ODDPixelBarrel_geo.cpp
  src/OpenDataDetector/ODDPixelEndcap_geo.cpp
  src/OpenDataDetector/ODDStripBarrel_geo.cpp
  src/OpenDataDetector/ODDStripEndcap_geo.cpp
  src/TestTracker/AirTube_geo.cpp
  src/TestTracker/Barrel_geo.cpp
  src/TestTracker/BeamPipe_geo.cpp
  src/TestTracker/Endcap_geo.cpp
  src/TestTracker/SimpleLayeredCylinder_geo.cpp
  src/TestTracker/ZPlanarTracker_geo.cpp
  src/TKLayout/TKLayoutBarrel_geo.cpp
  src/TKLayout/TKLayoutEndcap_geo.cpp
  src/DD4hepDetector.cpp
  src/DD4hepGeometryService.cpp
  src/DetUtils.cpp)
target_include_directories(
  ACTFWDD4hepDetector
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> ${DD4hep_INCLUDE_DIRS})
target_link_libraries(
  ACTFWDD4hepDetector
  PUBLIC ACTFramework ActsCore ActsDigitizationPlugin ActsDD4hepPlugin
    ACTFWDetectorsCommon ROOT::Geom ROOT::GenVector ${DD4hep_LIBRARIES})

# not sure why this needs to be set, but dd4hep fails to compile otherwise
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(ACTFWDD4hepDetector PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
dd4hep_set_version(ACTFWDD4hepDetector MAJOR 1 MINOR 0 PATCH 0)
dd4hep_generate_rootmap(ACTFWDD4hepDetector)

install(
  TARGETS ACTFWDD4hepDetector
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
