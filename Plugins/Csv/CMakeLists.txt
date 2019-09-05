add_library(ACTFWCsvPlugin SHARED
  src/CsvParticleReader.cpp
  src/CsvParticleWriter.cpp
  src/CsvPlanarClusterReader.cpp
  src/CsvPlanarClusterWriter.cpp
  src/CsvReader.cpp
  src/CsvTrackingGeometryWriter.cpp
  src/CsvWriterOptions.cpp)
target_include_directories(ACTFWCsvPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWCsvPlugin PRIVATE ACTFramework ActsCore ActsDigitizationPlugin IdentificationPlugin)
target_link_libraries(ACTFWCsvPlugin PRIVATE Threads::Threads)
target_link_libraries(ACTFWCsvPlugin PRIVATE dfelibs)

install(TARGETS ACTFWCsvPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
