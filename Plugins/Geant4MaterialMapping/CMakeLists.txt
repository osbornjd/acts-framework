# manually trigger build of Geant4MaterialMapping
option (BUILD_GEANT4_MATERIALMAPPING "build Geant4MaterialMapping" OFF)

if (BUILD_GEANT4_MATERIALMAPPING)
    # find the ACTS package
    find_package(ACTS COMPONENTS Core MaterialPlugin)

    # find the Geant4 package
    find_package(Geant4 REQUIRED)

    # find the Root geometry package
    find_package (ROOT REQUIRED COMPONENTS Geom)

    # finde the VGM package which is needed for the geometry conversion from TGeo Root to Geantt4
    #find_package(VGM)

    if (Geant4_FOUND AND ROOT_FOUND )#AND VGM_FOUND)
        # get list of all source files
        file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")
    
        add_library(ACTFWGeant4MaterialMapping SHARED ${src_files})
    
        # setup include directories
        target_include_directories(ACTFWGeant4MaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
        target_include_directories (ACTFWGeant4MaterialMapping PUBLIC ${ROOT_INCLUDE_DIRS})
        target_include_directories (ACTFWGeant4MaterialMapping PUBLIC ${Geant4_INCLUDE_DIRS})
#    target_include_directories (ACTFWGeant4MaterialMapping PUBLIC ${VGM_INCLUDE_DIRS})
    
#        include(${Geant4_USE_FILE})
#        message (STATUS "Geant4_USE_FILE " ${Geant4_USE_FILE})
        target_link_libraries(ACTFWGeant4MaterialMapping PUBLIC ${Geant4_LIBRARIES})
        target_link_libraries(ACTFWGeant4MaterialMapping PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin ACTFramework)
        target_link_libraries(ACTFWGeant4MaterialMapping PUBLIC ${ROOT_LIBRARIES})
#    target_link_libraries(ACTFWGeant4MaterialMapping PUBLIC ${VGM_LIBRARIES}) #${VGM_GEANT4_LIBRARIES} ${VGM_ROOT_LIBRARIES})

target_compile_options(ACTFWGeant4MaterialMapping PUBLIC ${Geant4_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "-std=c++14")
    # set installation directories
        install (TARGETS ACTFWGeant4MaterialMapping LIBRARY DESTINATION lib)
    
        install (DIRECTORY include/ACTFW DESTINATION include)
    elseif (NOT Geant4_FOUND)
        message (STATUS "${Red}Geant4 was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    elseif (NOT ROOT_FOUND)
        message (STATUS "${Red}ROOT was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    #elseif (NOT VGM_FOUND)
    #message (STATUS "${Red}VGM was not found -> skip builidng of Geant4MaterialMapping${ColorReset}")
    endif (Geant4_FOUND AND ROOT_FOUND )#AND VGM_FOUND)
endif(BUILD_GEANT4_MATERIALMAPPING)



