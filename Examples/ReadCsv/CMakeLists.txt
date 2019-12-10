add_executable(
  ACTFWGenericReadCsvExample
  GenericReadCsvExample.cpp
  PrintHits.cpp)
target_link_libraries(
  ACTFWGenericReadCsvExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ActsFrameworkIoCsv
    ACTFWGenericDetector Boost::program_options)

install(
  TARGETS ACTFWGenericReadCsvExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
