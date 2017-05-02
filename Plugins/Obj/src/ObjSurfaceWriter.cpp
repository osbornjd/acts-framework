#include <iostream>
#include "ACTFW/Obj/ObjSurfaceWriter.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Surfaces/SurfaceBounds.hpp"
#include "ACTS/Surfaces/PlanarBounds.hpp"


FWObj::ObjSurfaceWriter::ObjSurfaceWriter(
    const FWObj::ObjSurfaceWriter::Config& cfg)
  : FW::ISurfaceWriter()
  , m_cfg(cfg)
  , m_nvertices(0)    
{}

FWObj::ObjSurfaceWriter::~ObjSurfaceWriter()
{
}

FW::ProcessCode
FWObj::ObjSurfaceWriter::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWObj::ObjSurfaceWriter::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWObj::ObjSurfaceWriter::write(const Acts::Surface& surface)
{
  if (!m_cfg.outputStream)   return FW::ProcessCode::SUCCESS;
    
  // count the vertices
  unsigned int vcounter = 0;
  // let's get the bounds & the transform
  const Acts::SurfaceBounds& surfaceBounds = surface.bounds();
  auto surfaceTransform = surface.transform();
  // dynamic_cast to PlanarBounds
  const Acts::PlanarBounds* planarBounds = 
    dynamic_cast<const Acts::PlanarBounds*>(&surfaceBounds);
  // only continue if the cast worked
  // 
  if (planarBounds){
    // start a new section
    (*(m_cfg.outputStream)) << std::endl;
    (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
    // get the vertices
    auto planarVertices = planarBounds->vertices();
    // loop over the vertices
    for (auto vertex : planarVertices){
      // the counter
      ++vcounter;
      // get the point in 3D
      Acts::Vector3D v3D(surfaceTransform*Acts::Vector3D(vertex.x(),
                                                         vertex.y(),
                                                         0.));
      // write out the vertex 
      (*(m_cfg.outputStream)) << "v " << m_cfg.outputScalor*v3D.x() << " " 
                                      << m_cfg.outputScalor*v3D.y() << " "
                                      << m_cfg.outputScalor*v3D.z() << '\n'; 
      
    }
    // output to file
    (*(m_cfg.outputStream)) << "f ";
    for (size_t vc = 0; vc < vcounter; ++vc )
      (*(m_cfg.outputStream)) <<  ++m_nvertices << " ";
    // close the line and make a new line
    (*(m_cfg.outputStream)) << '\n';
    
  }
  (*(m_cfg.outputStream)) << std::endl;
  (*(m_cfg.outputStream)) << std::endl;

  return FW::ProcessCode::SUCCESS;
}


