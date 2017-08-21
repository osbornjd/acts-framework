#include "ACTFW/Plugins/BField/BFieldUtils.hpp"
#include <fstream>
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"

Acts::InterpolatedBFieldMap::FieldMapper<2, 2> FWBField::txt::fieldMapperRZ(
    std::function<size_t(std::array<size_t, 2> binsRZ,
                         std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
    std::string fieldMapFile,
    double      lengthUnit,
    double      BFieldUnit,
    size_t      nPoints,
    bool        firstQuadrant)
{
  /// [1] Read in field map file
  // Grid position points in r and z
  std::vector<double> rPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector2D> bField;
  // reserve estimated size
  rPos.reserve(nPoints);
  zPos.reserve(nPoints);
  bField.reserve(nPoints);
  // [1] Read in file and fill values
  std::ifstream map_file(fieldMapFile.c_str(), std::ios::in);
  std::string   line;
  double        r = 0., z = 0.;
  double        br = 0., bz = 0.;
  while (std::getline(map_file, line)) {
    if (line.empty() || line[0] == '%' || line[0] == '#'
        || line.find_first_not_of(' ') == std::string::npos)
      continue;

    std::istringstream tmp(line);
    tmp >> r >> z >> br >> bz;
    rPos.push_back(r);
    zPos.push_back(z);
    bField.push_back(Acts::Vector2D(br, bz));
  }
  map_file.close();
  /// [2] use helper function in core
  return Acts::fieldMapperRZ(localToGlobalBin,
                             rPos,
                             zPos,
                             bField,
                             lengthUnit,
                             BFieldUnit,
                             firstQuadrant);
}

Acts::InterpolatedBFieldMap::FieldMapper<3, 3> FWBField::txt::fieldMapperXYZ(
    std::function<size_t(std::array<size_t, 3> binsXYZ,
                         std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
    std::string fieldMapFile,
    double      lengthUnit,
    double      BFieldUnit,
    size_t      nPoints,
    bool        firstOctant)
{
  /// [1] Read in field map file
  // Grid position points in x, y and z
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector3D> bField;
  // reserve estimated size
  xPos.reserve(nPoints);
  yPos.reserve(nPoints);
  zPos.reserve(nPoints);
  bField.reserve(nPoints);
  // [1] Read in file and fill values
  std::ifstream map_file(fieldMapFile.c_str(), std::ios::in);
  std::string   line;
  double        x = 0., y = 0., z = 0.;
  double        bx = 0., by = 0., bz = 0.;
  while (std::getline(map_file, line)) {
    if (line.empty() || line[0] == '%' || line[0] == '#'
        || line.find_first_not_of(' ') == std::string::npos)
      continue;

    std::istringstream tmp(line);
    tmp >> x >> y >> z >> bx >> by >> bz;
    xPos.push_back(x);
    yPos.push_back(y);
    zPos.push_back(z);
    bField.push_back(Acts::Vector3D(bx, by, bz));
  }
  map_file.close();

  return Acts::fieldMapperXYZ(localToGlobalBin,
                              xPos,
                              yPos,
                              zPos,
                              bField,
                              lengthUnit,
                              BFieldUnit,
                              firstOctant);
}
