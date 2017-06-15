file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWObjPlugin SHARED ${src_files})
target_include_directories(ACTFWObjPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWObjPlugin PUBLIC ACTS::ACTSCore ACTFramework)
target_link_libraries(ACTFWObjPlugin PUBLIC Threads::Threads)

install(TARGETS ACTFWObjPlugin LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
