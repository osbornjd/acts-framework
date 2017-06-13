#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# define library target
add_library(ACTFWGenericDetector SHARED ${src_files})

# setup include directories for ACTFWGenericDetector library
target_include_directories(ACTFWGenericDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWGenericDetector PUBLIC ACTS::ACTSCore)

# set installation directories
install(TARGETS ACTFWGenericDetector LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
