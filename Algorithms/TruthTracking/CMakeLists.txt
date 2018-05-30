add_library(ACTFWTruthTracking INTERFACE)
target_include_directories(ACTFWTruthTracking INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWTruthTracking INTERFACE ACTS::ACTSCore ACTFramework)

install(TARGETS ACTFWTruthTracking LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})