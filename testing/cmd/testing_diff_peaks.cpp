
/*
    Copyright 2008 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 27/06/08.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "command.h"
#include "progressbar.h"
#include "datatype.h"

#ifdef MRTRIX_UPDATED_API
 
# include "image.h"
# include "algo/threaded_loop.h"
 
#else
 
# include "point.h"
# include "image/buffer.h"
# include "image/voxel.h"
# include "image/threaded_loop.h"
 
#endif


using namespace MR;
using namespace App;

void usage ()
{
  DESCRIPTION
  + "compare two peak images for differences, within specified tolerance.";

  ARGUMENTS
  + Argument ("peaks1", "a peaks image.").type_image_in ()
  + Argument ("peaks2", "another peaks image.").type_image_in ()
  + Argument ("tolerance", "the dot product difference to consider acceptable").type_float (0.0);
}


void run ()
{
#ifdef MRTRIX_UPDATED_API

  auto in1 = Image<double>::open (argument[0]);
  auto in2 = Image<double>::open (argument[1]);
  check_dimensions (in1, in2);
  if (in1.ndim() != 4)
    throw Exception ("images \"" + in1.name() + "\" and \"" + in2.name() + "\" are not 4D");
  if (in1.size(3) % 3)
    throw Exception ("images \"" + in1.name() + "\" and \"" + in2.name() + "\" do not contain XYZ peak directions");    
  for (size_t i = 0; i < in1.ndim(); ++i) {
    if (std::isfinite (in1.size(i)))
      if (in1.size(i) != in2.size(i))
        throw Exception ("images \"" + in1.name() + "\" and \"" + in2.name() + "\" do not have matching voxel spacings " +
                                       str(in1.size(i)) + " vs " + str(in2.size(i)));
  }
  for (size_t i  = 0; i < 3; ++i) {
    for (size_t j  = 0; j < 4; ++j) {
      if (std::abs (in1.transform().matrix()(i,j) - in2.transform().matrix()(i,j)) > 0.0001)
        throw Exception ("images \"" + in1.name() + "\" and \"" + in2.name() + "\" do not have matching header transforms "
                           + "\n" + str(in1.transform().matrix()) + "vs \n " + str(in2.transform().matrix()) + ")");
    }
  }


  double tol = argument[2];

  ThreadedLoop (in1, 0, 3)
  .run ([&tol] (decltype(in1)& a, decltype(in2)& b)
  {
    for (size_t i = 0; i != size_t(a.size(3)); i += 3) {
      Eigen::Vector3 veca, vecb;
      for (size_t axis = 0; axis != 3; ++axis) {
        a.index(3) = b.index(3) = i + axis;
        veca[axis] = a.value();
        vecb[axis] = b.value();
      }
      const double norma = veca.norm(), normb = vecb.norm();
      veca.normalize(); vecb.normalize();
      const double dp = std::abs (veca.dot (vecb));
      if (1.0 - dp > tol || std::abs (norma - normb) > tol)
        throw Exception ("images \"" + a.name() + "\" and \"" + b.name() + "\" do not match within specified precision of " + str(tol) + " ( [" + str(veca.transpose().cast<float>()) + "] vs [" + str(vecb.transpose().cast<float>()) + "], norms [" + str(norma) + " " + str(normb) + "], dot product = " + str(dp) + ")");
    }
  }, in1, in2);

#else

  Image::Buffer<double> buffer1 (argument[0]);
  Image::Buffer<double> buffer2 (argument[1]);
  Image::check_dimensions (buffer1, buffer2);
  if (buffer1.ndim() != 4)
    throw Exception ("images \"" + buffer1.name() + "\" and \"" + buffer2.name() + "\" are not 4D");
  if (buffer1.dim(3) % 3)
    throw Exception ("images \"" + buffer1.name() + "\" and \"" + buffer2.name() + "\" do not contain XYZ peak directions");  
  for (size_t i = 0; i < buffer1.ndim(); ++i) {
    if (std::isfinite (buffer1.vox(i)))
      if (buffer1.vox(i) != buffer2.vox(i))
        throw Exception ("images \"" + buffer1.name() + "\" and \"" + buffer2.name() + "\" do not have matching voxel spacings " +
                                       str(buffer1.vox(i)) + " vs " + str(buffer2.vox(i)));
  }
  for (size_t i  = 0; i < 4; ++i) {
    for (size_t j  = 0; j < 4; ++j) {
      if (std::abs (buffer1.transform()(i,j) - buffer2.transform()(i,j)) > 0.0001)
        throw Exception ("images \"" + buffer1.name() + "\" and \"" + buffer2.name() + "\" do not have matching header transforms "
                         + "\n" + str(buffer1.transform()) + "vs \n " + str(buffer2.transform()) + ")");
    }
  }

  double tol = argument[2];

  Image::ThreadedLoop (buffer1, 0, 3)
  .run ([&tol] (decltype(buffer1.voxel())& a, decltype(buffer2.voxel())& b)
  {
    for (size_t i = 0; i != size_t(a.dim(3)); i += 3) {
      Point<double> veca, vecb;
      for (size_t axis = 0; axis != 3; ++axis) {
        a[3] = b[3] = i + axis;
        veca[axis] = a.value();
        vecb[axis] = b.value();
      }
      const double norma = veca.norm(), normb = vecb.norm();
      veca.normalise(); vecb.normalise();
      const double dp = std::abs (veca.dot (vecb));
      if (1.0 - dp > tol || std::abs (norma - normb) > tol)
        throw Exception ("images \"" + a.name() + "\" and \"" + b.name() + "\" do not match within specified precision of " + str(tol) + " (" + str(veca) + " vs " + str(vecb) + ", norms [" + str(norma) + " " + str(normb) + "], dot product " + str(dp) + ")");
    }
  }, buffer1.voxel(), buffer2.voxel());

#endif

  CONSOLE ("data checked OK");
}

