#ifndef ACTFW_OPTIONS_H
#define ACTFW_OPTIONS_H

#include <string>

using read_range = std::vector<double>;

namespace std
{
  std::ostream& operator<<(std::ostream &os, const read_range &vec) 
  {    
    for (auto item : vec) 
    { 
      os << item << " "; 
    } 
    return os; 
  }
} 


#endif // ACTFW_OPTIONS_H

