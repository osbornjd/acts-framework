//
//  RootExCellWriter.ipp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

template <class T> FW::ProcessCode RootExCellWriter::writeT(const Acts::ExtrapolationCell<T>& eCell)
{
    MSG_VERBOSE("Writing an eCell object with " << eCell.extrapolationSteps.size() << " steps.");
    
    m_positionX.clear();
    m_positionY.clear();
    m_positionZ.clear();
    
    for (auto& es : eCell.extrapolationSteps){
        if (es.parameters){
            const T& pars = (*es.parameters);
            m_positionX.push_back(pars.position().x());
            m_positionY.push_back(pars.position().y());
            m_positionZ.push_back(pars.position().z());
        }
    }
    // write to 
    m_outputTree->Fill();
    
    // return scuess
    return FW::ProcessCode::SUCCESS;
}

