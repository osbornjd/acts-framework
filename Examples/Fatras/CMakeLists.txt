file(GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWFatrasExample src/GenericFatrasExample.cpp)
target_include_directories(ACTFWFatrasExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWFatrasExample PRIVATE Acts::Core Fatras::Core)

target_link_libraries(ACTFWFatrasExample PRIVATE ACTFramework ACTFWFatras ACTFWReadEvgen ACTFWParticleGun)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFWBFieldPlugin ACTFWRootPlugin ACTFWCsvPlugin ACTFWPluginPythia8)
target_link_libraries(ACTFWFatrasExample PRIVATE ACTFWGeometry ACTFWGenericDetector)

target_link_libraries(ACTFWFatrasExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFatrasExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
