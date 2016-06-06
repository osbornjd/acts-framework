# manually trigger build of DD4Hep example
option (BUILD_DD4HEP_EXAMPLE "build DD4HEP example" OFF)

if (BUILD_DD4HEP_EXAMPLE)
# find the ACTS package
find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
  # find DD4hep
  set ( DD4HEP_DEBUG_CMAKE ON )
  find_package (DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
  dd4hep_set_version(DD4hepDetector MAJOR 0 MINOR 1 PATCH 0)
  if (DD4hep_FOUND)
    find_package (ROOT REQUIRED COMPONENTS Geom)

    if (ROOT_FOUND)
      message (STATUS "ROOT=${ROOT_INCLUDE_DIRS}")
      # get list of all source files
      file (GLOB_RECURSE src_files "src/*.cpp")

      # define library target
      add_library (ACTFWDD4hepDetector SHARED ${src_files})

      target_compile_options(ACTFWDD4hepDetector PUBLIC "-I/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/include")
      # setup include directories
      target_include_directories(ACTFWDD4hepDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
      target_include_directories (ACTFWDD4hepDetector PUBLIC ${ROOT_INCLUDE_DIRS})
      target_include_directories (ACTFWDD4hepDetector PUBLIC ${DD4hep_INCLUDE_DIRS})

    target_link_libraries(ACTFWDD4hepDetector PUBLIC ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)
      target_link_libraries(ACTFWDD4hepDetector PRIVATE ACTFramework)
#      target_link_libraries (ACTFWDD4hepDetector PUBLIC Geom)
      target_link_libraries (ACTFWDD4hepDetector PUBLIC "-L/afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/x86_64-slc6-gcc49-opt/lib -lGeom" ${DD4hep_LIBRARIES})

     install(TARGETS ACTFWDD4hepDetector LIBRARY DESTINATION lib)


    cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
#    dd4hep_configure_output()
#    include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
#	dd4hep_add_plugin( DD4hepDetector SOURCES src/*.cpp  )
#    dd4hep_install_dir( compact DESTINATION Examples/DD4hepExample/DD4hepDetector )
#    dd4hep_configure_scripts( DD4hepDetector DEFAULT_SETUP WITH_TESTS)

  set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
#    message(STATUS "LIBRARY_OUTPUT_PATH -> ${LIBRARY_OUTPUT_PATH}")
    dd4hep_generate_rootmap(ACTFWDD4hepDetector)
#    add_custom_command(TARGET DD4hepDetector
#                    POST_BUILD
#                    COMMAND ${DD4hep_DIR}/bin/listcomponents -o ../../lib/libDD4hepDetector.components libDD4hepDetector.so)

    endif (ROOT_FOUND)
    # set installation directories

  else (DD4hep_FOUND)
    message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
  endif (DD4hep_FOUND)
endif(BUILD_DD4HEP_EXAMPLE)












