file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWDigitization SHARED ${src_files})
target_include_directories(ACTFWDigitization PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(ACTFWDigitization PRIVATE ACTFramework ActsCore ActsDigitizationPlugin IdentificationPlugin)

install(
  TARGETS ACTFWDigitization
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
