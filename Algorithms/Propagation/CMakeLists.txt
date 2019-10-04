add_library(ACTFWPropagation INTERFACE)
target_include_directories(ACTFWPropagation INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(ACTFWPropagation INTERFACE ActsCore ACTFramework)

# interface libraries do not exist in the filesystem; no installation needed
