# get list of all source files
file (GLOB src_files "src/*.*pp")

# define library target
add_executable(ACTFWRandomNumberExample ${src_files})

# setup linker dependencies
target_link_libraries(ACTFWRandomNumberExample PRIVATE ACTFramework)

# set installation directories
install(TARGETS ACTFWRandomNumberExample RUNTIME DESTINATION bin)





