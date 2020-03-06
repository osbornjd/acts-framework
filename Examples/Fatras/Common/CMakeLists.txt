add_library(
  ActsFrameworkFatrasCommon SHARED
  FatrasDigitizationBase.cpp
  FatrasEvgenBase.cpp
  FatrasMain.cpp
  FatrasSimulationBase.cpp)
target_include_directories(
  ActsFrameworkFatrasCommon
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  ActsFrameworkFatrasCommon
  PUBLIC
    ActsCore ActsFatras
    ACTFramework
    ActsFrameworkGenerators ActsFrameworkPythia8
    ACTFWBFieldPlugin ACTFWDetectorsCommon ACTFWFatras
    ACTFWDigitization
    ActsFrameworkIoCsv ActsFrameworkIoRoot
    ACTFWExamplesCommon
    Boost::program_options)

install(
  TARGETS ActsFrameworkFatrasCommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
