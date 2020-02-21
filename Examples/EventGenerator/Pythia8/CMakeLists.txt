add_executable(
  ActsGenPythia8
  Pythia8.cpp)
target_link_libraries(
  ActsGenPythia8
  PRIVATE
    ActsFrameworkGenerators ActsFrameworkPythia8 ACTFWExamplesCommon
    ActsFrameworkIoRoot ActsFrameworkIoCsv ActsFrameworkPrinters
    Boost::program_options)

install(
  TARGETS ActsGenPythia8
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
