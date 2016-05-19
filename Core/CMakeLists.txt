# get list of all source files
file (GLOB src_files "src/*/*.cpp" "include/*/*/*.hpp")

# define library target
add_library(ACTFWCore SHARED ${src_files})

# setup include directories
target_include_directories(ACTFWCore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWCore PUBLIC)

# set compiler flags
target_compile_options(ACTFWCore PUBLIC -std=c++14)

# set installation directories
install (TARGETS ACTFWCore
	 LIBRARY DESTINATION lib
	 RUNTIME DESTINATION bin)

install (DIRECTORY include/ACTFW DESTINATION include)

