//
//  DataClassTwo
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef DataClassTwo_h
#define DataClassTwo_h

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace FWE {

class DataClassTwo
{
public:
  DataClassTwo(const std::string& stringData, double eventData)
    : m_dataString(stringData), m_dataDouble(eventData)
  {
  }

  /// the contained data : string
  const std::string
  data() const;

private:
  std::string m_dataString;  /// data member string
  double      m_dataDouble;  /// data member size_t
};

inline const std::string
DataClassTwo::data() const
{
  std::ostringstream oss;
  oss << "Data : " << m_dataString << " | " << m_dataDouble;
  return oss.str();
}

typedef std::vector<DataClassTwo> DataClassTwoCollection;

}  // namespace FWE

#endif
