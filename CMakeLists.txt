cmake_minimum_required(VERSION 3.5)

project(ACTFW VERSION 0.0.1 LANGUAGES CXX)

option(USE_DD4HEP "Build DD4hep-based code" off)
option(USE_GEANT4 "Build Geant4-based code" off)
option(USE_OPENMP "Build framework with OpenMP enabled" off)
option(USE_PYTHIA8 "Build Pythia8-based code" off)

# require C++14 globally
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# required packages
find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)
# Enable C++11 threading support, with a preference for pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(ROOT 6.08 REQUIRED COMPONENTS Core GenVector Geom Hist Tree TreePlayer)
# ROOT namespace fix: 6.08 does not support ROOT CMake namespace
# to be removed when minimum version > 6.09
if(${ROOT_VERSION_MINOR} MATCHES "8")
  set(ROOT_CNS "")
else()
  set(ROOT_CNS "ROOT::")
endif()

# handle optional packages
if(USE_DD4HEP AND USE_GEANT4)
  set(DD4HEP_DEBUG_CMAKE on)
  find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin DD4hepPlugin TGeoPlugin)
  find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4 DDSegmentation)
  find_package(Geant4 REQUIRED)
elseif(USE_DD4HEP)
  set(DD4HEP_DEBUG_CMAKE on)
  find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin DD4hepPlugin TGeoPlugin)
  find_package(DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
elseif(USE_GEANT4)
  find_package(Geant4 REQUIRED COMPONENTS gdml)
endif()
if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
if (USE_PYTHIA8)
  # TODO replace manual path checks w/ something like FindPythia8
  if (NOT DEFINED PYTHIA8_INCLUDEDIR OR NOT DEFINED PYTHIA8_LIBRARYDIR)
    message(FATAL_ERROR "Missing 'PYTHIA8_INCLUDEDIR' or 'PYTHIA_LIBRARYDIR'")
  endif()
  find_package(ROOT REQUIRED COMPONENTS EGPythia8)
endif()

add_subdirectory(Core)
add_subdirectory(Algorithms)
add_subdirectory(Detectors)
add_subdirectory(Plugins)
add_subdirectory(Examples)

