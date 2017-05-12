
# find the ACTS package
find_package(ACTS REQUIRED COMPONENTS Core Examples)


# (1) GENERIC DETECTOR
# define library target
add_executable(ACTFWGenericGeometryExample src/GenericGeometryExample)

# setup linker dependencies
target_include_directories(ACTFWGenericGeometryExample PUBLIC ${DD4hep_INCLUDE_DIRS})  
target_link_libraries(ACTFWGenericGeometryExample PUBLIC ACTS::ACTSCore ACTS::ACTSExamples)
target_link_libraries(ACTFWGenericGeometryExample PRIVATE ACTFramework ACTFWObjPlugin)

# set installation directories
install(TARGETS ACTFWGenericGeometryExample RUNTIME DESTINATION bin)


# (2) DD4HEP DETECTOR
