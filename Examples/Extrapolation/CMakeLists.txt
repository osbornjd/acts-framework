file (GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWGenericExtrapolationExample src/GenericExtrapolationExample.cpp)
target_link_libraries(ACTFWGenericExtrapolationExample PRIVATE ACTS::ACTSCore)
target_link_libraries(ACTFWGenericExtrapolationExample PRIVATE ACTFramework ACTFWGenericDetector ACTFWRootPlugin ACTFWExtrapolation ACTFWFatras)

install(TARGETS ACTFWGenericExtrapolationExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(USE_DD4HEP)
  add_executable(ACTFWDD4hepExtrapolationExample src/DD4hepExtrapolationExample.cpp)
  target_link_libraries(ACTFWDD4hepExtrapolationExample PRIVATE ACTS::ACTSCore)
  target_link_libraries(ACTFWDD4hepExtrapolationExample PRIVATE ACTFWDD4hepPlugin ACTFWGenericDetector ACTFramework ACTFWRootPlugin ACTFWExtrapolation ACTFWFatras)
  target_link_libraries(ACTFWDD4hepExtrapolationExample PUBLIC ${DD4hep_LIBRARIES})

  install(TARGETS ACTFWDD4hepExtrapolationExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
