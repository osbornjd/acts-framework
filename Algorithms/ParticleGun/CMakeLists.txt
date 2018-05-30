file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWParticleGun SHARED ${src_files})
target_include_directories(ACTFWParticleGun PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
<<<<<<< HEAD
target_link_libraries(ACTFWParticleGun PUBLIC Acts::Core ACTFramework)
=======
target_link_libraries(ACTFWParticleGun PUBLIC ACTSCore FatrasCore ACTFramework)
>>>>>>> 7bb1efe... update to new Propagator

install(TARGETS ACTFWParticleGun LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
