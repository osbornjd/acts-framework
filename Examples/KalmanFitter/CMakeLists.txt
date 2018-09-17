#file(GLOB_RECURSE src_files "*.cpp" "*.*pp")

#add_executable(ACTFWKalmanExample ${src_files})
  add_executable(ACTFWKalmanExample KalmanFilterTests.cpp)
target_include_directories(ACTFWKalmanExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWKalmanExample PRIVATE ACTFramework ACTFWGenericDetector)
target_link_libraries(ACTFWKalmanExample PUBLIC ACTSCore ACTSLegacy)
target_link_libraries(ACTFWKalmanExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWKalmanExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

