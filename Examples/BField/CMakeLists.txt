add_executable(ACTFWBFieldExample src/BFieldExample.cpp)
target_include_directories (ACTFWBFieldExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWBFieldExample PRIVATE Acts::Core)
target_link_libraries(ACTFWBFieldExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin)
target_link_libraries(ACTFWBFieldExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWBFieldExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(ACTFWBFieldAccessExample src/BFieldAccessExample.cpp)
target_include_directories (ACTFWBFieldAccessExample PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(ACTFWBFieldAccessExample PRIVATE Acts::Core)
target_link_libraries(ACTFWBFieldAccessExample PRIVATE ACTFramework ACTFWExamplesCommon ACTFWBFieldPlugin)
target_link_libraries(ACTFWBFieldAccessExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWBFieldAccessExample RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
