# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")


# find the ACTS package
find_package(ACTS COMPONENTS Core)

# define library target
add_library(ACTFramework SHARED ${src_files})

# setup include directories
target_include_directories (ACTFramework PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFramework PUBLIC ACTS::ACTSCore)

# set compiler flags
target_compile_options(ACTFramework PUBLIC -std=c++14)

# set installation directories
install (TARGETS ACTFramework
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install (DIRECTORY include/ACTFW DESTINATION include)

