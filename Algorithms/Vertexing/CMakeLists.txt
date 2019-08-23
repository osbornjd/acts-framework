add_library(ACTFWVertexing SHARED src/VertexFitAlgorithm.cpp src/VertexFindingAlgorithm.cpp)
target_include_directories(ACTFWVertexing PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWVertexing PRIVATE ActsFrameworkTruthTracking)
target_link_libraries(ACTFWVertexing PUBLIC ActsCore)
target_link_libraries(ACTFWVertexing PUBLIC ACTFramework)

install(TARGETS ACTFWVertexing LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
