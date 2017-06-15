file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFramework SHARED ${src_files})
target_include_directories (ACTFramework PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFramework PUBLIC ACTS::ACTSCore)

install(TARGETS ACTFramework LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)

