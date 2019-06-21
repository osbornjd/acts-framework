file(GLOB_RECURSE src_files "src/*.cpp" "include/*.*pp")

add_library(ACTFWTxtPlugin SHARED ${src_files})
target_include_directories(ACTFWTxtPlugin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWTxtPlugin PRIVATE ACTFramework ActsCore ActsDigitizationPlugin IdentificationPlugin)
target_link_libraries(ACTFWTxtPlugin PRIVATE Threads::Threads)

install(TARGETS ACTFWTxtPlugin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
