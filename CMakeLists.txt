cmake_minimum_required(VERSION 3.9)

project(ActsFramework LANGUAGES CXX)

option(USE_STANDALONE_ACTS "Use a separate, standalone acts-core installation" OFF)
option(USE_DD4HEP "Build DD4hep-based geometry code" OFF)
option(USE_GEANT4 "Build Geant4-based code" OFF)
option(USE_HEPMC3 "Build HepMC3-based code" OFF)
option(USE_PYTHIA8 "Build Pythia8-based code" OFF)
option(USE_TGEO "Build TGeo-based geometry code" OFF)

# Use the framework identifier instead of the bare Acts one
add_definitions(-DACTS_CORE_IDENTIFIER_PLUGIN="${CMAKE_CURRENT_SOURCE_DIR}/Core/include/ACTFW/EventData/SimIdentifier.hpp")

# determine build-flag dependent components
set(DD4hep_COMPONENTS DDCore)
if(USE_GEANT4)
  list(APPEND DD4hep_COMPONENTS DDG4)
endif()
set(ROOT_COMPONENTS Core Hist Tree TreePlayer)
if(USE_DD4HEP)
  list(APPEND ROOT_COMPONENTS Geom GenVector)
endif()

# use install paths consistent w/ ACTS
include(GNUInstallDirs)
# place build products in `<build_dir>/bin` and `<build_dir>/lib` for simple use
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# required packages

# Enable C++11 threading support, with a preference for pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
# heterogeneous lookup in set-like containers requires 1.68
find_package(Boost 1.68 REQUIRED COMPONENTS filesystem program_options)
find_package(ROOT 6.10 REQUIRED COMPONENTS ${ROOT_COMPONENTS})
find_package(TBB REQUIRED)

# optional packages

if(USE_STANDALONE_ACTS)
  message(STATUS "Using external Acts installation")
  find_package(Acts REQUIRED COMPONENTS Core DD4hepPlugin DigitizationPlugin IdentificationPlugin JsonPlugin TGeoPlugin Fatras)
else()
  message(STATUS "Building Acts from source")
  # acts-core is build as subprojects with fixed options
  set(ACTS_BUILD_DD4HEP_PLUGIN ${USE_DD4HEP} CACHE BOOL "")
  set(ACTS_BUILD_DIGITIZATION_PLUGIN ON CACHE BOOL "")
  set(ACTS_BUILD_IDENTIFICATION_PLUGIN ON CACHE BOOL "")
  set(ACTS_BUILD_JSON_PLUGIN ON CACHE BOOL "")
  set(ACTS_BUILD_TGEO_PLUGIN ON CACHE BOOL "")
  set(ACTS_BUILD_FATRAS ON CACHE BOOL "")
  set(ACTS_BUILD_LEGACY OFF CACHE BOOL "")
  set(ACTS_BUILD_BENCHMARKS OFF CACHE BOOL "")
  set(ACTS_BUILD_EXAMPLES OFF CACHE BOOL "")
  set(ACTS_BUILD_UNITTESTS OFF CACHE BOOL "")
  set(ACTS_BUILD_INTEGRATIONTESTS OFF CACHE BOOL "")
  set(ACTS_BUILD_DOC OFF CACHE BOOL "")
  set(ACTS_USE_BUNDLED_NLOHMANN_JSON ON CACHE BOOL "")
  add_subdirectory(external/acts-core)
endif()
if(USE_DD4HEP)
  set(DD4HEP_DEBUG_CMAKE on)
  # DD4hep issues w/ C++17 are handled by setting C++17 only as a per-target
  # requirement and not globally. no workaround required anymore.
  find_package(DD4hep REQUIRED COMPONENTS ${DD4hep_COMPONENTS})
endif()
if(USE_GEANT4)
  set(Geant4_CONFIG_DEBUG on)
  find_package(Geant4 REQUIRED gdml)
endif()
if(USE_HEPMC3)
  # must be version 3.1+. version 3.0 uses HepMC w/o number as module name.
  # version 3.1.0 misses the version variable so we can not check for it
  find_package(HepMC3 REQUIRED)
  find_package(HepPID REQUIRED)
  # workaround to fix broken cmake config for version 3.1.0.
  # it searches for the HepMC libray instead of the correct HepMC3 one
  # and sets HEPMC_LIBRARIES instead of the expected HEPMC3_LIBRARIES.
  if(NOT HEPMC3_LIBRARIES)
    message(STATUS "Workaround for HepMC3. Search in $ENV{HepMC3_DIR}")
    find_library(
      HEPMC3_LIB Names HepMC3
      PATHS $ENV{HepMC3_DIR}/lib $ENV{HepMC3_DIR}/lib64 NO_DEFAULT_PATH)
    set(HEPMC3_INCLUDE_DIR ${HEPMC_INCLUDE_DIR})
    set(HEPMC3_LIBRARIES ${HEPMC3_LIB})
  endif()
endif()
if(USE_PYTHIA8)
  find_package(Pythia8 REQUIRED)
endif()

# packages not available as external packages that are always build from source

add_subdirectory(external/dfelibs)

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

add_subdirectory(Core)
add_subdirectory(Algorithms)
add_subdirectory(Detectors)
add_subdirectory(Io)
add_subdirectory(Plugins)
add_subdirectory(Examples)
