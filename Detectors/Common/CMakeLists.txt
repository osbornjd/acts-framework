#file(GLOB_RECURSE src_files "src/*.cpp")
add_library(ACTFWDetectorsCommon INTERFACE)
target_include_directories(ACTFWDetectorsCommon INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWDetectorsCommon INTERFACE Boost::program_options)
