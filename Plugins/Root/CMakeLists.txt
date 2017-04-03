
# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

#find the ROOT package
find_package(ROOT REQUIRED COMPONENTS Core Hist Tree)

  # get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWRootPlugins SHARED ${src_files})
    
# setup include directories
target_include_directories(ACTFWRootPlugins PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories (ACTFWRootPlugins PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugins PUBLIC Threads::Threads)
target_link_libraries(ACTFWRootPlugins PUBLIC ACTS::ACTSCore ACTFramework)
target_link_libraries(ACTFWRootPlugins PUBLIC ROOT::Core ROOT::Hist ROOT::Tree ROOT::TreePlayer)
    
# set installation directories
install (TARGETS ACTFWRootPlugins LIBRARY DESTINATION lib)
    
install (DIRECTORY include/ACTFW DESTINATION include)


