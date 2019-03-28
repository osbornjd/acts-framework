file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWContextualDetector SHARED ${src_files})
target_include_directories(ACTFWContextualDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWContextualDetector PUBLIC ActsCore IdentificationPlugin ActsDigitizationPlugin)
target_link_libraries(ACTFWContextualDetector PUBLIC ACTFramework ACTFWGenericDetector ACTFWBFieldPlugin)

install(TARGETS ACTFWContextualDetector LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})