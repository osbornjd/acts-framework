set(_common_libraries
  ActsCore
  ACTFramework
  ACTFWPropagation
  ACTFWExamplesCommon
  ACTFWBFieldPlugin
  ACTFWObjPlugin
  ACTFWJsonPlugin
  ACTFWRootPlugin
  Boost::program_options)


# Generic detector
add_executable(ACTFWEmptyPropagationExample src/EmptyDetectorPropagationExample.cpp)
target_include_directories(ACTFWEmptyPropagationExample PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWEmptyPropagationExample PRIVATE
  ${_common_libraries}
  ACTFWEmptyDetector)
install(
  TARGETS ACTFWEmptyPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Generic detector
add_executable(ACTFWGenericPropagationExample GenericPropagationExample.cpp)
target_link_libraries(ACTFWGenericPropagationExample
  PRIVATE ${_common_libraries} ACTFWGenericDetector)

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedPropagationExample AlignedPropagationExample.cpp)
target_link_libraries(ACTFWAlignedPropagationExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadPropagationExample PayloadPropagationExample.cpp)
target_link_libraries(
  ACTFWPayloadPropagationExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

install(
  TARGETS
    ACTFWGenericPropagationExample
    ACTFWAlignedPropagationExample
    ACTFWPayloadPropagationExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(DD4hep USE_DD4HEP)
add_subdirectory_if(TGeo USE_TGEO)
