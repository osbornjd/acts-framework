cmake_minimum_required (VERSION 3.5)
project (ACTFW)

# set the current version of ACTFW
set (ACTFW_VERSION 0.0.1)

# default installation direction
#set (CMAKE_INSTALL_PREFIX "/opt/ACTFW/${ACTFW_VERSION}" CACHE STRING "installation directory")

# Enable OpenMP support
find_package (OpenMP)
if (OpenMP_FOUND)
  message (STATUS "Using OpenMP")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
 endif(OpenMP_FOUND)

# Enable C++11 threading support, with a preference for pthread
set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# check for Pythia8
if (DEFINED PYTHIA8_INCLUDE_DIR AND PYTHIA8_LIB_DIR)
  message (STATUS "Found Pythia8")
  set (PYTHIA8_FOUND ON)
elseif (NOT DEFINED PYTHIA8_INCLUDE_DIR )
  message (STATUS "'PYTHIA8_INCLUDE_DIR' variable not set
  include directory of Pythia8 library is not set and 'RootPythia8Plugins' 
  and the Examples 'ReadEvGen' and 'Fatras' will not be build. 
  In case you want to use any of them please set it using 
  'cmake <path/to/ACTS> -DPYTHIA8_INCLUDE_DIR=path/to/Pythia8/include")
elseif (NOT DEFINED PYTHIA8_LIB_DIR)
  message (STATUS "'PYTHIA8_LIB_DIR' variable not set
  library directory of Pythia8 library is not set and 'RootPythia8Plugins' 
  and the Examples 'ReadEvGen' and 'Fatras' will not be build. 
  In case you want to use any of them please set it using 
  'cmake <path/to/ACTS> -DPYTHIA8_LIB_DIR=path/to/Pythia8/lib")
endif (DEFINED PYTHIA8_INCLUDE_DIR AND PYTHIA8_LIB_DIR)

# Configure test framework components
add_subdirectory (Core)
add_subdirectory (Examples)
add_subdirectory (Plugins)


