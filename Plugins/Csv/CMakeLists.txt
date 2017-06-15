file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWCsvPlugin SHARED ${src_files})
target_include_directories(ACTFWCsvPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWCsvPlugin PUBLIC ACTS::ACTSCore ACTFramework)
target_link_libraries(ACTFWCsvPlugin PUBLIC Threads::Threads)

install(TARGETS ACTFWCsvPlugin LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include/)
