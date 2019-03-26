add_library(ACTFWReconstruction SHARED
    ACTFW/Reconstruction/Empty.cpp)
target_include_directories(
    ACTFWReconstruction
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWReconstruction PRIVATE ACTFramework ActsCore)

install(TARGETS ACTFWReconstruction LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
