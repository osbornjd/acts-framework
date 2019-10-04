add_library(ACTFWDetectorsCommon INTERFACE)
target_include_directories(ACTFWDetectorsCommon INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(ACTFWDetectorsCommon INTERFACE ${Boost_LIBRARIES})

# interface libraries do not exist in the filesystem; no installation needed
