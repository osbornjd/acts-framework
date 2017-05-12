if (PYTHIA8_FOUND)
  # get list of all source files
  file (GLOB_RECURSE src_files "src/*.*pp")

  # find the ACTS package
  find_package(ACTS REQUIRED COMPONENTS Core)

  # define executable 
  add_executable(ACTFWReadEvgenExample src/ReadEvgenExample.cpp)

  # setup include directories for ExtrapolationTest
  target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTS::ACTSCore)
  target_link_libraries(ACTFWReadEvgenExample PRIVATE ACTFramework ACTFWReadEvgen ACTFWRootPlugin)

  # set installation directories
  install(TARGETS ACTFWReadEvgenExample RUNTIME DESTINATION bin)
else (NOT PYTHIA8_FOUND)
  message (STATUS "${Red}ACTFWReadEvgenExample: PYTHIA8 was not found -> skip building of ReadEvgen example${ColorReset}")
endif (PYTHIA8_FOUND)


