add_executable(ActsPythia8 Pythia8.cpp)
target_link_libraries(
  ActsPythia8
  PRIVATE ActsFrameworkGenerators ActsFrameworkPythia8 ACTFWExamplesCommon
    ACTFWRootPlugin ActsFrameworkIoCsv Boost::program_options)

install(
  TARGETS ActsPythia8
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
