file(GLOB_RECURSE src_files "src/*.cpp")
add_library(ACTFWExamplesCommon SHARED ${src_files})
target_include_directories(ACTFWExamplesCommon PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWExamplesCommon PUBLIC ActsCore FatrasCore ACTFramework)
target_link_libraries(ACTFWExamplesCommon PUBLIC ACTFWObjPlugin 
        ACTFWCsvPlugin ACTFWJsonPlugin ACTFWRootPlugin ACTFWDetectorsCommon
        ACTFWBFieldPlugin ACTFWDigitization ACTFWFatras ActsFrameworkGenerators ActsFrameworkPythia8 ACTFWMaterialMapping)
