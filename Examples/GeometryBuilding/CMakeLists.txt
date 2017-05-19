
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
find_package(DD4hep COMPONENTS DDCore DDG4)
if (DD4hep_FOUND)
  add_executable(ACTFWDD4hepGeometryExample src/DD4hepGeometryExample.cpp)  
  target_include_directories(ACTFWDD4hepGeometryExample PRIVATE ${DD4hep_INCLUDE_DIRS}) 
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC ACTS::ACTSCore)  
  target_link_libraries(ACTFWDD4hepGeometryExample PRIVATE ACTFWDD4hepPlugin ACTFramework ACTFWObjPlugin)
  target_link_libraries(ACTFWDD4hepGeometryExample PUBLIC ${DD4hep_LIBRARIES})
  install(TARGETS ACTFWDD4hepGeometryExample RUNTIME DESTINATION bin)
endif (DD4hep_FOUND)

