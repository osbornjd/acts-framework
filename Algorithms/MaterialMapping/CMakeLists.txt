include(${Geant4_USE_FILE})

file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWMaterialMapping SHARED ${src_files})
target_include_directories(ACTFWMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFramework ACTFWGeant4Plugin)

install(TARGETS ACTFWMaterialMapping LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)

