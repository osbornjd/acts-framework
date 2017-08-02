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
  ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialStep.hpp
  ${ACTS_INCLUDE_DIR}/ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp
  ${ACTS_INCLUDE_DIR}/ACTS/Material/Material.hpp
  LINKDEF include/ACTFW/Plugins/Root/Material_LinkDef.h
  OPTIONS -I/${ACTS_INCLUDE_DIR})

add_library(ACTFWRootPlugin SHARED ${srcs} G__Material.cxx)
target_include_directories(ACTFWRootPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_include_directories(ACTFWRootPlugin PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(ACTFWRootPlugin PUBLIC ACTS::ACTSCore ACTS::ACTSMaterialPlugin)
target_link_libraries(ACTFWRootPlugin PUBLIC ACTFramework)
<<<<<<< HEAD
target_link_libraries(ACTFWRootPlugin PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)
=======
target_link_libraries(ACTFWRootPlugin PUBLIC ROOT::Core ROOT::Hist ROOT::Tree ROOT::TreePlayer)
target_link_libraries(ACTFWRootPlugin PUBLIC Threads::Threads)
if(USE_PYTHIA8)
  target_include_directories(ACTFWRootPlugin PUBLIC ${PYTHIA8_INCLUDEDIR})
  target_link_libraries(ACTFWRootPlugin PUBLIC -L${PYTHIA8_LIBRARYDIR})
  target_link_libraries(ACTFWRootPlugin PUBLIC ROOT::EGPythia8)
endif()
>>>>>>> 6df828c... DD4hep v. 1.00.01 changes to code

install(TARGETS ACTFWRootPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
