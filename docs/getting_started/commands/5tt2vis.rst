.. _5tt2vis:

5tt2vis
===========

Synopsis
--------

::

    5tt2vis [ options ]  input output

-  *input*: the input 4D tissue-segmented image
-  *output*: the output 3D image for visualisation

Description
-----------

generate an image for visualisation purposes from an ACT 5TT segmented anatomical image

Options
-------

-  **-bg value** image intensity of background (default: 0)

-  **-cgm value** image intensity of cortical grey matter (default: 0.5)

-  **-sgm value** image intensity of sub-cortical grey matter (default: 0.75)

-  **-wm value** image intensity of white matter (default: 1)

-  **-csf value** image intensity of CSF (default: 0.15)

-  **-path value** image intensity of pathological tissue (default: 2)

Standard options
^^^^^^^^^^^^^^^^

-  **-info** display information messages.

-  **-quiet** do not display information messages or progress status.

-  **-debug** display debugging messages.

-  **-force** force overwrite of output files. Caution: Using the same file as input and output might cause unexpected behaviour.

-  **-nthreads number** use this number of threads in multi-threaded applications (set to 0 to disable multi-threading)

-  **-failonwarn** terminate program if a warning is produced

-  **-help** display this information page and exit.

-  **-version** display version information and exit.

--------------



**Author:** Robert E. Smith (r.smith@brain.org.au)

**Copyright:** Copyright (c) 2008-2016 the MRtrix3 contributors

This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/

MRtrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

For more details, see www.mrtrix.org

