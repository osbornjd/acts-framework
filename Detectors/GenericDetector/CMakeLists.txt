file(GLOB_RECURSE src_files "src/*.*pp")

add_library(ACTFWGenericericDetector SHARED ${src_files})
target_include_directories(ACTFWGenericericDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWGenericericDetector PUBLIC Acts::Core Acts::DigitizationPlugin ACTFramework)

install(TARGETS ACTFWGenericericDetector LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
