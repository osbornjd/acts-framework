cmake_minimum_required (VERSION 2.6)
project (ACTFW)

# set the current version of ACTFW
set (ACTFW_VERSION 0.0.1)

# default installation direction
set (CMAKE_INSTALL_PREFIX "/opt/ACTFW/${ACTFW_VERSION}" CACHE STRING "installation directory")

# Enable OpenMP support
find_package (OpenMP REQUIRED)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Configure test framework components
add_subdirectory (Core)
add_subdirectory (Examples)
add_subdirectory (Plugins)
