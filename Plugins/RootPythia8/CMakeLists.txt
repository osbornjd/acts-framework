# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

#find the ROOT package
find_package(ROOT REQUIRED COMPONENTS Core Hist Tree)

if (ROOT_FOUND)
    # get list of all source files
    file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

    add_library(ACTFWRootPythia8Plugins SHARED ${src_files})
    
    # setup include directories
    target_include_directories(ACTFWRootPythia8Plugins PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_include_directories(ACTFWRootPythia8Plugins PUBLIC /Users/salzburg/Documents/andiWork/acts/externals-installed/pythia8219/include/) # {PYTHIA8}/include/) 
    target_include_directories(ACTFWRootPythia8Plugins PUBLIC ${ROOT_INCLUDE_DIRS})
    target_link_libraries(ACTFWRootPythia8Plugins PUBLIC Threads::Threads)
    target_link_libraries(ACTFWRootPythia8Plugins PUBLIC ACTS::ACTSCore ACTFramework)
    target_link_libraries(ACTFWRootPythia8Plugins PUBLIC /Users/salzburg/Documents/andiWork/acts/externals-installed/pythia8219/lib/libpythia8.dylib ) #${PYTHIA8}/lib/)
    target_link_libraries(ACTFWRootPythia8Plugins PUBLIC Core Hist Tree EGPythia8)
    
    # set installation directories
    install (TARGETS ACTFWRootPythia8Plugins LIBRARY DESTINATION lib)
    
    install (DIRECTORY include/ACTFW DESTINATION include)
else (ROOT_FOUND)
  message (STATUS "${Red}ROOT was not found -> skip builidng of RootPythia8Plugins${ColorReset}")
endif (ROOT_FOUND)

