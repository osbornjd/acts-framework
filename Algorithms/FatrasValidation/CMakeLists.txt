file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWFatrasValidation SHARED ${src_files})
target_include_directories(ACTFWFatrasValidation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWFatrasValidation PUBLIC ACTS::ACTSCore ACTFramework ACTFWGeant4Plugin ACTFWRootPlugin)

install(TARGETS ACTFWFatrasValidation LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
