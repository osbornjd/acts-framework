file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWHelloWorldExample ${src_files})
target_include_directories(ACTFWHelloWorldExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWHelloWorldExample PRIVATE ACTFramework)
target_link_libraries(ACTFWHelloWorldExample PUBLIC Acts::Core)
target_link_libraries(ACTFWHelloWorldExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWHelloWorldExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
