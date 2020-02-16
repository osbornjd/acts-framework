add_library(
  ACTFWObjPlugin SHARED
  src/ObjHelper.cpp
  src/ObjSurfaceWriter.cpp
  src/ObjTrackingGeometryWriter.cpp)
target_include_directories(
  ACTFWObjPlugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ACTFWObjPlugin
  PUBLIC ActsCore ACTFramework Threads::Threads)

install(
  TARGETS ACTFWObjPlugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
