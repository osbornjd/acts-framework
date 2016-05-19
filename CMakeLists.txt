cmake_minimum_required (VERSION 2.6)
project (ACTFW)

# set the current version of ACTFW
set (ACTFW_VERSION 0.0.1)

# default installation direction
set (CMAKE_INSTALL_PREFIX "/opt/ACTFW/${ACTFW_VERSION}" CACHE STRING "installation directory")

add_subdirectory (Core)
add_subdirectory (Examples)
