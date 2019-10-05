set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWExamplesCommon
  ACTFWObjPlugin
  ACTFWCsvPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  Boost::program_options)

# Empty detector
add_executable(ACTFWEmptyGeometryExample src/EmptyGeometryExample.cpp)
target_include_directories(ACTFWEmptyGeometryExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWEmptyGeometryExample PRIVATE
  ${_common_libraries}
  ACTFWEmptyDetector)
install(
  TARGETS ACTFWEmptyGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector
add_executable(ACTFWGenericGeometryExample GenericGeometryExample.cpp)
target_link_libraries(
  ACTFWGenericGeometryExample
  PRIVATE ${_common_libraries} ACTFWGenericDetector)

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedGeometryExample AlignedGeometryExample.cpp)
target_link_libraries(
  ACTFWAlignedGeometryExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadGeometryExample PayloadGeometryExample.cpp)
target_link_libraries(
  ACTFWPayloadGeometryExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

install(
  TARGETS
    ACTFWGenericGeometryExample
    ACTFWAlignedGeometryExample
    ACTFWPayloadGeometryExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(DD4hep USE_DD4HEP)
add_subdirectory_if(TGeo USE_TGEO)
