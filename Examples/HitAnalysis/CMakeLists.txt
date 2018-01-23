file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_executable(ACTFWFCCHitAnalysisExample ${src_files})
target_include_directories(ACTFWFCCHitAnalysisExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWFCCHitAnalysisExample PRIVATE ACTFramework ACTFWHitAnalysis)
target_link_libraries(ACTFWFCCHitAnalysisExample PRIVATE ACTFWFCCedm ACTFWDD4hepPlugin ACTFWRootPlugin)
target_link_libraries(ACTFWFCCHitAnalysisExample PUBLIC ACTSCore)
target_link_libraries(ACTFWFCCHitAnalysisExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFCCHitAnalysisExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
