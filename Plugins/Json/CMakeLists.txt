# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

add_library(ACTFWJsonPlugin INTERFACE)

# setup include directories
target_include_directories(ACTFWJsonPlugin INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)

install (DIRECTORY include/ACTFW DESTINATION include/ACTFW/Plugins)
