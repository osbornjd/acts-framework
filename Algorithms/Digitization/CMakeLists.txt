#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# define library target
add_library(ACTFWDigitization SHARED ${src_files})

# setup include directories for ACTFWDigitization library
target_include_directories(ACTFWDigitization PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWDigitization PUBLIC ACTS::ACTSCore ACTFramework)

# set installation directories
install(TARGETS ACTFWDigitization LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
