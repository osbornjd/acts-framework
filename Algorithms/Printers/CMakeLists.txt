add_library(
  ActsFrameworkPrinters SHARED
  ACTFW/Printers/PrintHits.cpp
  ACTFW/Printers/PrintParticles.cpp)
target_include_directories(
  ActsFrameworkPrinters
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
  ActsFrameworkPrinters
  PUBLIC ActsCore ActsDigitizationPlugin ACTFramework)

install(
  TARGETS ActsFrameworkPrinters
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
