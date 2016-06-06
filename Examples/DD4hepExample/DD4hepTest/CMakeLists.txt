# manually trigger build of DD4Hep example
option (BUILD_DD4HEP_EXAMPLE "build DD4HEP example" OFF)

if (BUILD_DD4HEP_EXAMPLE)
  # find the ACTS package
  find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
  # find DD4hep
  set ( DD4HEP_DEBUG_CMAKE ON )
  find_package (DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
  if (DD4hep_FOUND)
    find_package (ROOT REQUIRED COMPONENTS Geom)

    if (ROOT_FOUND)
    message (STATUS "ROOT=${ROOT_INCLUDE_DIRS}")
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp")

    # define library target
    add_executable(ACTFWDD4hepTest ${src_files})

    target_compile_options(ACTFWDD4hepTest PUBLIC "-I/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/include")
    # setup include directories
    target_include_directories(ACTFWDD4hepTest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories (ACTFWDD4hepTest PUBLIC ${ROOT_INCLUDE_DIRS})
    target_include_directories (ACTFWDD4hepTest PUBLIC ${DD4hep_INCLUDE_DIRS})

    target_link_libraries(ACTFWDD4hepTest PUBLIC ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)
    target_link_libraries(ACTFWDD4hepTest PRIVATE ACTFramework)
    #      target_link_libraries (ACTFWDD4hepTest PUBLIC Geom)
    target_link_libraries (ACTFWDD4hepTest PUBLIC "-L/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/lib -lGeom" ${DD4hep_LIBRARIES})

    install(TARGETS ACTFWDD4hepTest RUNTIME DESTINATION lib)

    endif (ROOT_FOUND)
    # set installation directories

  else (DD4hep_FOUND)
  message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
  endif (DD4hep_FOUND)
endif(BUILD_DD4HEP_EXAMPLE)

