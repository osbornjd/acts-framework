# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)
# find the Geant4 package
find_package(Geant4)

# find the Root geometry package
find_package (ROOT COMPONENTS Geom)

if (Geant4_FOUND AND ROOT_FOUND )
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")    
    add_library(ACTFWGeant4Plugin SHARED ${src_files})

    # setup include directories
    target_include_directories(ACTFWGeant4Plugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories (ACTFWGeant4Plugin PUBLIC ${ROOT_INCLUDE_DIRS})
    target_include_directories (ACTFWGeant4Plugin PUBLIC ${Geant4_INCLUDE_DIRS})
    target_link_libraries(ACTFWGeant4Plugin PUBLIC ${Geant4_LIBRARIES})
    target_link_libraries(ACTFWGeant4Plugin PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin ACTFramework)
    target_link_libraries(ACTFWGeant4Plugin PUBLIC ${ROOT_LIBRARIES})
    target_compile_options(ACTFWGeant4Plugin PUBLIC ${Geant4_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "-std=c++14")
    
    # set installation directories
    install (TARGETS ACTFWGeant4Plugin LIBRARY DESTINATION lib)

    install (DIRECTORY include/ACTFW DESTINATION include)
elseif (NOT Geant4_FOUND)
    message (STATUS "${Red}ACTFWGeant4Plugin: Geant4 was not found -> skip builidng of Plugin.${ColorReset}")
elseif (NOT ROOT_FOUND)
    message (STATUS "${Red}ACTFWGeant4Plugin: ROOT was not found -> skip builidng of Plugin.${ColorReset}")
endif (Geant4_FOUND AND ROOT_FOUND )



