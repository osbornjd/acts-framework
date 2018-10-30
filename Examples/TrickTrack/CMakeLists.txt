file (GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWTrickTrackExample src/TrickTrackExample.cpp)
target_include_directories(ACTFWTrickTrackExample PRIVATE ${tricktrack_INCLUDE_DIR})
target_include_directories(ACTFWTrickTrackExample PRIVATE ${Boost_INCLUDE_DIRS}  )
target_link_libraries(ACTFWTrickTrackExample PRIVATE ActsCore)
target_link_libraries(ACTFWTrickTrackExample PRIVATE ACTFramework ACTFWExamplesCommon
ACTFWGenericDetector ACTFWExtrapolation ACTFWSeeding)
target_link_libraries(ACTFWTrickTrackExample PRIVATE ACTFWRootPlugin ACTFWObjPlugin ACTFWBFieldPlugin)
target_link_libraries(ACTFWTrickTrackExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWTrickTrackExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


