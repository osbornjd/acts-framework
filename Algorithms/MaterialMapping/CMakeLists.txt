# get list of all source files
file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)
find_package(Geant4 REQUIRED)
find_package(DD4hep REQUIRED COMPONENTS DDCore DDG4)

# define library target
add_library(ACTFWMaterialMapping SHARED ${src_files})
target_include_directories(ACTFWMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWMaterialMapping PUBLIC ${DD4hep_INCLUDE_DIRS})  
include(${Geant4_USE_FILE})
target_link_libraries(ACTFWMaterialMapping PUBLIC ${DD4hep_LIBRARIES} ACTFramework ACTFWGeant4Plugin ACTFWDD4hepG4Plugin)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWMaterialMapping PUBLIC ${DD4hep_LIBRARIES})

# set installation directories
install(TARGETS ACTFWMaterialMapping LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)
