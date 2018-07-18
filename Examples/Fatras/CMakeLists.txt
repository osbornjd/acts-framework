file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWGenericFatrasExample src/GenericFatrasExample.cpp)
target_include_directories(ACTFWGenericFatrasExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWGenericFatrasExample PRIVATE Acts::Core Acts::Legacy Fatras::Core)

target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWFatras ACTFWReadEvgen ACTFWParticleGun)
target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTFWBFieldPlugin ACTFWRootPlugin ACTFWCsvPlugin ACTFWPluginPythia8)
target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTFWGenericDetector)

target_link_libraries(ACTFWGenericFatrasExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWGenericFatrasExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


add_executable(ACTFWRootFatrasExample src/RootFatrasExample.cpp)
target_include_directories(ACTFWRootFatrasExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWRootFatrasExample PRIVATE Acts::Core Acts::Legacy Fatras::Core)

target_link_libraries(ACTFWRootFatrasExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWFatras ACTFWReadEvgen ACTFWParticleGun)
target_link_libraries(ACTFWRootFatrasExample PRIVATE ACTFWBFieldPlugin ACTFWRootPlugin ACTFWCsvPlugin ACTFWPluginPythia8)
target_link_libraries(ACTFWRootFatrasExample PRIVATE ACTFWRootDetector)

target_link_libraries(ACTFWRootFatrasExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWRootFatrasExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
