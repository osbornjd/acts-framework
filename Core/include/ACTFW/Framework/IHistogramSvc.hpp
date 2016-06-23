//
//  IHistogramSvc.hpp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//

#ifndef ACTFW_FRAMEWORK_IHISTOGRAMSVC_h
#define ACTFW_FRAMEWORK_IHISTOGRAMSVC_h

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/IService.hpp"

#incluce <array>
#include <string>

namespace FW {
  
    ///  @class IHistogramSvc
    ///
    /// Interface class for HistrogramSvc in order to write out validation histograms 
    /// Decouples actual hist/tree writing */
    
    class IHistogramSvc : public IService {
    public:
      
        /// Virtual Destructor 
        virtual ~IHistogramSvc(){}
        
        ///  book a 1D histogram 
        virtual ProcessCode book1D(const std::string name, size_t bins0, double low0, double high0) = 0;

        ///  book a 2D histogram 
        virtual ProcessCode book2D(const std::string& name,
                                   size_t bins0, double low0, double high0,
                                   size_t bins1, double low1, double high1) = 0;
        
    
    };
     
     
}



#endif // ACTFW_FRAMEWORK_IHISTOGRAMSVC_h
