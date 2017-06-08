# this plugin needs DD4hep and ROOT
find_package (DD4hep COMPONENTS DDCore)
find_package (ROOT COMPONENTS Geom)
if (DD4hep_FOUND AND ROOT_FOUND)
    # find the ACTS packages
    find_package(ACTS REQUIRED COMPONENTS Core TGeoPlugin DD4hepPlugin)
    # set DD4hep cmake 
    set ( DD4HEP_DEBUG_CMAKE ON )
    dd4hep_set_version(DD4hepPlugin MAJOR 0 MINOR 1 PATCH 0)
    
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp")
    
    # define library target
    add_library (ACTFWDD4hepPlugin SHARED ${src_files})

    # setup include directories
    target_include_directories(ACTFWDD4hepPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories (ACTFWDD4hepPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
    target_include_directories (ACTFWDD4hepPlugin PUBLIC ${DD4hep_INCLUDE_DIRS})
    
    target_link_libraries(ACTFWDD4hepPlugin PUBLIC ACTS::ACTSCore ACTS::ACTSDD4hepPlugin)
    target_link_libraries(ACTFWDD4hepPlugin PRIVATE ACTFramework)
    target_link_libraries (ACTFWDD4hepPlugin PUBLIC ${DD4hep_LIBRARIES})
    target_link_libraries(ACTFWDD4hepPlugin PUBLIC ROOT::Geom)

    # set installation directories
    install (TARGETS ACTFWDD4hepPlugin LIBRARY DESTINATION lib)
    install(DIRECTORY include/ACTFW DESTINATION include)
    
    cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)          
elseif (NOT DD4hep_FOUND)
    message (STATUS "${Red}ACTFWDD4hepPlugin: DD4hep was not found -> skip build of Plugin.${ColorReset}")
elseif (NOT ROOT_FOUND)
    message (STATUS "${Red}ACTFWDD4hepPlugin: ROOT was not found -> skip build of Plugin.${ColorReset}")
endif (DD4hep_FOUND AND ROOT_FOUND)

