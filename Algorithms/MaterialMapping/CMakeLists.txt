# manually trigger build of Geant4MaterialMapping
option (BUILD_GEANT4_PLUGIN "build Geant4MaterialMapping" OFF)

if (BUILD_GEANT4_PLUGIN)
  # get list of all source files
  file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")
  
  # find the ACTS package
  find_package(ACTS REQUIRED COMPONENTS Core MaterialPlugin)
  find_package(Geant4 REQUIRED)
  
  # define library target
  add_library(ACTFWMaterialMapping SHARED ${src_files})
  target_include_directories(ACTFWMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
  include(${Geant4_USE_FILE})
  target_link_libraries(ACTFWMaterialMapping PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
  target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFramework ACTFWGeant4Plugin)
  
  # set installation directories
  install(TARGETS ACTFWMaterialMapping LIBRARY DESTINATION lib)
  install(DIRECTORY include/ACTFW DESTINATION include)
endif (BUILD_GEANT4_PLUGIN)
