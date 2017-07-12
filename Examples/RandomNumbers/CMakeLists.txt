file (GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWRandomNumberExample ${src_files})
target_link_libraries(ACTFWRandomNumberExample PRIVATE ACTFramework)
target_link_libraries(ACTFWRandomNumberExample PUBLIC ACTS::ACTSCore)

install(TARGETS ACTFWRandomNumberExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
