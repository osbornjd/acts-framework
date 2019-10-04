add_library(
  ACTFWBFieldPlugin SHARED
  src/BFieldOptions.cpp
  src/BFieldScalor.cpp
  src/BFieldUtils.cpp)
target_include_directories(
  ACTFWBFieldPlugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> ${ROOT_INCLUDE_DIRS})
target_link_libraries(
  ACTFWBFieldPlugin
  PUBLIC ActsCore ACTFramework ${ROOT_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(
  TARGETS ACTFWBFieldPlugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
