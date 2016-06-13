# manually trigger build of DD4Hep example
option (BUILD_DD4HEP_EXAMPLE "build DD4HEP example" OFF)

if (BUILD_DD4HEP_EXAMPLE)
  # find the ACTS package
  find_package(ACTS COMPONENTS Core)
  # find DD4hep
  set ( DD4HEP_DEBUG_CMAKE ON )
  find_package (DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
  if (DD4hep_FOUND)
    find_package (ROOT REQUIRED COMPONENTS Geom)

    if (ROOT_FOUND)
    message (STATUS "ROOT=${ROOT_INCLUDE_DIRS}")
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp" "include/*.hpp")

    # define library target
    add_library(ACTFWDD4hepTest SHARED ${src_files})
#   add_executable(ACTFWDD4hepExample bin/DD4hepGeometryTest.cpp)

   target_compile_options(ACTFWDD4hepTest PUBLIC "-I/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/include")
    # setup include directories
    target_include_directories(ACTFWDD4hepTest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories (ACTFWDD4hepTest PUBLIC ${ROOT_INCLUDE_DIRS})
    target_include_directories (ACTFWDD4hepTest PUBLIC ${DD4hep_INCLUDE_DIRS})
    target_include_directories(ACTFWDD4hepTest PUBLIC ACTFWDD4hepDetector)

    target_link_libraries(ACTFWDD4hepTest PUBLIC ACTS::ACTSCore)
    target_link_libraries(ACTFWDD4hepTest PRIVATE ACTFramework ACTFWDD4hepDetector)
    target_link_libraries (ACTFWDD4hepTest PUBLIC "-L/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/lib -lGeom" ${DD4hep_LIBRARIES})

    install(TARGETS ACTFWDD4hepTest LIBRARY DESTINATION lib)
#  install(TARGETS ACTFWDD4hepExample RUNTIME DESTINATION bin)
    install(DIRECTORY include/ACTFW DESTINATION include)

    endif (ROOT_FOUND)
    # set installation directories

  else (DD4hep_FOUND)
  message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
  endif (DD4hep_FOUND)
endif(BUILD_DD4HEP_EXAMPLE)
