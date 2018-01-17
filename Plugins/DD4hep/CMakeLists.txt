file (GLOB_RECURSE src_files "src/*.cpp")

add_library (ACTFWDD4hepPlugin SHARED ${src_files})
target_include_directories(ACTFWDD4hepPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWDD4hepPlugin PUBLIC ${DD4hep_INCLUDE_DIRS})
target_include_directories(ACTFWDD4hepPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWDD4hepPlugin PUBLIC ACTSCore ACTSDD4hepPlugin)
target_link_libraries(ACTFWDD4hepPlugin PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWDD4hepPlugin PUBLIC ${DD4hep_LIBRARIES})
target_link_libraries(ACTFWDD4hepPlugin PRIVATE ACTFramework)

dd4hep_set_version(ACTFWDD4hepPlugin MAJOR 1 MINOR 0 PATCH 0)

install(TARGETS ACTFWDD4hepPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
