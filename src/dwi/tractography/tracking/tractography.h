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

#ifndef __dwi_tractography_tracking_tractography_h__
#define __dwi_tractography_tracking_tractography_h__

#include "app.h"
#include "mrtrix.h"
#include "dwi/tractography/properties.h"


#define DEFAULT_TRACTOGRAPHY_MAX_TRIALS 1000
#define DEFAULT_TRACTOGRAPHY_CUTOFF 0.1
#define DEFAULT_TRACTOGRAPHY_IFOD2_NSAMPLES 4


namespace MR
{
  namespace App { class OptionGroup; }

  namespace DWI
  {

    namespace Tractography
    {

      namespace Tracking
      {

        extern const App::OptionGroup TrackOption;

        void load_streamline_properties (Properties&);

      }
    }
  }
}

#endif

