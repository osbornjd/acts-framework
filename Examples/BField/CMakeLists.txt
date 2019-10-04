add_executable(ACTFWBFieldExample src/BFieldExample.cpp)
target_link_libraries(
  ACTFWBFieldExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ACTFWRootPlugin Boost::program_options)

add_executable(ACTFWBFieldAccessExample src/BFieldAccessExample.cpp)
target_link_libraries(
  ACTFWBFieldAccessExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ACTFWRootPlugin Boost::program_options)

install(
  TARGETS ACTFWBFieldExample ACTFWBFieldAccessExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
