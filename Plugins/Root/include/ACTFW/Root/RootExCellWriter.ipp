
template <class T> FW::ProcessCode RootExCellWriter::writeT(const Acts::ExtrapolationCell<T>& eCell)
{
    ACTS_VERBOSE("Writing an eCell object with " << eCell.extrapolationSteps.size() << " steps.");
    
    m_positionX.clear();
    m_positionY.clear();
    m_positionZ.clear();
    
    m_material.clear();
    m_sensitive.clear();
    m_boundary.clear();
    
    m_volumeID.clear();
    m_layerID.clear();
    m_surfaceID.clear();

    m_localposition0.clear();
    m_localposition1.clear();
    
    for (auto& es : eCell.extrapolationSteps){
        if (es.parameters){
            const T& pars = (*es.parameters);
            /// goblal position information
            m_positionX.push_back(pars.position().x());
            m_positionY.push_back(pars.position().y());
            m_positionZ.push_back(pars.position().z());
            /// local position information
            m_localposition0.push_back(pars.parameters()[Acts::eLOC_X]);
            m_localposition1.push_back(pars.parameters()[Acts::eLOC_Y]);
            /// check the layer, surface, volume ID
            Acts::geo_id_value volumeID = pars.associatedSurface().geoID().value(
                                                                Acts::GeometryID::volume_mask,
                                                                Acts::GeometryID::volume_shift);
            Acts::geo_id_value layerID = pars.associatedSurface().geoID().value(
                                                                Acts::GeometryID::layer_mask,
                                                                Acts::GeometryID::layer_shift);
            Acts::geo_id_value surfaceID = pars.associatedSurface().geoID().value(
                                                                  Acts::GeometryID::sensitive_mask,
                                                                  Acts::GeometryID::sensitive_shift);
            m_volumeID.push_back(volumeID);
            m_layerID.push_back(layerID);
            m_surfaceID.push_back(surfaceID);
            /// type information
            int material  = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectMaterial);
            int boundary  = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectBoundary);
            int sensitive = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectSensitive);
            m_material.push_back(material);
            m_boundary.push_back(boundary);
            m_sensitive.push_back(sensitive);
            
            
        }
    }
    // write to 
    m_outputTree->Fill();
    
    // return scuess
    return FW::ProcessCode::SUCCESS;
}

