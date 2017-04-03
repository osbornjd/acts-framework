
# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)

#find the ROOT package
find_package(ROOT REQUIRED COMPONENTS Core Hist Tree TreePlayer)

  # get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
message(STATUS "ROOT_USE_FILE" ${ROOT_USE_FILE})
include(${ROOT_USE_FILE})
# generate root dictionary for material mapping - needed to write this class to a root file

ROOT_GENERATE_DICTIONARY(G__Material  ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialStep.hpp ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp ${ACTS_INCLUDE_DIR}/ACTS/Material/Material.hpp LINKDEF include/ACTFW/RootMaterialMapping/Material_LinkDef.h OPTIONS -I/${ACTS_INCLUDE_DIR})

add_library(ACTFWRootMaterialMapping SHARED ${src_files} G__Material.cxx)
    
# setup include directories
target_include_directories(ACTFWRootMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories (ACTFWRootMaterialMapping PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootMaterialMapping PUBLIC Threads::Threads)
target_link_libraries(ACTFWRootMaterialMapping PUBLIC ACTS::ACTSCore ACTFramework ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWRootMaterialMapping PUBLIC ${ROOT_LIBRARIES})
    
# set installation directories
install (TARGETS ACTFWRootMaterialMapping LIBRARY DESTINATION lib)
    
install (DIRECTORY include/ACTFW DESTINATION include)

