add_library(ACTFWFitting INTERFACE)
target_include_directories(ACTFWFitting INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(ACTFWFitting INTERFACE ActsCore ACTFramework)

# interface libraries do not exist in the filesystem; no installation needed
