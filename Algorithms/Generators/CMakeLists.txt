add_library(
  ActsFrameworkGenerators SHARED
  ACTFW/Generators/EventGenerator.cpp
  ACTFW/Generators/FlattenEvent.cpp
  ACTFW/Generators/ParametricProcessGenerator.cpp
  ACTFW/Generators/ParticleSelector.cpp
  ACTFW/Options/ParticleGunOptions.cpp)
target_include_directories(
  ActsFrameworkGenerators
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  ActsFrameworkGenerators
  PUBLIC ActsCore ACTFramework Boost::program_options)

install(
  TARGETS ActsFrameworkGenerators
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
