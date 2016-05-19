//
//  MsgStreamMacros.h
//  ACTFW
//
//  Created by Andreas Salzburger on 12/05/16.
//
//

#ifndef MsgStreamMacros_h
#define MsgStreamMacros_h

#include <iostream>
#include <iomanip>

namespace FW {

    enum class MessageLevel { VERBOSE, DEBUG, INFO, ERROR, WARNING, FATAL };

#define MSG_VERBOSE(x) if (messageLevel() <= FW::MessageLevel::VERBOSE) std::cout << std::setw(20) << name() << " - VERBOSE : " << x << std::endl;
#define MSG_DEBUG(x)   if (messageLevel() <= FW::MessageLevel::DEBUG)   std::cout << std::setw(20) << name() << " - DEBUG   : " << x << std::endl;
#define MSG_INFO(x)    if (messageLevel() <= FW::MessageLevel::INFO)    std::cout << std::setw(20) << name() << " - INFO    : " << x << std::endl;
#define MSG_ERROR(x)   if (messageLevel() <= FW::MessageLevel::ERROR)   std::cout << std::setw(20) << name() << " - ERROR   : " << x << std::endl;
#define MSG_WARNING(x) if (messageLevel() <= FW::MessageLevel::WARNING) std::cout << std::setw(20) << name() << " - WARNING : " << x << std::endl;
#define MSG_FATAL(x)   if (messageLevel() <= FW::MessageLevel::FATAL)   std::cout << std::setw(20) << name() << " - FATAL   : " << x << std::endl;

}

#endif /* MsgStreamMacros_h */
