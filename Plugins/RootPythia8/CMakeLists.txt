

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

#find the ROOT package
find_package(ROOT REQUIRED COMPONENTS Core Hist Tree)

# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWRootPythia8Plugins SHARED ${src_files})
    
# setup include directories
target_include_directories(ACTFWRootPythia8Plugins PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWRootPythia8Plugins PUBLIC -I${PYTHIA8_INCLUDE_DIR}) 
target_include_directories(ACTFWRootPythia8Plugins PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPythia8Plugins PUBLIC Threads::Threads)
target_link_libraries(ACTFWRootPythia8Plugins PUBLIC ACTS::ACTSCore ACTFramework)
target_link_libraries(ACTFWRootPythia8Plugins PUBLIC -L${PYTHIA8_LIB_DIR} -lpythia8)
target_link_libraries(ACTFWRootPythia8Plugins PUBLIC Core Hist Tree EGPythia8)
    
# set installation directories
install (TARGETS ACTFWRootPythia8Plugins LIBRARY DESTINATION lib)
    
install (DIRECTORY include/ACTFW DESTINATION include)


