# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# define library target
add_executable(ACTFWHelloWorldExample ${src_files})

# setup linker dependencies
target_link_libraries(ACTFWHelloWorldExample PRIVATE ACTFramework)

# set installation directories
install(TARGETS ACTFWHelloWorldExample RUNTIME DESTINATION bin)





