#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)

# define library target
add_library(ACTFWExtrapolation SHARED ${src_files})

# setup include directories for ACTFWExtrapolation library
target_include_directories(ACTFWExtrapolation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWExtrapolation PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin ACTFramework)

# set installation directories
install(TARGETS ACTFWExtrapolation LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)

