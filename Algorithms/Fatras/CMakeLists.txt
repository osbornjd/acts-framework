file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWFatras INTERFACE)
target_include_directories(ACTFWFatras INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWFatras INTERFACE ACTS::ACTSCore ACTFramework ACTFWExtrapolation)

install(TARGETS ACTFWFatras LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})