file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWObjPlugin SHARED ${src_files})
target_include_directories(ACTFWObjPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWObjPlugin PRIVATE ActsCore ACTFramework)
target_link_libraries(ACTFWObjPlugin PRIVATE Threads::Threads)
target_link_libraries(ACTFWObjPlugin PRIVATE Boost::program_options dfelibs)

install(TARGETS ACTFWObjPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
