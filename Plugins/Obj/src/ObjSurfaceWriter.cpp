#include <iostream>
#include "ACTFW/Obj/ObjSurfaceWriter.hpp"
#include "ACTS/Surfaces/SurfaceBounds.hpp"
#include "ACTS/Surfaces/CylinderBounds.hpp"
#include "ACTS/Surfaces/PlanarBounds.hpp"

FWObj::ObjSurfaceWriter::ObjSurfaceWriter(
    const FWObj::ObjSurfaceWriter::Config& cfg)
  : FW::IWriterT<Acts::Surface>()
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
  // dynamic cast to CylinderBounds or disc bounds work the same 
  const Acts::CylinderBounds* cylinderBounds = 
    dynamic_cast<const Acts::CylinderBounds*>(&surfaceBounds);
  if (cylinderBounds){
    // take the flip
    std::vector<int> flip    = { -1, 1};
    std::vector<int> vfaces  = { 1, 2, 4, 3};
    // get the surface transform
    auto transform = surface.transform();
    double r    = cylinderBounds->r();
    double hZ   = cylinderBounds->halflengthZ();
    // get the radius and the halfZ value
    size_t nfaces    = 36;
    double phistep   = 2*M_PI/nfaces;
    size_t vcounter = m_nvertices;
    // 
    for (size_t iphi = 0; iphi < nfaces; ++iphi){
      // currentPhi 
      double phi = -M_PI + iphi*phistep;
      double cphi = cos(phi);
      double sphi = sin(phi);
      for (auto iflip : flip ){
        // create the vertex
        Acts::Vector3D point(transform*Acts::Vector3D(r*cos(phi),r*sin(phi),iflip*hZ));
        // the counter for later usage
        ++m_nvertices;
        // write out the vertex 
        (*(m_cfg.outputStream)) << "v " << m_cfg.outputScalor*point.x() << " "
                                        << m_cfg.outputScalor*point.y() << " "
                                        << m_cfg.outputScalor*point.z() << '\n'; 
      }
    }
    // now create the faces
    size_t iphi = 0;
    for (; iphi < nfaces-1; ++iphi ){
      // output to file
      (*(m_cfg.outputStream)) << "f ";
      for (auto face: vfaces)
         (*(m_cfg.outputStream)) << vcounter+(2*iphi)+face << " ";
      (*(m_cfg.outputStream)) << '\n';
    }
    // close the loop
    (*(m_cfg.outputStream)) << "f " << vcounter+(2*iphi)+1 << " " 
                                    << vcounter+(2*iphi)+2 << " "
                                    << vcounter+2          << " "
                                    << vcounter+1          << '\n';
    //
    (*(m_cfg.outputStream)) << '\n';
  }
  // and a newline at the end
  (*(m_cfg.outputStream)) << '\n';
  // return success 
  return FW::ProcessCode::SUCCESS;
}


