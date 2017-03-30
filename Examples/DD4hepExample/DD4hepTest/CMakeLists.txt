
    # find the ACTS package
    find_package(ACTS COMPONENTS Core)
    # find DD4hep
    set ( DD4HEP_DEBUG_CMAKE ON )
    find_package (DD4hep REQUIRED COMPONENTS DDCore DDSegmentation)
    find_package (ROOT REQUIRED COMPONENTS Geom)
    if (DD4hep_FOUND AND ROOT_FOUND)
        # get list of all source files
        file (GLOB_RECURSE src_files "src/*.cpp" "include/*.hpp")

        # define library target
        add_library(ACTFWDD4hepTest SHARED ${src_files})

        add_executable(ACTFWDD4hepExample bin/DD4hepGeometryTest.cpp)
        add_executable(ACTFWFCCTkLayoutExample bin/FCCGeometryTest.cpp)
        set(ACTFWAllDD4hepTestList ACTFWDD4hepExample ACTFWFCCExample)
        foreach (ACTFWAllDD4hepTests ${ACTFWAllDD4hepTestList})
        target_include_directories (${ACTFWAllDD4hepTests} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
        target_link_libraries (${ACTFWAllDD4hepTests} PRIVATE ACTFWDD4hepTest)
        target_link_libraries (${ACTFWAllDD4hepTests} PUBLIC ${ROOT_LIBRARIES})
        target_compile_options (${ACTFWAllDD4hepTests} PRIVATE -std=c++14)
        install(TARGETS ${ACTFWAllDD4hepTests} RUNTIME DESTINATION bin)
        endforeach(ACTFWAllDD4hepTests)

        # setup include directories
        target_include_directories(ACTFWDD4hepTest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
        target_include_directories (ACTFWDD4hepTest PUBLIC ${ROOT_INCLUDE_DIRS})
        target_include_directories (ACTFWDD4hepTest PUBLIC ${DD4hep_INCLUDE_DIRS})
        target_include_directories(ACTFWDD4hepTest PUBLIC ACTFramework ACTFWDD4hepDetector ACTFWDD4hepPlugin ACTFWRootPlugins ACTFWExtrapolationExample)

        target_link_libraries(ACTFWDD4hepTest PUBLIC ACTS::ACTSCore)
        target_link_libraries(ACTFWDD4hepTest PRIVATE ACTFramework ACTFWDD4hepDetector ACTFWDD4hepPlugin ACTFWRootPlugins ACTFWExtrapolation)
        target_link_libraries (ACTFWDD4hepTest PUBLIC ${ROOT_LIBRARIES} ${DD4hep_LIBRARIES} ${GEANT4_LIBRARIES})
        target_link_libraries (ACTFWDD4hepTest PUBLIC ${DD4hep_LIBRARIES})


        install(TARGETS ACTFWDD4hepTest LIBRARY DESTINATION lib)
        install(DIRECTORY include/ACTFW DESTINATION include)


    elseif (NOT ROOT_FOUND)
        message (STATUS "${Red}Root was not found -> skip build of DD4hepPlugins${ColorReset}")
    elseif (NOT DD4hep_FOUND)
        message (STATUS "${Red}DD4hep was not found -> skip build of DD4hepPlugins${ColorReset}")
    endif (DD4hep_FOUND AND ROOT_FOUND)
