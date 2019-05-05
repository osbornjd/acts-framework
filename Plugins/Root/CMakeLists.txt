Examples/Propagation/src/detail/PropagationExampleBase.hppset(srcs include/ACTFW/Plugins/Root/RootExCellWriter.hpp
         include/ACTFW/Plugins/Root/RootExCellWriter.ipp
         include/ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp
         include/ACTFW/Plugins/Root/RootMaterialReader.hpp
         include/ACTFW/Plugins/Root/RootMaterialWriter.hpp
         include/ACTFW/Plugins/Root/RootMaterialTrackReader.hpp
         include/ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp
         include/ACTFW/Plugins/Root/RootPropagationStepsWriter.hpp
         include/ACTFW/Plugins/Root/RootParticleWriter.hpp
         include/ACTFW/Plugins/Root/RootSimHitWriter.hpp
         src/RootMaterialReader.cpp
         src/RootMaterialWriter.cpp
         src/RootMaterialTrackReader.cpp
         src/RootMaterialTrackWriter.cpp
         src/RootPlanarClusterWriter.cpp
         src/RootParticleWriter.cpp
         src/RootSimHitWriter.cpp
         src/RootPropagationStepsWriter.cpp)

add_library(ACTFWRootPlugin SHARED ${srcs})
target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugin PUBLIC ActsCore ActsDigitizationPlugin IdentificationPlugin) # ActsMaterialPlugin)
target_link_libraries(ACTFWRootPlugin PUBLIC ACTFramework ACTFWPropagation)
target_link_libraries(ACTFWRootPlugin PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)

add_library(ACTFWRootPlugin SHARED ${sources})
target_include_directories(
  ACTFWRootPlugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> ${ROOT_INCLUDE_DIRS})
target_link_libraries(
  ACTFWRootPlugin
  PUBLIC ActsCore ActsDigitizationPlugin IdentificationPlugin
         ACTFramework ACTFWPropagation ${ROOT_LIBRARIES} Threads::Threads)
install(TARGETS ACTFWRootPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
