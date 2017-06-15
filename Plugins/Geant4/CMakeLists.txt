file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWGeant4Plugin SHARED ${src_files})
target_include_directories(ACTFWGeant4Plugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWGeant4Plugin PUBLIC ${Geant4_INCLUDE_DIRS})
target_include_directories(ACTFWGeant4Plugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWGeant4Plugin PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin ACTFramework)
target_link_libraries(ACTFWGeant4Plugin PUBLIC ${Geant4_LIBRARIES})
target_link_libraries(ACTFWGeant4Plugin PUBLIC ${ROOT_LIBRARIES})
target_compile_options(ACTFWGeant4Plugin PUBLIC ${Geant4_DEFINITIONS})

install(TARGETS ACTFWGeant4Plugin LIBRARY DESTINATION lib)
install(DIRECTORY include/ACTFW DESTINATION include)

