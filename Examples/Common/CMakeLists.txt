add_library(ACTFWxamplesCommon INTERFACE)
target_include_directories(ACTFWxamplesCommon INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWxamplesCommon INTERFACE ActsCore ACTFramework)

install(TARGETS ACTFWxamplesCommon LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})