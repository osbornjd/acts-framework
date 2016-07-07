# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# define library target
add_executable(ACTFWRandomNumberExample ${src_files})

# setup linker dependencies
target_link_libraries(ACTFWRandomNumberExample PRIVATE ACTFramework)
target_link_libraries(ACTFWRandomNumberExample PUBLIC ACTS::ACTSCore)

# set installation directories
install(TARGETS ACTFWRandomNumberExample RUNTIME DESTINATION bin)





