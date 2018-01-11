file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWParticleGun SHARED ${src_files})
target_include_directories(ACTFWParticleGun PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWParticleGun PUBLIC ACTSCore ACTFramework)

install(TARGETS ACTFWParticleGun LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
