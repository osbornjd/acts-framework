# get list of all source files
file (GLOB src_files "src/*.*pp")

# define library target
add_executable(RandomNumberExample ${src_files})

# setup linker dependencies
target_link_libraries(RandomNumberExample PRIVATE ACTFWCore)

# set installation directories
install(TARGETS RandomNumberExample RUNTIME DESTINATION bin)





