# get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# define executable 
add_executable(ACTFWGenericFatrasPGExample src/GenericFatrasPGExample.cpp)
# setup include directories for FatrasPGExample
target_link_libraries(ACTFWGenericFatrasPGExample PRIVATE ACTS::ACTSCore)
target_link_libraries(ACTFWGenericFatrasPGExample PRIVATE ACTFramework ACTFWGenericDetector ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)

# set installation directories
install(TARGETS ACTFWGenericFatrasPGExample RUNTIME DESTINATION bin)


# check if also to build the DD4hep one
if (USE_DD4HEP)
  # define executable 
  add_executable(ACTFWDD4hepFatrasPGExample src/DD4hepFatrasPGExample.cpp)
  # setup link directories for ACTFWDD4hepFatrasPGExample
  target_link_libraries(ACTFWDD4hepFatrasPGExample PRIVATE ACTS::ACTSCore)
  target_link_libraries(ACTFWDD4hepFatrasPGExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWDD4hepPlugin ACTFWReadEvgen ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
  target_link_libraries(ACTFWDD4hepFatrasPGExample PUBLIC ${DD4hep_LIBRARIES})
  # set installation directories
  install(TARGETS ACTFWDD4hepFatrasPGExample RUNTIME DESTINATION bin)  
 endif (USE_DD4HEP)

