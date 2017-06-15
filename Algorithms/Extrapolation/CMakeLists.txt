file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWExtrapolation SHARED ${src_files})
target_include_directories(ACTFWExtrapolation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWExtrapolation PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin ACTFramework)

install(TARGETS ACTFWExtrapolation LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)

