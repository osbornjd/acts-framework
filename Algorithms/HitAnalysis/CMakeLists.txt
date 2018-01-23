file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWHitAnalysis SHARED ${src_files})
target_include_directories(ACTFWHitAnalysis  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWHitAnalysis  PUBLIC ACTSCore ACTFramework)

install(TARGETS ACTFWHitAnalysis  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})