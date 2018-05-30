// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_BFIELD_BFIELDACCESSEXAMPLE_H
#define ACTFW_BFIELD_BFIELDACCESSEXAMPLE_H

#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <random>
#include <string>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/MagneticField/concept/AnyFieldLookup.hpp"
#include "Acts/Utilities/Units.hpp"

/// The main executable
///
/// Creates an InterpolatedBFieldMap from a txt or csv file
/// It then tests random versus stepwise access with the
/// direct getField access and the cell.getField access
/// with cell caching

namespace po = boost::program_options;

using UniformDist  = std::uniform_real_distribution<double>;
using RandomEngine = std::mt19937;

template <typename Field>
Acts::Vector3D
accessFieldCell(Field&                         bField,
                Acts::concept::AnyFieldCell<>& cell,
                size_t                         istep,
                const Acts::Vector3D&          position)
{
  if (!istep || !cell.isInside(position)) {
    cell = bField.getFieldCell(position);
  }  // get the field from the cell
  return cell.getField(position);
}

template <typename Field>
Acts::Vector3D
accessFieldDirect(Field& bField, const Acts::Vector3D& position)
{
  return std::move(bField.getField(position));
}

template <typename Field>
void
accessStepWise(Field& bField,
               size_t events,
               size_t theta_steps,
               double theta_0,
               double theta_step,
               size_t phi_steps,
               double phi_0,
               double phi_step,
               size_t access_steps,
               double access_step)
{
  std::cout << "[>>>] Start: step-wise access pattern ... " << std::endl;
  size_t mismatched = 0;
  // get the field cell
  auto cell = bField.getFieldCell(Acts::Vector3D{0., 0., 0.});
  // boost display
  size_t totalSteps = events * theta_steps * phi_steps * access_steps;
  boost::progress_display show_progress(totalSteps);
  // the event loop
  // loop over the events - @todo move to parallel for
  for (size_t ievt = 0; ievt < events; ++ievt) {
    for (size_t itheta = 0; itheta < theta_steps; ++itheta) {
      double theta = theta_0 + itheta * theta_step;
      for (size_t iphi = 0; iphi < phi_steps; ++iphi) {
        double phi = phi_0 + iphi * phi_step;
        // make a direction
        Acts::Vector3D dir(
            cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
        // check for the current step
        double currentStep = 0.;
        // now step through the magnetic field
        for (size_t istep = 0; istep < access_steps; ++istep) {
          Acts::Vector3D position = currentStep * dir;
          // access the field directly
          auto field_direct = accessFieldDirect<Field>(bField, position);
          // access the field with the cell
          auto field_cell
              = accessFieldCell<Field>(bField, cell, istep, position);
          // check
          if (!field_direct.isApprox(field_cell)) {
            ++mismatched;
          }
          // increase the step
          currentStep += access_step;
          // show the progress bar
          ++show_progress;
        }
      }
    }
    std::cout << "[<<<] End result : " << mismatched << "/" << totalSteps
              << " mismatches" << std::endl;
  }
}

template <typename Field>
void
accessRandom(Field& bField, size_t totalSteps, double radius)
{
  std::cout << "[>>>] Start: random access pattern ... " << std::endl;
  size_t       mismatched = 0;
  RandomEngine rng;
  UniformDist  xDist(-radius, radius);
  UniformDist  yDist(-radius, radius);
  UniformDist  zDist(-radius, radius);

  // get the field cell
  auto cell = bField.getFieldCell(Acts::Vector3D{0., 0., 0.});
  boost::progress_display show_progress(totalSteps);

  // the event loop
  // loop over the events - @todo move to parallel for
  for (size_t istep = 0; istep < totalSteps; ++istep) {
    Acts::Vector3D position(xDist(rng), yDist(rng), zDist(rng));
    // access the field directly
    auto field_direct = accessFieldDirect(bField, position);
    // access the field with the cell
    auto field_cell = accessFieldCell(bField, cell, istep, position);
    // check
    if (!field_direct.isApprox(field_cell)) {
      ++mismatched;
    }
    // show the progress bar
    ++show_progress;
  }
  std::cout << "[<<<] End result : " << mismatched << "/" << totalSteps
            << " mismatches" << std::endl;
}

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add an output file
  desc.add_options()("bf-phirange",
                     po::value<read_range>()->default_value({-M_PI, M_PI}),
                     "range in which the phi parameter is generated.")(
      "bf-thetarange",
      po::value<read_range>()->default_value({0., M_PI}),
      "range in which the eta parameter is generated.")(
      "bf-phisteps",
      po::value<size_t>()->default_value(1000),
      "number of steps for the phi parameter.")(
      "bf-thetasteps",
      po::value<size_t>()->default_value(100),
      "number of steps for the eta parameter.")(
      "bf-accesssteps",
      po::value<size_t>()->default_value(100),
      "number of steps for magnetic field access.")(
      "bf-tracklength",
      po::value<double>()->default_value(100.),
      "track length in [mm] magnetic field access.");
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents = standardOptions.first;
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  if (!bField.first) {
    std::cout << "Bfield could not be set up. Exiting." << std::endl;
    return -1;
  }
  // get the phi and eta range
  auto phir   = vm["bf-phirange"].as<read_range>();
  auto thetar = vm["bf-thetarange"].as<read_range>();
  // get the granularity
  size_t phi_steps   = vm["bf-phisteps"].as<size_t>();
  size_t theta_steps = vm["bf-thetasteps"].as<size_t>();
  // the defaults
  size_t access_steps = vm["bf-accesssteps"].as<size_t>();
  double track_length = vm["bf-tracklength"].as<double>() * Acts::units::_mm;
  // sort the ranges - and prepare the access grid
  std::sort(phir.begin(), phir.end());
  std::sort(thetar.begin(), thetar.end());
  double phi_span    = std::abs(phir[1] - phir[0]);
  double phi_step    = phi_span / phi_steps;
  double theta_span  = std::abs(thetar[1] - thetar[0]);
  double theta_step  = theta_span / theta_steps;
  double access_step = track_length / access_steps;
  // step-wise access pattern
  accessStepWise(*(bField.first),
                 nEvents,
                 theta_steps,
                 thetar[0],
                 theta_step,
                 phi_steps,
                 phir[0],
                 phi_step,
                 access_steps,
                 access_step);
  // random access pattern
  accessRandom(*(bField.first),
               nEvents * theta_steps * phi_steps * access_steps,
               track_length);
}

#endif  // ACTFW_BFIELD_BFIELDACCESSEXAMPLE_H
