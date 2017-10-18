file (GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWPropagationExample src/PropagationExample.cpp)
target_include_directories(ACTFWPropagationExample PRIVATE ACTFWPropagation ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWPropagationExample PRIVATE ACTS::ACTSCore)
target_link_libraries(ACTFWPropagationExample PRIVATE ACTFWPropagation ACTFramework ACTFWParticleGun)
target_link_libraries(ACTFWPropagationExample PRIVATE ACTFWRootPlugin ACTFWBFieldPlugin)
target_link_libraries(ACTFWPropagationExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWPropagationExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

