add_executable(
  ACTFWGenericReadCsvExample
  GenericReadCsvExample.cpp
  PrintHits.cpp)
target_include_directories(
  ACTFWGenericReadCsvExample
  PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(
  ACTFWGenericReadCsvExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ACTFWCsvPlugin
    ACTFWGenericDetector ${Boost_LIBRARIES})

install(
  TARGETS ACTFWGenericReadCsvExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
