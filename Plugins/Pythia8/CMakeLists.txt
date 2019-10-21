add_library(ActsFrameworkPythia8 SHARED
  ACTFW/Generators/Pythia8ProcessGenerator.cpp
  ACTFW/Options/Pythia8Options.cpp)

target_include_directories(ActsFrameworkPythia8 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(ActsFrameworkPythia8 PRIVATE
  ActsCore
  ActsFrameworkGenerators
  ACTFramework
  Pythia8
  Boost::program_options)
target_link_libraries(ActsFrameworkPythia8 PUBLIC Pythia8)

install(TARGETS ActsFrameworkPythia8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
