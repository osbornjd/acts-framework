file(GLOB_RECURSE src_files "src/MaterialMapping.cpp")

if (USE_GEANT4)
    set(src_files ${src_files} "src/Geant4MaterialRecording.cpp")
endif()

add_library(ACTFWMaterialMapping SHARED ${src_files})
target_include_directories(ACTFWMaterialMapping PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWMaterialMapping PUBLIC ActsCore ActsMaterialMappingPlugin)
target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFramework)
if (USE_GEANT4)
    target_link_libraries(ACTFWMaterialMapping PUBLIC ACTFWGeant4Plugin ACTFWGeometryInterfaces)
endif()

install(TARGETS ACTFWMaterialMapping LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
