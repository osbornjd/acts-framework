add_executable(ACTFWFatrasPythia8 FatrasPythia8.cpp)
target_include_directories (ACTFWFatrasPythia8 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWFatrasPythia8 PUBLIC ACTFWParticleGunCommon)
target_link_libraries(ACTFWFatrasPythia8 PUBLIC ACTFWReadEvgen ACTFWPluginPythia8)
target_link_libraries(ACTFWFatrasPythia8 PUBLIC ACTFWGenericDetector)
target_link_libraries(ACTFWFatrasPythia8 PUBLIC ${Boost_LIBRARIES})

install(TARGETS ACTFWFatrasPythia8 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(ACTFWTrackingML TrackingML.cpp)
target_include_directories (ACTFWTrackingML PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWTrackingML PUBLIC ACTFWParticleGunCommon)
target_link_libraries(ACTFWTrackingML PUBLIC ACTFWReadEvgen ACTFWPluginPythia8)
target_link_libraries(ACTFWTrackingML PUBLIC ACTFWGenericDetector)
target_link_libraries(ACTFWTrackingML PUBLIC ${Boost_LIBRARIES})

install(TARGETS ACTFWTrackingML RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
