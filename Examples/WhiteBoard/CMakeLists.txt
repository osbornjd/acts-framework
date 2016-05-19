# get list of all source files
file (GLOB src_files "src/*.*pp")

# define library target
add_executable(WhiteBoardExample ${src_files})

# setup linker dependencies
target_link_libraries(WhiteBoardExample PRIVATE ACTFWCore)

# set installation directories
install(TARGETS WhiteBoardExample RUNTIME DESTINATION bin)





