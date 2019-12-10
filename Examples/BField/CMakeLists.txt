add_executable(ACTFWBFieldExample BFieldExample.cpp)
target_link_libraries(
  ACTFWBFieldExample
  PRIVATE
    ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ActsFrameworkIoRoot Boost::program_options)

add_executable(ACTFWBFieldAccessExample BFieldAccessExample.cpp)
target_link_libraries(
  ACTFWBFieldAccessExample
  PRIVATE
    ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ActsFrameworkIoRoot Boost::program_options)

install(
  TARGETS ACTFWBFieldExample ACTFWBFieldAccessExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
