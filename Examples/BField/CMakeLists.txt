file (GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWBFieldExample src/BFieldExample.cpp)
target_include_directories (ACTFWBFieldExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWBFieldExample PRIVATE ACTS::ACTSCore)
target_link_libraries(ACTFWBFieldExample PRIVATE ACTFramework ACTFWBFieldPlugin)
target_link_libraries(ACTFWBFieldExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWBFieldExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
