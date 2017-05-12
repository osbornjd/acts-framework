# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWObjPlugin SHARED ${src_files})

# setup include directories
target_include_directories(ACTFWObjPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWObjPlugin PUBLIC Threads::Threads)
target_link_libraries(ACTFWObjPlugin PUBLIC ACTS::ACTSCore ACTFramework)

# set installation directories
install (TARGETS ACTFWObjPlugin LIBRARY DESTINATION lib)

install (DIRECTORY include/ACTFW DESTINATION include)
