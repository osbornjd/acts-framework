add_library(
  ACTFWBFieldPlugin SHARED
  src/BFieldOptions.cpp
  src/BFieldScalor.cpp
  src/BFieldUtils.cpp)
target_include_directories(
  ACTFWBFieldPlugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWBFieldPlugin
  PUBLIC ActsCore ACTFramework Boost::program_options ROOT::Core ROOT::Tree)

install(
  TARGETS ACTFWBFieldPlugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})