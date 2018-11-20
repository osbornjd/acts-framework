cmake_minimum_required(VERSION 3.5)

project(ACTSFramework LANGUAGES CXX)

# We normally want to build the framework with OpenMP-based multithreaded event
# processing enabled, because we want Acts developers to test their code with
# multiple threads and every modern compiler supports OpenMP.
#
# However, Apple ships the OSX/macOS system compiler with OpenMP support
# disabled and we don't want to force people to go through the trouble of using
# a non-system compiler just because of this issue. So we make an exception for
# Mac users relying on the Apple-provided version of Clang/LLVM.
#
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(USE_OPENMP_DEFAULT off)
else()
  set(USE_OPENMP_DEFAULT on)
endif()

option(USE_LEGACY "Build Legacy-based code from acts-core" on)
option(USE_FATRAS "Build Fatras-based geometry code" off)
option(USE_TGEO "Build TGeo-based geometry code" off)
option(USE_DD4HEP "Build DD4hep-based geometry code" off)
option(USE_GEANT4 "Build Geant4-based code" off)
option(USE_OPENMP "Build framework with OpenMP enabled" ${USE_OPENMP_DEFAULT})
option(USE_PYTHIA8 "Build Pythia8-based code" off)

option(BUILD_ACTS "Build acts-core as submodule" on)

# Use the framework identifier instead of the bare Acts one
add_definitions(-DACTS_CORE_IDENTIFIER_PLUGIN="${CMAKE_CURRENT_SOURCE_DIR}/Core/include/ACTFW/EventData/SimIdentifier.hpp")

# determine build-flag dependent components
set(DD4hep_COMPONENTS DDCore)
if(USE_GEANT4)
  list(APPEND DD4hep_COMPONENTS DDG4)
endif()
set(ROOT_COMPONENTS Core GenVector Hist Tree TreePlayer)
if(USE_DD4HEP)
  list(APPEND ROOT_COMPONENTS Geom GenVector)
endif()
if(USE_PYTHIA8)
  list(APPEND ROOT_COMPONENTS EG)
  # needs fatras
  set(USE_FATRAS ON)
endif()

# use install paths consistent w/ ACTS
include(GNUInstallDirs)
# require C++14 globally
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
# place build products in `<build_dir>/bin` and `<build_dir>/lib` for simple use
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# required packages

# Enable C++11 threading support, with a preference for pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(Boost 1.64 REQUIRED COMPONENTS program_options)
find_package(ROOT 6.10 REQUIRED COMPONENTS ${ROOT_COMPONENTS})
include(${ROOT_USE_FILE})

# optional packages

if(USE_DD4HEP)
  set(DD4HEP_DEBUG_CMAKE on)
  find_package(DD4hep REQUIRED COMPONENTS ${DD4hep_COMPONENTS})
endif()
if(USE_GEANT4)
  set(Geant4_CONFIG_DEBUG on)
  find_package(Geant4 REQUIRED gdml)
endif()
if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
if(USE_PYTHIA8)
  find_package(Pythia8 REQUIRED)
endif()

if(BUILD_ACTS)
  message(STATUS "Building Acts core from source")
  # acts-core and acts-fatras are build as subprojects with fixed options
  set(ACTS_BUILD_DOC OFF CACHE BOOL "Build Acts documentation")
  set(ACTS_BUILD_EXAMPLES OFF CACHE BOOL "Build Acts examples")
  set(ACTS_BUILD_TESTS OFF CACHE BOOL "Build Acts unit tests")
  set(ACTS_BUILD_INTEGRATION_TESTS OFF CACHE BOOL "Build Acts integration tests")
  set(ACTS_BUILD_LEGACY ${USE_LEGACY} CACHE BOOL "Build Acts legacy code")
  set(ACTS_BUILD_DD4HEP_PLUGIN ${USE_DD4HEP} CACHE BOOL "Build Acts DD4HEP plugin")
  set(ACTS_BUILD_MATERIAL_PLUGIN ON CACHE BOOL "Build Acts Material plugin")
  set(ACTS_BUILD_DIGITIZATION_PLUGIN ON CACHE BOOL "Build Acts Digitization plugin")
  set(ACTS_BUILD_TGEO_PLUGIN ${USE_TGEO} CACHE BOOL "Build Acts TGeo plugin")  
  set(ACTS_BUILD_IDENTIFICATION_PLUGIN ON CACHE BOOL "Build Acts Identification plugin")  
  add_subdirectory(external/acts-core)
else()
  message(STATUS "Using external Acts core installation")
  find_package(Acts REQUIRED COMPONENTS Core Legacy MaterialMappingPlugin IdentificationPlugin TGeoPlugin DD4hepPlugin DigitizationPlugin)
endif()


# simplify handling of optional components
# all arguments after the path are evaluated in the if(...) context
function(add_subdirectory_if path)
  file(RELATIVE_PATH _name ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
  if(${ARGN})
    add_subdirectory(${path})
    message(STATUS "Enable component '${_name}'")
  else()
    message(STATUS "Disable component '${_name}'")
  endif()
endfunction()

add_subdirectory_if(external/acts-fatras USE_FATRAS)

add_subdirectory(Core)
add_subdirectory(Detectors)
add_subdirectory(Algorithms)
add_subdirectory(Plugins)
add_subdirectory(Examples)
add_subdirectory(Legacy)
