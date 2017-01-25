
# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWCsvPlugins SHARED ${src_files})

# setup include directories
target_include_directories(ACTFWCsvPlugins PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWCsvPlugins PUBLIC Threads::Threads)
target_link_libraries(ACTFWCsvPlugins PUBLIC ACTS::ACTSCore ACTFramework)

# set installation directories
install (TARGETS ACTFWCsvPlugins LIBRARY DESTINATION lib)

install (DIRECTORY include/ACTFW DESTINATION include)
