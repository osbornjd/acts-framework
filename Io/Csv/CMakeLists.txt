add_library(
  ActsFrameworkIoCsv SHARED
  src/CsvOptionsReader.cpp
  src/CsvOptionsWriter.cpp
  src/CsvParticleReader.cpp
  src/CsvParticleWriter.cpp
  src/CsvPlanarClusterReader.cpp
  src/CsvPlanarClusterWriter.cpp
  src/CsvTrackingGeometryWriter.cpp)
target_include_directories(
  ActsFrameworkIoCsv
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ActsFrameworkIoCsv
  PRIVATE
    ACTFramework ActsCore ActsDigitizationPlugin ActsIdentificationPlugin
    Threads::Threads Boost::program_options dfelibs)

install(
  TARGETS ActsFrameworkIoCsv
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
