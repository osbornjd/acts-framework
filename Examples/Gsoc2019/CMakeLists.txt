add_executable(
    ACTFWGsoc2019Reconstruction Reconstruction.cpp)
target_link_libraries(
    ACTFWGsoc2019Reconstruction
    PRIVATE ACTFramework ACTFWReconstruction ACTFWExamplesCommon
        ActsFrameworkGenerators ActsCore
        ACTFWGenericDetector
        Boost::program_options)
install(
    TARGETS ACTFWGsoc2019Reconstruction
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
