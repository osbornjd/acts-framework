file(GLOB_RECURSE src_files "src/EventToTrackConverter.cpp")

add_library(ACTFWEventToTrackConverter SHARED ${src_files})
target_include_directories(ACTFWEventToTrackConverter PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWEventToTrackConverter PUBLIC ActsCore)
target_link_libraries(ACTFWEventToTrackConverter PUBLIC ACTFramework)


install(TARGETS ACTFWEventToTrackConverter LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
