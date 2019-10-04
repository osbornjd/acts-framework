add_executable(ACTFWBFieldExample src/BFieldExample.cpp)
target_include_directories(
  ACTFWBFieldExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWBFieldExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ACTFWRootPlugin ${Boost_LIBRARIES})

add_executable(ACTFWBFieldAccessExample src/BFieldAccessExample.cpp)
target_include_directories(
  ACTFWBFieldAccessExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWBFieldAccessExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin
    ACTFWRootPlugin ${Boost_LIBRARIES})

install(
  TARGETS ACTFWBFieldExample ACTFWBFieldAccessExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
