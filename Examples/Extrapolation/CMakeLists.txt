#get list of all source files
file (GLOB_RECURSE src_files "src/*.*pp")

# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)

# define executable 
add_executable(ACTFWGenericExtrapolationExample src/GenericExtrapolationExample.cpp)
# setup link directories for ACTFWExtrapolationExample
target_link_libraries(ACTFWGenericExtrapolationExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWGenericExtrapolationExample PRIVATE ACTFramework ACTFWRootPlugin ACTFWExtrapolation)
# set installation directories
install(TARGETS ACTFWGenericExtrapolationExample RUNTIME DESTINATION bin)

# check if also to build the DD4hep one
find_package(DD4hep COMPONENTS DDCore DDG4)
if (DD4hep_FOUND)
  # define executable 
  add_executable(ACTFWDD4hepExtrapolationExample src/DD4hepExtrapolationExample.cpp)
  # setup link directories for ACTFWExtrapolationExample
  target_link_libraries(ACTFWDD4hepExtrapolationExample PRIVATE ACTS::ACTSCore ACTS::ACTSExamples)
  target_link_libraries(ACTFWDD4hepExtrapolationExample PRIVATE ACTFWDD4hepPlugin ACTFramework ACTFWRootPlugin ACTFWExtrapolation)
  target_link_libraries(ACTFWDD4hepExtrapolationExample PUBLIC ${DD4hep_LIBRARIES})
  # set installation directories
  install(TARGETS ACTFWDD4hepExtrapolationExample RUNTIME DESTINATION bin)  
endif (DD4hep_FOUND)


