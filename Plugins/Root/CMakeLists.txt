
# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

# (1) Main functionality:
message(STATUS "ACTFWRootPlugin: adding base components")
# find the ROOT components for core
find_package(ROOT REQUIRED COMPONENTS Core Hist Tree)
# add the core source
file (GLOB_RECURSE src_files "src/ParticlePropertiesWriter.cpp" "include/RootExCell*pp" "include/ParticlePropertiesWriter.hpp")

# (2) IF pythia8 is supported
if (PYTHIA8_FOUND)
  message(STATUS "ACTFWRootPlugin: adding TPythia8 support")
  # find the ROOT components for EGPythia8
  find_package(ROOT REQUIRED COMPONENTS EGPythia8)
  file (GLOB_RECURSE src_files ${src_files} "src/TPythia8Generator.cpp" "include/TPythia8Generator.hpp")
endif (PYTHIA8_FOUND)

add_library(ACTFWRootPlugin SHARED ${src_files})
    
# (1) setup include directories
target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories (ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)
target_link_libraries(ACTFWRootPlugin PUBLIC ACTS::ACTSCore ACTFramework)
target_link_libraries(ACTFWRootPlugin PUBLIC ROOT::Core ROOT::Hist ROOT::Tree ROOT::TreePlayer)
# (2) Pythia8 support
if (PYTHIA8_FOUND)
  target_include_directories(ACTFWRootPlugin PUBLIC ${PYTHIA8_INCLUDE_DIR}) 
  target_link_libraries(ACTFWRootPlugin PUBLIC -L${PYTHIA8_LIB_DIR})
  target_link_libraries(ACTFWRootPlugin PUBLIC ROOT::EGPythia8)
endif (PYTHIA8_FOUND)

# set installation directories
install (TARGETS ACTFWRootPlugin LIBRARY DESTINATION lib)
    
install (DIRECTORY include/ACTFW DESTINATION include)


