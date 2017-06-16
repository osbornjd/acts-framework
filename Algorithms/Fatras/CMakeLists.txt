file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWFatras SHARED ${src_files})
target_include_directories(ACTFWFatras PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWFatras PUBLIC ACTS::ACTSCore ACTFramework)

install(TARGETS ACTFWFatras LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
