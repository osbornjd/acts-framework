file (GLOB_RECURSE src_files "src/*.*pp")

add_executable(ACTFWFatrasValidationExample src/FatrasValidation.cpp)

target_link_libraries(ACTFWFatrasValidationExample PRIVATE ACTSCore)
target_link_libraries(ACTFWFatrasValidationExample PRIVATE ACTFramework ACTFWDD4hepPlugin ACTFWDD4hepG4Plugin ACTFWFatrasValidation)
target_link_libraries(ACTFWFatrasValidationExample PRIVATE ${DD4hep_LIBRARIES})
target_link_libraries(ACTFWFatrasValidationExample PRIVATE ${Boost_LIBRARIES})

install(TARGETS ACTFWFatrasValidationExample RUNTIME DESTINATION bin)
