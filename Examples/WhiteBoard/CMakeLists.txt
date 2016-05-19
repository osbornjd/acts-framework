# get list of all source files
file (GLOB src_files "src/*.*pp")

# define library target
add_executable(ACTFWWhiteBoardExample ${src_files})

# setup linker dependencies
target_link_libraries(ACTFWWhiteBoardExample PRIVATE ACTFramework)

# set installation directories
install(TARGETS ACTFWWhiteBoardExample RUNTIME DESTINATION bin)





