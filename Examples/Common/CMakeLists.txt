add_library(ACTFWExamplesCommon INTERFACE)
target_include_directories(ACTFWExamplesCommon INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWExamplesCommon INTERFACE Acts::Core ACTFramework)

install(TARGETS ACTFWExamplesCommon LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})