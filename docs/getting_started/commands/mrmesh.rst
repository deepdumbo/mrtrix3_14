.. _mrmesh:

mrmesh
===========

Synopsis
--------

::

    mrmesh [ options ]  input output

-  *input*: the input image.
-  *output*: the output mesh file.

Description
-----------

Generate a mesh file from an image.

Options
-------

-  **-blocky** generate a 'blocky' mesh that precisely represents the voxel edges

-  **-threshold value** manually set the intensity threshold at which the mesh will be generated (if omitted, a threshold will be determined automatically)

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

