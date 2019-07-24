# shared code comes first
add_subdirectory(Common)

add_subdirectory(BField)
add_subdirectory(EventGenerator)
add_subdirectory_if(Fatras USE_PYTHIA8)
add_subdirectory(Geometry)
add_subdirectory(HelloWorld)
add_subdirectory(MaterialMapping)
add_subdirectory(Propagation)
add_subdirectory(RandomNumbers)
add_subdirectory_if(VertexFitter USE_PYTHIA8)
add_subdirectory(WhiteBoard)
add_subdirectory_if(HepMCExample USE_HEPMC3)
