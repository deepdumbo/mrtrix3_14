/*
 * Copyright (c) 2008-2016 the MRtrix3 contributors
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 * 
 * MRtrix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * For more details, see www.mrtrix.org
 * 
 */


#include "command.h"
#include "exception.h"
#include "mrtrix.h"
#include "progressbar.h"

#include "image.h"
#include "algo/loop.h"

#include "math/math.h"
#include "math/SH.h"

#include "dwi/gradient.h"
#include "dwi/shells.h"





using namespace MR;
using namespace App;






void usage () {

  DESCRIPTION 
    + "generate an appropriate response function from the image data for spherical deconvolution";

  ARGUMENTS
    + Argument ("SH", "the spherical harmonic decomposition of the diffusion-weighted images").type_image_in()
    + Argument ("mask", "the mask containing the voxels from which to estimate the response function").type_image_in()
    + Argument ("directions", "a 4D image containing the direction vectors along which to estimate the response function").type_image_in()
    + Argument ("response", "the output axially-symmetric spherical harmonic coefficients").type_file_out();

  OPTIONS

    + Option ("lmax", "specify the maximum harmonic degree of the response function to estimate")
      + Argument ("value").type_integer (0, 20);
}



typedef double value_type;



void run () 
{
  auto SH = Image<value_type>::open(argument[0]);
  Math::SH::check (SH);
  auto mask = Image<bool>::open(argument[1]);
  auto dir = Image<value_type>::open(argument[2]).with_direct_io();

  int lmax = get_option_value ("lmax", Math::SH::LforN (SH.size(3)));

  check_dimensions (SH, mask, 0, 3);
  check_dimensions (SH, dir, 0, 3);
  if (dir.ndim() != 4)
    throw Exception ("input direction image \"" + std::string (argument[2]) + "\" must be a 4D image");
  if (dir.size(3) != 3)
    throw Exception ("input direction image \"" + std::string (argument[2]) + "\" must contain precisely 3 volumes");

  Eigen::VectorXd delta;
  std::vector<value_type> response (lmax/2 + 1, 0.0);
  size_t count = 0;

  auto loop = Loop ("estimating response function", SH, 0, 3);
  for (auto l = loop(mask, SH, dir); l; ++l) {

    if (!mask.value()) 
      continue;

    Eigen::Vector3d d = dir.row(3);
    d.normalize();
    Math::SH::delta (delta, d, lmax);

    for (int l = 0; l <= lmax; l += 2) {
      value_type d_dot_s = 0.0;
      value_type d_dot_d = 0.0;
      for (int m = -l; m <= l; ++m) {
        size_t i = Math::SH::index (l,m);
        SH.index(3) = i;
        value_type s = SH.value();
        // TODO: currently this does NOT handle the non-orthonormal basis
        d_dot_s += s*delta[i];
        d_dot_d += Math::pow2 (delta[i]);
      }
      response[l/2] += d_dot_s / d_dot_d;
    }
    ++count;
  }

  Eigen::Matrix<value_type,Eigen::Dynamic,1,0,64> AL (lmax+1);
  Math::Legendre::Plm_sph (AL, lmax, 0, value_type (1.0));
  for (size_t l = 0; l < response.size(); l++)
    response[l] *= AL[2*l] / count;

  if (std::string(argument[3]) == "-") {
    for (auto r : response)
      std::cout << r << " ";
    std::cout << "\n";
  }
  else {
    save_vector(response, argument[3]);
  }
}
