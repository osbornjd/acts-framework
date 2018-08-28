add_library(ACTFWGeometryInterfaces INTERFACE)
target_include_directories(ACTFWGeometryInterfaces INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWGeometryInterfaces INTERFACE Acts::Core Acts::TGeoPlugin)
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})