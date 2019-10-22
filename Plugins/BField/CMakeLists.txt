file (GLOB_RECURSE src_files "src/*.cpp")

add_library (ACTFWBFieldPlugin SHARED ${src_files})
target_include_directories(ACTFWBFieldPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWBFieldPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWBFieldPlugin PRIVATE ActsCore)
target_link_libraries(ACTFWBFieldPlugin PRIVATE ACTFramework)
target_link_libraries(ACTFWBFieldPlugin PRIVATE Boost::program_options)
target_link_libraries(ACTFWBFieldPlugin PRIVATE ${ROOT_LIBRARIES})


install(TARGETS ACTFWBFieldPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
