file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWEmptyDetector SHARED ${src_files})
target_include_directories(ACTFWEmptyDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWEmptyDetector PUBLIC ActsCore)
target_link_libraries(ACTFWEmptyDetector PUBLIC ACTFramework ACTFWDetectorsCommon)

install(TARGETS ACTFWEmptyDetector LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
