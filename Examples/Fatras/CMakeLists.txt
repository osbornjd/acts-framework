add_library(ACTFWFatrasCommon STATIC FatrasCommon.cpp FatrasCommon.cpp)

target_include_directories(ACTFWFatrasCommon PUBLIC ${ROOT_INCLUDE_DIRS})
target_include_directories(ACTFWFatrasCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ACTFWFatrasCommon PUBLIC ACTS::ACTSCore)
target_link_libraries(ACTFWFatrasCommon PUBLIC ACTFramework ACTFWExtrapolation ACTFWDigitization ACTFWFatras)
target_link_libraries(ACTFWFatrasCommon PUBLIC ACTFWCsvPlugin ACTFWJsonPlugin ACTFWObjPlugin ACTFWRootPlugin ACTFWBFieldPlugin)

add_executable(ACTFWFatrasGun FatrasGun.cpp)
target_include_directories(ACTFWFatrasGun PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWFatrasGun PUBLIC ACTFWFatrasCommon)
target_link_libraries(ACTFWFatrasGun PUBLIC ACTFWGenericDetector)
target_link_libraries(ACTFWFatrasGun PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFatrasGun RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#add_subdirectory_if(DD4hep USE_DD4HEP)
#add_subdirectory_if(Pythia8 USE_PYTHIA8)
#add_subdirectory_if(Pythia8DD4hep USE_PYTHIA8 AND USE_DD4HEP)
