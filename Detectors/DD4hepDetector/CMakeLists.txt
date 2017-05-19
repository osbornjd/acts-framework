# find the ACTS package
find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
# find DD4hep
set ( DD4HEP_DEBUG_CMAKE ON )
find_package (DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
dd4hep_set_version(DD4hepDetector MAJOR 0 MINOR 1 PATCH 0)
if (DD4hep_FOUND)
  find_package (ROOT REQUIRED COMPONENTS Geom GenVector)

  if (ROOT_FOUND)
    
    list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
    
    message (STATUS "ROOT_INCLUDE_DIRS=${ROOT_INCLUDE_DIRS}")
    message (STATUS "ROOT_LIBRARIES=${ROOT_LIBRARIES}")      
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp")

    # define library target
    add_library (ACTFWDD4hepDetector SHARED ${src_files})

    # setup include directories
    target_include_directories(ACTFWDD4hepDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories (ACTFWDD4hepDetector PUBLIC ${ROOT_INCLUDE_DIRS})
    target_include_directories (ACTFWDD4hepDetector PUBLIC ${DD4hep_INCLUDE_DIRS})

    target_link_libraries(ACTFWDD4hepDetector PRIVATE ACTFramework)
    target_link_libraries(ACTFWDD4hepDetector PUBLIC ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)
    target_link_libraries(ACTFWDD4hepDetector PUBLIC ${ROOT_LIBRARIES} ${DD4hep_LIBRARIES})

    install(TARGETS ACTFWDD4hepDetector LIBRARY DESTINATION lib)

    cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    dd4hep_generate_rootmap(ACTFWDD4hepDetector)

  endif (ROOT_FOUND)
  # set installation directories

else (DD4hep_FOUND)
message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
endif (DD4hep_FOUND)
