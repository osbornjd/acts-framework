include(${ROOT_USE_FILE})

file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library (ACTFWFCCedm SHARED ${src_files})
target_include_directories(ACTFWFCCedm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWFCCedm PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWFCCedm PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWFCCedm PUBLIC ACTFramework)
target_link_libraries(ACTFWFCCedm PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWFCCedm PUBLIC Threads::Threads)
target_include_directories(ACTFWFCCedm PUBLIC ${PODIO_INCLUDE_DIRS})
target_link_libraries(ACTFWFCCedm PUBLIC ${PODIO_LIBRARIES})
target_include_directories(ACTFWFCCedm PUBLIC ${FCCEDM_INCLUDE_DIRS})
target_link_libraries(ACTFWFCCedm PUBLIC ${FCCEDM_LIBRARIES})


install(TARGETS ACTFWFCCedm LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
