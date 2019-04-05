add_library(ACTFWTGeoDetector INTERFACE)
target_include_directories(ACTFWTGeoDetector INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWTGeoDetector INTERFACE ActsCore ActsTGeoPlugin)
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
