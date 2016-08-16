//
//  ConstantFieldSvc.hpp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 19/05/16.
//
//

#ifndef ConstantFieldSvc_hpp
#define ConstantFieldSvc_hpp

#include <array>
#include <string>

#include "ACTS/MagneticField/IMagneticFieldSvc.hpp"

namespace FWE {

/// @ class ConstantFieldSvc
///
/// @ author Andreas.Salzburger -at- cern.ch
///
class ConstantFieldSvc : public Acts::IMagneticFieldSvc
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:
  /// @class Config - nested configuraiton class
  class Config
  {
  public:
    std::array<double, 3> field;
    std::string name;

    Config() : field({{0., 0., 20.}}), name("Anonymous") {}
  };

  /// Constructor
  ConstantFieldSvc(const Config& cfg) : m_cfg(cfg) {}
  /// Destructor
  ~ConstantFieldSvc() {}
  // get B field value at given position
  // xyz[3] is in mm, bxyz[3] is in kT
  // if deriv[9] is given, field derivatives are returned in kT/mm
  void
  getField(const double* xyz,
           double*       bxyz,
           double*       deriv = nullptr) const final;

  /// get B field value on the z-r plane at given position
  /// works only inside the solenoid; otherwise calls getField() above
  /// xyz[3] is in mm, bxyz[3] is in kT
  /// if deriv[9] is given, field derivatives are returned in kT/mm
  void
  getFieldZR(const double* xyz,
             double*       bxyz,
             double*       deriv = nullptr) const final;

private:
  Config m_cfg;
};

inline void
ConstantFieldSvc::getField(const double*, double* bxyz, double*) const
{
  bxyz[0] = m_cfg.field[0];
  bxyz[1] = m_cfg.field[1];
  bxyz[2] = m_cfg.field[2];
  return;
}

inline void
ConstantFieldSvc::getFieldZR(const double*, double* bxyz, double*) const
{
  bxyz[0] = m_cfg.field[0];
  bxyz[1] = m_cfg.field[1];
  bxyz[2] = m_cfg.field[2];
  return;
}
}

#endif /* ConstantFieldSvc_hpp */
