# get list of all source files
file (GLOB src_files "src/*.*pp")

# define library target
add_executable(HelloWorldExample ${src_files})

# setup linker dependencies
target_link_libraries(HelloWorldExample PRIVATE ACTFWCore)

# set installation directories
install(TARGETS HelloWorldExample RUNTIME DESTINATION bin)





