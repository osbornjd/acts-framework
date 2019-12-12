set(_common_libraries
  ActsCore
  FatrasCore
  ACTFramework
  ACTFWBFieldPlugin
  ActsFrameworkIoCsv
  ACTFWDigitization
  ACTFWExamplesCommon
  ACTFWFatras
  ACTFWFitting
  ACTFWObjPlugin
  ACTFWRootPlugin
  ActsFrameworkGenerators
  ActsFrameworkPythia8
  Boost::program_options)

# Generic detector
add_executable(ACTFWGenericFatrasExample GenericFatrasExample.cpp)
target_link_libraries(
  ACTFWGenericFatrasExample
  PRIVATE ${_common_libraries} ACTFWGenericDetector)

# Generic detector with IOV based alignment
add_executable(ACTFWAlignedFatrasExample AlignedFatrasExample.cpp)
target_link_libraries(
  ACTFWAlignedFatrasExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

# Generic detector with Payload based alignment
add_executable(ACTFWPayloadFatrasExample PayloadFatrasExample.cpp)
target_link_libraries(
  ACTFWPayloadFatrasExample
  PRIVATE ${_common_libraries} ACTFWContextualDetector)

install(
  TARGETS
    ACTFWGenericFatrasExample
    ACTFWAlignedFatrasExample
    ACTFWPayloadFatrasExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(DD4hep USE_DD4HEP)
add_subdirectory_if(TGeo USE_TGEO)
