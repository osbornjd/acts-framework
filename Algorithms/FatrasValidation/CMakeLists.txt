file(GLOB_RECURSE src_files "src/G4FatrasValidation.cpp")

add_library(ACTFWFatrasValidation SHARED ${src_files})
target_include_directories(ACTFWFatrasValidation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWFatrasValidation PUBLIC ACTSCore ACTFramework ACTFWGeant4Plugin ACTFWRootPlugin)

install(TARGETS ACTFWFatrasValidation LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
