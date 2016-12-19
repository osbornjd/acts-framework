if (BUILD_DD4HEP_PLUGIN AND BUILD_GEANT4_MATERIALMAPPING)
    # find the Geant4 package
    find_package(Geant4 REQUIRED COMPONENTS)
    find_package(ROOT REQUIRED COMPONENTS Geom)
    find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

    if (Geant4_FOUND AND DD4hep_FOUND AND ROOT_FOUND)
        # get list of all source files
        file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")
    
        add_library(ACTFWDD4hepG4 SHARED ${src_files})
    
        # setup include directories
        target_include_directories(ACTFWDD4hepG4 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
        target_include_directories (ACTFWDD4hepG4 PUBLIC ${ROOT_INCLUDE_DIRS})
        target_include_directories (ACTFWDD4hepG4 PUBLIC ${DD4hep_INCLUDE_DIRS})
    
        include(${Geant4_USE_FILE})
        message (STATUS "Geant4_USE_FILE " ${Geant4_USE_FILE})
        target_link_libraries(ACTFWDD4hepG4 PUBLIC ${ROOT_LIBRARIES})
        target_link_libraries(ACTFWDD4hepG4 PUBLIC ${G4LIB})
        target_link_libraries(ACTFWDD4hepG4 PUBLIC ${DD4hep_LIBRARIES})
        target_link_libraries(ACTFWDD4hepG4 PUBLIC ${DD4hep_DDG4_LIBRARY})
        target_link_libraries(ACTFWDD4hepG4 PUBLIC ACTFramework)
        # set installation directories
        install (TARGETS ACTFWDD4hepG4 LIBRARY DESTINATION lib)
    
        install (DIRECTORY include/ACTFW DESTINATION include)
    elseif (NOT Geant4_FOUND)
        message (STATUS "${Red}Geant4 was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    elseif (NOT DD4hep_FOUND)
        message (STATUS "${Red}DD4hep was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    elseif (NOT ROOT_FOUND)
        message (STATUS "${Red}Root was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    endif (Geant4_FOUND AND DD4hep_FOUND AND ROOT_FOUND)
endif(BUILD_DD4HEP_PLUGIN AND BUILD_GEANT4_MATERIALMAPPING)