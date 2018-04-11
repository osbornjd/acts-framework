file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWSpacePointFinder SHARED ${src_files})
target_include_directories(ACTFWSpacePointFinder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWSpacePointFinder PUBLIC ACTSCore ACTFramework ACTFWGenericDetector)

install(TARGETS ACTFWSpacePointFinder LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
