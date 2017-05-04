#include <iostream>
#include "ACTFW/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTFW/Writers/ITrackingGeometryWriter.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Surfaces/Surface.hpp"

FWObj::ObjTrackingGeometryWriter::ObjTrackingGeometryWriter(
    const FWObj::ObjTrackingGeometryWriter::Config& cfg)
  : FW::IWriterT<Acts::TrackingGeometry>()
  , m_cfg(cfg)
{}

FWObj::ObjTrackingGeometryWriter::~ObjTrackingGeometryWriter()
{
}

FW::ProcessCode
FWObj::ObjTrackingGeometryWriter::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWObj::ObjTrackingGeometryWriter::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWObj::ObjTrackingGeometryWriter::write(const Acts::TrackingGeometry& tGeometry)
{
  // get the world volume
  auto world = tGeometry.highestTrackingVolume();
  if (world) 
      write(*world);
  // return the success code
  return FW::ProcessCode::SUCCESS;
}

/// process this volume
void
FWObj::ObjTrackingGeometryWriter::write(const Acts::TrackingVolume& tVolume)
{
  // get the confined layers and process them
  if (tVolume.confinedLayers()){
    // loop over the layers
    for (auto layer : tVolume.confinedLayers()->arrayObjects()){
      // get the volume name
      const std::string& volumeName = tVolume.volumeName();      
      // find the right surfacewriter
      std::shared_ptr< FW::IWriterT<Acts::Surface> > surfaceWriter = nullptr;
      for (auto writer : m_cfg.surfaceWriters){
        // get name and writer
        auto writerName = writer->name();
        if (volumeName.find(writerName) != std::string::npos ){
            // asign the writer           
            surfaceWriter = writer;
            // and break
            break;
        }        
      }
      // bail out if you have no surface writer
      if (!surfaceWriter) return;
      // try to write the material surface as well 
      if (layer->surfaceRepresentation().associatedMaterial())
          surfaceWriter->write(layer->surfaceRepresentation());
      // the the approaching surfaces and check if they have material
      if (layer->approachDescriptor()){
        // loop over the contained Surfaces
        for (auto& cSurface : layer->approachDescriptor()->containedSurfaces() )
          if (cSurface->associatedMaterial())
              surfaceWriter->write(*cSurface);
      }
      // check for sensitive surfaces
      if (layer->surfaceArray() && surfaceWriter){
        // loop over the surface
        for (auto surface : layer->surfaceArray()->arrayObjects()){
          if (surface 
              && (surfaceWriter->write(*surface)) == FW::ProcessCode::ABORT) 
              return FW::ProcessCode::ABORT;
           }
        }
     }
  }
  
  // get the confined volumes and step down the hierarchy 
  if (tVolume.confinedVolumes()){
    // loop over the volumes and write what they have
    for (auto volume : tVolume.confinedVolumes()->arrayObjects()){
      write(*volume.get());
    }
  }
}


