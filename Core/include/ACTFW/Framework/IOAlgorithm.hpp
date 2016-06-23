//
//  IOAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_FRAMEWORK_IOALGORITHM_H 
#define ACTFW_FRAMEWORK_IOALGORITHM_H 1

#include "ACTFW/Framework/ProcessCode.hpp"
#include <string>
#include <memory>

namespace FW {
    
    class WhiteBoard;
    
    /// @class IOAlgorithm 
    ///
    /// Base class that defines the interface for algorithms that deal with I/O
    class IOAlgorithm {
        
        public :
        /// Virtual destructor
        virtual ~IOAlgorithm(){}
        
        /// Framework intialize method 
        virtual ProcessCode initialize(std::shared_ptr<WhiteBoard> eventStore = nullptr,
                                       std::shared_ptr<WhiteBoard> jobStore = nullptr) = 0;

        /// Framework execode method 
        virtual ProcessCode skip(size_t nEvents=1) = 0;

        /// Framework execode method 
        virtual ProcessCode read(size_t eventNumber) = 0;

        /// Framework execode method 
        virtual ProcessCode write(size_t eventNumber) = 0;

        /// Framework finalize mehtod 
        virtual ProcessCode finalize() = 0;
        
        /// Framework name() method 
        virtual const std::string& name() const = 0;
        
        /// return the eventStore - things that live per event
        virtual std::shared_ptr<WhiteBoard> eventStore() const = 0;
        
        /// return the jobStore - things that live for the full job 
        virtual std::shared_ptr<WhiteBoard> jobStore() const = 0;
        
    };

}

#endif // ACTFW_FRAMEWORK_IOALGORITHM_H
