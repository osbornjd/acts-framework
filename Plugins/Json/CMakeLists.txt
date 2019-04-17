set(srcs src/JsonProtoMaterialReader.cpp)

add_library(ACTFWJsonPlugin SHARED ${srcs})
target_include_directories(ACTFWJsonPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWJsonPlugin PUBLIC ActsCore ActsJsonPlugin) 
target_link_libraries(ACTFWJsonPlugin PUBLIC ACTFramework)

install(TARGETS ACTFWJsonPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
