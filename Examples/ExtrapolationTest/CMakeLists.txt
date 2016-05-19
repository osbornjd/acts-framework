# get list of all source files
file (GLOB src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS)

# define library target
add_executable(ExtrapolationExample ${src_files})

# setup include directories
target_include_directories(ExtrapolationExample PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ExtrapolationExample PRIVATE ACTFWCore)
target_link_libraries(ExtrapolationExample PUBLIC ACTSCore)

# set installation directories
install(TARGETS ExtrapolationExample RUNTIME DESTINATION bin)





