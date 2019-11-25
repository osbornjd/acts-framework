set(src_files 
        src/CommonGeometry.cpp 
        src/CommonOptions.cpp 
        src/GeometryExampleBase.cpp 
        src/MaterialMappingBase.cpp 
        src/MaterialValidationBase.cpp 
        src/PropagationExampleBase.cpp )

# append Fatras example is Pythia8 is avaialbe
if(USE_PYTHIA8)
  list(APPEND src_files 
        src/FatrasExampleBase.cpp
        src/FatrasDigitizationBase.cpp
        src/FatrasSimulationBase.cpp
        src/FatrasEvgenBase.cpp
        src/FittingBase.cpp)
endif()

add_library(ACTFWExamplesCommon SHARED ${src_files})
target_include_directories(
  ACTFWExamplesCommon
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWExamplesCommon
  PUBLIC ActsCore FatrasCore ACTFramework ACTFWObjPlugin ACTFWCsvPlugin
    ACTFWJsonPlugin ACTFWRootPlugin ACTFWDetectorsCommon ACTFWBFieldPlugin
    ACTFWDigitization ACTFWPropagation ACTFWFatras ActsFrameworkGenerators ACTFWMaterialMapping
    ACTFWFitting)

if(USE_PYTHIA8)
  target_sources(ACTFWExamplesCommon PRIVATE src/FatrasExampleBase.cpp)
  target_link_libraries(ACTFWExamplesCommon PUBLIC ActsFrameworkPythia8)
endif()

install(
  TARGETS ACTFWExamplesCommon
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
