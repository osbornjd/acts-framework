# Common library that implements the core functionality
add_subdirectory(Common)

# Generic detector
add_executable(
  ActsSimFatrasGeneric
  GenericFatrasExample.cpp)
target_link_libraries(
  ActsSimFatrasGeneric
  PRIVATE ActsFrameworkFatrasCommon ACTFWGenericDetector)

# Generic detector with IOV based alignment
add_executable(
  ActsSimFatrasAligned
  AlignedFatrasExample.cpp)
target_link_libraries(
  ActsSimFatrasAligned
  PRIVATE ActsFrameworkFatrasCommon ACTFWContextualDetector)

# Generic detector with Payload based alignment
add_executable(
  ActsSimFatrasPayload
  PayloadFatrasExample.cpp)
target_link_libraries(
  ActsSimFatrasPayload
  PRIVATE ActsFrameworkFatrasCommon ACTFWContextualDetector)

install(
  TARGETS
    ActsSimFatrasGeneric
    ActsSimFatrasAligned
    ActsSimFatrasPayload
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory_if(DD4hep USE_DD4HEP)
add_subdirectory_if(TGeo USE_TGEO)
