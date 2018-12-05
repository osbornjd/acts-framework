set(srcs include/ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp
         #include/ACTFW/Plugins/Root/RootIndexedMaterialReader.hpp
         #include/ACTFW/Plugins/Root/RootIndexedMaterialWriter.hpp
         #include/ACTFW/Plugins/Root/RootMaterialTrackReader.hpp
         include/ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp
         include/ACTFW/Plugins/Root/RootPropagationStepsWriter.hpp
         include/ACTFW/Plugins/Root/RootParticleWriter.hpp
         include/ACTFW/Plugins/Root/RootSimHitWriter.hpp
         #src/RootIndexedMaterialReader.cpp
         #src/RootIndexedMaterialWriter.cpp
         #src/RootMaterialTrackReader.cpp
         src/RootMaterialTrackWriter.cpp
         src/RootPlanarClusterWriter.cpp
         src/RootParticleWriter.cpp
         src/RootSimHitWriter.cpp
         src/RootPropagationStepsWriter.cpp)


add_library(ACTFWRootPlugin SHARED ${srcs}) 
target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugin PUBLIC ActsCore ActsDigitizationPlugin IdentificationPlugin)
target_link_libraries(ACTFWRootPlugin PUBLIC ACTFramework ACTFWPropagation)
target_link_libraries(ACTFWRootPlugin PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)

install(TARGETS ACTFWRootPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
