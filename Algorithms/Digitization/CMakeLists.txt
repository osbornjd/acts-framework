file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWDigitization SHARED ${src_files})
target_include_directories(ACTFWDigitization PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWDigitization PUBLIC ACTS::ACTSCore ACTFramework)

install(TARGETS ACTFWDigitization LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
