file(GLOB_RECURSE src_files "src/*.cpp")

add_library(ACTFWTGeoDetector SHARED ${src_files})
target_include_directories(ACTFWTGeoDetector PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_link_libraries(ACTFWTGeoDetector PUBLIC ActsCore IdentificationPlugin ActsDigitizationPlugin ACTFramework ACTFWGenericDetector ACTFWBFieldPlugin ACTFWDetectorsCommon ActsTGeoPlugin)
install(DIRECTORY include/ACTFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
