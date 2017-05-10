# manually trigger build of DD4Hep example
option (BUILD_DD4HEP_PLUGIN "build DD4HEP example" OFF)

if (BUILD_DD4HEP_PLUGIN)
    # find the ACTS package
    find_package(ACTS COMPONENTS Core TGeoPlugin DD4hepPlugin)
    # find DD4hep
    set ( DD4HEP_DEBUG_CMAKE ON )
    find_package (DD4hep REQUIRED COMPONENTS DDCore)
    dd4hep_set_version(DD4hepPlugin MAJOR 0 MINOR 1 PATCH 0)
    find_package (ROOT REQUIRED COMPONENTS Geom)
    if (DD4hep_FOUND AND ROOT_FOUND)
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
        
        set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    
    elseif (NOT DD4hep_FOUND)
        message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
    elseif (NOT ROOT_FOUND)
        message (STATUS "${Red}Root was not found -> skip build of DD4hepPlugins${ColorReset}")
    endif (DD4hep_FOUND AND ROOT_FOUND)
endif(BUILD_DD4HEP_PLUGIN)



