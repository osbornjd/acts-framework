file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWHitAnalysisExample ${src_files})
target_include_directories(ACTFWHitAnalysisExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWHitAnalysisExample PRIVATE ACTFramework ACTFWHitAnalysis)
target_link_libraries(ACTFWHitAnalysisExample PRIVATE ACTFWFCCedm ACTFWDD4hepPlugin ACTFWRootPlugin)
target_link_libraries(ACTFWHitAnalysisExample PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWHitAnalysisExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWHitAnalysisExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
