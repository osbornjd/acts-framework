file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWWhiteBoardExample ${src_files})
target_link_libraries(ACTFWWhiteBoardExample PRIVATE ACTFramework)
target_link_libraries(ACTFWWhiteBoardExample PUBLIC ACTS::ACTSCore)

install(TARGETS ACTFWWhiteBoardExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
