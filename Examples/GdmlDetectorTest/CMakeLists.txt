# manually trigger build of Gdml example
option (BUILD_GDML_EXAMPLE "build Gdml example" OFF)

if (BUILD_GDML_EXAMPLE)
  # get list of all source files
  file (GLOB_RECURSE src_files "*/*.*pp")

  # we need root
  find_package (ROOT 6.04.06 QUIET COMPONENTS Geom)

  # find the ACTS package
  find_package(ACTS QUIET COMPONENTS Core TGeoPlugin)
  if (ACTS_FOUND)

    # define library target
    add_executable(ACTFWGdmlDetectorExample ${src_files})

    # setup include directories
    target_include_directories(ACTFWGdmlDetectorExample PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
    target_link_libraries(ACTFWGdmlDetectorExample PUBLIC ACTS::ACTSCore ACTS::ACTSTGeoPlugin )

    #target_include_directories(ACTFWGdmlDetectorExample PUBLIC ACTFWRootPlugins)
    #target_link_libraries(ACTFWGdmlDetectorExample PRIVATE ACTFramework ACTFWRootPlugins)

    # set installation directories
    install(TARGETS ACTFWGdmlDetectorExample RUNTIME DESTINATION bin)
  elseif (NOT ACTS_FOUND)
	message (STATUS “${Red}TGeoPlugin was not found ->skip build of GdmlDetectorExample${ColorReset}”)
  endif(ACTS_FOUND)
endif(BUILD_GDML_EXAMPLE)





