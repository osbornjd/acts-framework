# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# define library target
add_library(ACTFWFatras SHARED ${src_files})

# setup include directories for FATRAS library
target_include_directories(ACTFWFatras PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWFatras PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWFatras PUBLIC ACTFramework)

# set installation directories
install(TARGETS ACTFWFatras LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
