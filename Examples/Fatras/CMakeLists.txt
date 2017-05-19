# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define executable 
add_executable(ACTFWGenericFatrasExample src/GenericFatrasExample.cpp)
# setup include directories for FatrasExample
target_include_directories(ACTFWGenericFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_include_directories(ACTFWGenericFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
target_include_directories(ACTFWGenericFatrasExample PRIVATE ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin)
target_link_libraries(ACTFWGenericFatrasExample PRIVATE ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)

# set installation directories
install(TARGETS ACTFWGenericFatrasExample RUNTIME DESTINATION bin)


# check if also to build the DD4hep one
find_package(DD4hep COMPONENTS DDCore DDG4)
if (DD4hep_FOUND)
  # define executable 
  add_executable(ACTFWDD4hepFatrasExample src/DD4hepFatrasExample.cpp)
  # setup link directories for ACTFWFatrasExample
  # setup include directories for FatrasExample
  target_include_directories(ACTFWDD4hepFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
  target_include_directories(ACTFWDD4hepFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWDD4hepPlugin)
  target_include_directories(ACTFWDD4hepFatrasExample PRIVATE ACTWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
  target_link_libraries(ACTFWDD4hepFatrasExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
  target_link_libraries(ACTFWDD4hepFatrasExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWDD4hepPlugin)
  target_link_libraries(ACTFWDD4hepFatrasExample PRIVATE ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
  target_link_libraries(ACTFWDD4hepFatrasExample PUBLIC ${DD4hep_LIBRARIES})
  # set installation directories
  install(TARGETS ACTFWDD4hepFatrasExample RUNTIME DESTINATION bin)  
endif (DD4hep_FOUND)

