add_library(ACTFWGeometry INTERFACE)
target_include_directories(ACTFWGeometry INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWGeometry INTERFACE Acts::Core ACTFramework)

install(TARGETS ACTFWGeometry LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})