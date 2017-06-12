# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

  # define executable 
  add_executable(ACTFWGenericFatrasPGExample src/GenericFatrasPGExample.cpp)
  # setup include directories for FatrasPGExample
  target_include_directories(ACTFWGenericFatrasPGExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
  target_include_directories(ACTFWGenericFatrasPGExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
  target_include_directories(ACTFWGenericFatrasPGExample PRIVATE ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
  target_link_libraries(ACTFWGenericFatrasPGExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
  target_link_libraries(ACTFWGenericFatrasPGExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
  target_link_libraries(ACTFWGenericFatrasPGExample PRIVATE ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)

  # set installation directories
  install(TARGETS ACTFWGenericFatrasPGExample RUNTIME DESTINATION bin)


  # check if also to build the DD4hep one
  find_package(DD4hep COMPONENTS DDCore DDG4)
  if (DD4hep_FOUND)
    # define executable 
    add_executable(ACTFWDD4hepFatrasPGExample src/DD4hepFatrasPGExample.cpp)
    # setup link directories for ACTFWFatrasPGExample
    # setup include directories for FatrasPGExample
    target_include_directories(ACTFWDD4hepFatrasPGExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
    target_include_directories(ACTFWDD4hepFatrasPGExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWDD4hepPlugin)
    target_include_directories(ACTFWDD4hepFatrasPGExample PRIVATE ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
    target_link_libraries(ACTFWDD4hepFatrasPGExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
    target_link_libraries(ACTFWDD4hepFatrasPGExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWDD4hepPlugin)
    target_link_libraries(ACTFWDD4hepFatrasPGExample PRIVATE ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
    target_link_libraries(ACTFWDD4hepFatrasPGExample PUBLIC ${DD4hep_LIBRARIES})
    # set installation directories
    install(TARGETS ACTFWDD4hepFatrasPGExample RUNTIME DESTINATION bin)  
  endif (DD4hep_FOUND)

