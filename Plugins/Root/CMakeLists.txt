include(${ROOT_USE_FILE})

set(srcs include/ACTFW/Plugins/Root/RootExCellWriter.hpp
         include/ACTFW/Plugins/Root/RootExCellWriter.ipp
         include/ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp
         include/ACTFW/Plugins/Root/RootIndexedMaterialReader.hpp
         include/ACTFW/Plugins/Root/RootIndexedMaterialWriter.hpp
         include/ACTFW/Plugins/Root/RootMaterialTrackReader.hpp
         include/ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp
         include/ACTFW/Plugins/Root/RootParticleWriter.hpp
         src/RootIndexedMaterialReader.cpp
         src/RootIndexedMaterialWriter.cpp
         src/RootMaterialTrackReader.cpp
         src/RootMaterialTrackWriter.cpp
         src/RootPlanarClusterWriter.cpp
         src/RootParticleWriter.cpp)
# generate root dictionary for material mapping - needed to write this class to a root file
root_generate_dictionary(
  G__Material
  ${PROJECT_SOURCE_DIR}/external/acts-core/Core/include/ACTS/Material/Material.hpp
  ${PROJECT_SOURCE_DIR}/external/acts-core/Plugins/MaterialPlugin/include/ACTS/Plugins/MaterialPlugins/MaterialStep.hpp
  ${PROJECT_SOURCE_DIR}/external/acts-core/Plugins/MaterialPlugin/include/ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp
  LINKDEF include/ACTFW/Plugins/Root/Material_LinkDef.h
  OPTIONS -I${PROJECT_SOURCE_DIR}/external/acts-core/Core/include
          -I${PROJECT_SOURCE_DIR}/external/acts-core/Plugins/MaterialPlugin/include)

add_library(ACTFWRootPlugin SHARED ${srcs} G__Material.cxx)
target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugin PUBLIC ACTSCore ACTSMaterialPlugin)
target_link_libraries(ACTFWRootPlugin PUBLIC ACTFramework)
target_link_libraries(ACTFWRootPlugin PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)

install(TARGETS ACTFWRootPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
