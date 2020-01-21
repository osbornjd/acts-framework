add_executable(
  ACTFWGenericReadCsvExample
  GenericReadCsvExample.cpp)
target_link_libraries(
  ACTFWGenericReadCsvExample
  PRIVATE ActsCore ACTFramework ACTFWExamplesCommon ActsFrameworkIoCsv
    ActsFrameworkPrinters ACTFWGenericDetector Boost::program_options)

install(
  TARGETS ACTFWGenericReadCsvExample
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
