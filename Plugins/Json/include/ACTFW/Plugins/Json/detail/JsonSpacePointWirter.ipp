template <class T> FWJson::JsonSpacePointWriter<T>::JsonSpacePointWriter(
    const FWJson::JsonSpacePointWriter<T>::Config& cfg)
  : FW::IEventDataWriterT<T>()
  , m_cfg(cfg)
{}


template <class T> FW::ProcessCode
FWJson::JsonSpacePointWriter<T>::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
FWJson::JsonSpacePointWriter<T>::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
FWJson::JsonSpacePointWriter<T>::write(const FW::DetectorData<geo_id_value, T>& eData)
{
 if (!m_cfg.outputStream)   return FW::ProcessCode::SUCCESS;
    
  (*(m_cfg.outputStream)) << std::endl;
  (*(m_cfg.outputStream)) << "{ \"SpacePoints\" : [";
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
  
  // loop and fill the space point data
  for (auto& volumeData : eData){
    // get the volume id for the naming
    geo_id_value volumeID = volumeData.first.value(Atcts::GeometryID::volume_mask);
    // 
    (*(m_cfg.outputStream)) << "{ \"SpacePoints_" << volumeID <<  "\" : [";
    (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
    // initialize the virgule
    bool comma = false;
    for (auto& layerData : volumeData.second)
      for (auto& moduleData : layerData.second)
        for (auto& data : moduleData.second){
          // set the virugle correctly
          if (comma) (*(m_cfg.outputStream)) << ", ";
          comma = true;
          // write the space point
          (*(m_cfg.outputStream)) << "[" << data.x() << ", " << data.y() << ", " << data.z() << "]";
        }
        
    (*(m_cfg.outputStream)) << "] }" << std::endl;
  }    
  // return success
  return FW::ProcessCode::SUCCESS;
}