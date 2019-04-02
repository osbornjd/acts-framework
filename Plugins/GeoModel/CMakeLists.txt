file (GLOB_RECURSE src_files "src/*.cpp")

add_library(ACTFWPluginGeoModel SHARED ${src_files})
target_include_directories(ACTFWPluginGeoModel PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWPluginGeoModel PUBLIC ${GeoModelRead_INCLUDE_DIRS})
target_link_libraries(ACTFWPluginGeoModel PRIVATE ACTFramework ActsCore)
target_link_libraries(ACTFWPluginGeoModel PUBLIC Qt5::Core Qt5::Sql GeoModelDBManager GeoModelRead GeoModelKernel)

install(TARGETS ACTFWPluginGeoModel LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
