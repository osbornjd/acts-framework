file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWHelloWorldExample ${src_files})
target_link_libraries(ACTFWHelloWorldExample PRIVATE ACTFramework)
target_link_libraries(ACTFWHelloWorldExample PUBLIC ACTS::ACTSCore)

install(TARGETS ACTFWHelloWorldExample RUNTIME DESTINATION bin)

