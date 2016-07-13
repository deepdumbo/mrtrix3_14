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
#include "image.h"
#include "filter/base.h"
#include "filter/connected_components.h"
#include "filter/dilate.h"
#include "filter/erode.h"
#include "filter/median.h"


using namespace MR;
using namespace App;



const char* filters[] = { "connect", "dilate", "erode", "median", nullptr };




const OptionGroup ConnectOption = OptionGroup ("Options for connected-component filter")

+ Option ("axes", "specify which axes should be included in the connected components. By default only "
                  "the first 3 axes are included. The axes should be provided as a comma-separated list of values.")
  + Argument ("axes").type_sequence_int()

+ Option ("largest", "only retain the largest connected component")

+ Option ("connectivity", "use 26-voxel-neighbourhood connectivity (Default: 6)");



const OptionGroup DilateErodeOption = OptionGroup ("Options for dilate / erode filters")

  + Option ("npass", "the number of times to repeatedly apply the filter")
    + Argument ("value").type_integer (1, 1e6);



const OptionGroup MedianOption = OptionGroup ("Options for median filter")

  + Option ("extent", "specify the extent (width) of kernel size in voxels. "
            "This can be specified either as a single value to be used for all axes, "
            "or as a comma-separated list of the extent for each axis. The default is 3x3x3.")
  + Argument ("voxels").type_sequence_int();





void usage ()
{
  AUTHOR = "Robert E. Smith (r.smith@brain.org.au), David Raffelt (david.raffelt@florey.edu.au) and J-Donald Tournier (jdtournier@gmail.com)";

  DESCRIPTION
  + "Perform filtering operations on 3D / 4D mask images."

  + "The available filters are: connect, dilate, erode, median."

  + "Each filter has its own unique set of optional parameters.";


  ARGUMENTS
  + Argument ("input",  "the input image.").type_image_in ()
  + Argument ("filter", "the type of filter to be applied (connect, dilate, erode, median)").type_choice (filters)
  + Argument ("output", "the output image.").type_image_out ();


  OPTIONS
  + ConnectOption
  + DilateErodeOption
  + MedianOption

  + Stride::Options;
}


typedef bool value_type;

void run () {

  auto input_image = Image<value_type>::open (argument[0]);

  int filter_index = argument[1];

  if (filter_index == 0) { // Connected components
    Filter::ConnectedComponents filter (input_image, std::string("applying connected-component filter to image ") + Path::basename (argument[0]));
    auto opt = get_options ("axes");
    std::vector<int> axes;
    if (opt.size()) {
      axes = opt[0][0];
      for (size_t d = 0; d < input_image.ndim(); d++)
        filter.set_ignore_dim (d, true);
      for (size_t i = 0; i < axes.size(); i++) {
        if (axes[i] >= static_cast<int> (input_image.ndim()) || axes[i] < 0)
          throw Exception ("axis supplied to option -ignore is out of bounds");
        filter.set_ignore_dim (axes[i], false);
      }
    }
    bool largest_only = false;
    opt = get_options ("largest");
    if (opt.size()) {
      largest_only = true;
      filter.set_largest_only (true);
    }
    opt = get_options ("connectivity");
    if (opt.size())
      filter.set_26_connectivity (true);

    Stride::set_from_command_line (filter);

    if (largest_only) {
      filter.datatype() = DataType::UInt8;
      auto output_image = Image<value_type>::create (argument[2], filter);
      filter (input_image, output_image);
    } else {
      filter.datatype() = DataType::UInt32;
      filter.datatype().set_byte_order_native();
      auto output_image = Image<uint32_t>::create (argument[2], filter);
      filter (input_image, output_image);
    }
    return;
  }

  if (filter_index == 1) { // Dilate
    Filter::Dilate filter (input_image, std::string("applying dilate filter to image ") + Path::basename (argument[0]));
    auto opt = get_options ("npass");
    if (opt.size())
      filter.set_npass (int(opt[0][0]));

    Stride::set_from_command_line (filter);
    filter.datatype() = DataType::Bit;

    auto output_image = Image<value_type>::create (argument[2], filter);
    filter (input_image, output_image);
    return;
  }

  if (filter_index == 2) { // Erode
    Filter::Erode filter (input_image, std::string("applying erode filter to image ") + Path::basename (argument[0]));
    auto opt = get_options ("npass");
    if (opt.size())
      filter.set_npass (int(opt[0][0]));

    Stride::set_from_command_line (filter);
    filter.datatype() = DataType::Bit;

    auto output_image = Image<value_type>::create (argument[2], filter);
    filter (input_image, output_image);
    return;
  }

  if (filter_index == 3) { // Median
    Filter::Median filter (input_image, std::string("applying median filter to image ") + Path::basename (argument[0]));
    auto opt = get_options ("extent");
    if (opt.size())
      filter.set_extent (parse_ints (opt[0][0]));

    Stride::set_from_command_line (filter);
    filter.datatype() = DataType::Bit;

    auto output_image = Image<value_type>::create (argument[2], filter);
    filter (input_image, output_image);
    return;
  }

}