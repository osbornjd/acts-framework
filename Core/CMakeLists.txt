# get list of all source files
file (GLOB src_files "src/*/*.cpp" "include/*/*/*.hpp")

# define library target
add_library(ACTFramework SHARED ${src_files})

# setup include directories
target_include_directories (ACTFramework PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFramework PUBLIC)

# set compiler flags
target_compile_options(ACTFramework PUBLIC -std=c++14)

# set installation directories
install (TARGETS ACTFramework
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install (DIRECTORY include/ACTFW DESTINATION include)

