// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace po = boost::program_options;

namespace FW {
namespace Options {

  /// The options for the material loading
  template <typename aopt_t>
  void
  addMaterialOptions(aopt_t& opt)
  {
    opt.add_options()("mat-source",
                      po::value<size_t>()->default_value(0),
                      "The source for the material: 0 - none, 1 - proxy, 2 - "
                      "building, 3 - reading")(
        "mat-input-file",
        po::value<std::string>()->default_value(""),
        "The filename for the material, auto-detects loading plugin");
  }

  /// Read the material decorator
  ///
  /// @tparam amap_t Type of the options map
  ///
  /// @param[in] vm Map to be read in
  template <typename amap_t>
  std::shared_ptr<const Acts::IMaterialDecorator>
  readMaterialDecorator(const amap_t& vm)
  {

    return nullptr;
  }

}  // namespace
}  // namespace
