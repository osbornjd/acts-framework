# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core)

find_package(ROOT COMPONENTS Core Hist Tree)
if (ROOT_FOUND)  
  # (1) Main functionality:
  message(STATUS "${Blue}ACTFWRootPlugin: adding base components.${ColorReset}")
  # find the ROOT components for core
  # add the core source
  file (GLOB_RECURSE src_files "src/RootParticleWriter.cpp" "include/RootExCell*pp" "include/RootParticleWriter.hpp")
  
  # (2) IF pythia8 is supported
  if (PYTHIA8_FOUND)
    message(STATUS "${Blue}ACTFWRootPlugin: adding TPythia8 support.${ColorReset}")
    # find the ROOT components for EGPythia8
    find_package(ROOT REQUIRED COMPONENTS EGPythia8)
    file (GLOB_RECURSE src_files ${src_files} "src/TPythia8Generator.cpp" "include/TPythia8Generator.hpp")
  endif (PYTHIA8_FOUND)
  
  # (3) IF MaterialPlugin support 
  find_package(ACTS QUIET COMPONENTS MaterialPlugin)
  message(STATUS "${Blue}ACTFWRootPlugin: adding MaterialPlugin support.${ColorReset}")
    # get list of all source files
  file (GLOB_RECURSE src_files ${src_files} "src/Root*Material*.cpp" "include/Root*Material.*pp")
  
  #---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
  include(${ROOT_USE_FILE})
  
  # generate root dictionary for material mapping - needed to write this class to a root file
  ROOT_GENERATE_DICTIONARY(G__Material  ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialStep.hpp ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp ${ACTS_INCLUDE_DIR}/ACTS/Material/Material.hpp LINKDEF include/ACTFW/Plugins/Root/Material_LinkDef.h OPTIONS -I/${ACTS_INCLUDE_DIR})
  add_library(ACTFWRootPlugin SHARED ${src_files} G__Material.cxx)
      
  # (1) setup include directories
  target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
  target_include_directories (ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
  target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)
  target_link_libraries(ACTFWRootPlugin PUBLIC ACTS::ACTSCore ACTFramework)
  target_link_libraries(ACTFWRootPlugin PUBLIC Core Hist Tree TreePlayer)
  # (2) Pythia8 support
  if (PYTHIA8_FOUND)
    target_include_directories(ACTFWRootPlugin PUBLIC ${PYTHIA8_INCLUDE_DIR}) 
    target_link_libraries(ACTFWRootPlugin PUBLIC -L${PYTHIA8_LIB_DIR})
    target_link_libraries(ACTFWRootPlugin PUBLIC ROOT::EGPythia8)
  endif (PYTHIA8_FOUND)
  # (3) MaterialPlugin
  target_link_libraries(ACTFWRootPlugin PUBLIC ACTS::ACTSMaterialPlugin)
  
  # set installation directories
  install (TARGETS ACTFWRootPlugin LIBRARY DESTINATION lib)
      
  install (DIRECTORY include/ACTFW DESTINATION include)
else (NOT ROOT_FOUND)  
  message (STATUS "${Red}ACTFWRootPlugin: ROOT was not found -> skip build of Plugin.${ColorReset}")
endif (ROOT_FOUND)

