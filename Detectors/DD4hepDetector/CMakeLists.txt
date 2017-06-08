# find DD4hep
set ( DD4HEP_DEBUG_CMAKE ON )
find_package (DD4hep COMPONENTS DDCore DDSegmentation)
find_package (ROOT COMPONENTS Geom GenVector)
if (DD4hep_FOUND AND ROOT_FOUND)
  dd4hep_set_version(DD4hepDetector MAJOR 0 MINOR 1 PATCH 0)
  
    find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
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

          
elseif (NOT DD4hep_FOUND)
    message (STATUS "${Red}ACTFWDD4hepDetector: DD4hep was not found -> skip build of Plugin.${ColorReset}")
elseif (NOT ROOT_FOUND)
    message (STATUS "${Red}ACTFWDD4hepDetector: ROOT was not found -> skip build of Plugin.${ColorReset}")
endif (DD4hep_FOUND AND ROOT_FOUND)
