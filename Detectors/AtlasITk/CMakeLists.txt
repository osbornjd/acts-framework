file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWAtlasITk SHARED ${src_files})
target_include_directories(ACTFWAtlasITk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWAtlasITk PUBLIC ACTSCore)

install(TARGETS ACTFWAtlasITk LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
