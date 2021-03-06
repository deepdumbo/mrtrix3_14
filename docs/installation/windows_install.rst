=============
Windows installation
=============


We outline the steps for installing *MRtrix3* for Windows using `MSYS2 <http://sourceforge.net/p/msys2/wiki/MSYS2%20introduction/>`__. 
Please consult :ref:`windows_trouble_shooting` if you encounter any issues with the configure, build
or runtime operations of *MRtrix3*.

Check requirements
------------------

To install *MRtrix3*, you will need the following:

-  a `C++11 <https://en.wikipedia.org/wiki/C%2B%2B11>`__ compliant
   compiler
-  `Python <https://www.python.org/>`__ version >= 2.6
-  The `zlib <http://www.zlib.net/>`__ compression library
-  `Eigen <http://eigen.tuxfamily.org>`__ version 3
-  `Qt <http://www.qt.io/>`__ version >= 4.7 *[GUI components only]*

.. NOTE::
    All of these dependencies are installed below by the MSYS2 package manager.

.. WARNING:: 
    To run the GUI components of *MRtrix3* (``mrview`` & ``shview``), you will also need:

    -  an `OpenGL <https://en.wikipedia.org/wiki/OpenGL>`__ 3.3 compliant graphics card and corresponding software driver 

.. WARNING:: 
    When following the instructions below, use the **'MinGW-w64 Win64 shell'**; 'MSYS2 shell' and 'MinGW-w64 Win32 shell' should be avoided.

Install and update MSYS2
------------------------

1. Download and install the most recent 64-bit MSYS2 installer from
   http://msys2.github.io/ (msys2-x86\_64-\*.exe).

2. Run the program **'MinGW-w64 Win64 Shell'** from the start menu.

3. Update the system packages:

   ::

       update-core
   
   .. NOTE::
    Future versions of MSYS2 will drop ``update-core``. If your version came without ``update-core``, it is probably safe to skip this step.

4. Close the shell and start **'MinGW-w64 Win64 Shell'**
   
5. Update the other packages:

   ::

       pacman -Su

Install *MRtrix3* dependencies
----------------------------

1. From the **'MinGW-w64 Win64 Shell'** run:

   ::

       pacman -S git python pkg-config mingw-w64-x86_64-gcc mingw-w64-x86_64-eigen3 mingw-w64-x86_64-qt5

Set up git and download *MRtrix3* sources
---------------------------------------

1. Configure global settings for Git in the **'MinGW-w64 Win64 Shell'**:

   ::

       git config --global user.name "John Doe"
       git config --global user.email johndoe@example.com
       git config --global push.default upstream

2. Clone the *MRtrix3* repository:

   ::

       git clone https://github.com/MRtrix3/mrtrix3.git

Build *MRtrix3*
-------------

1. Configure the MRtrix install:

   ::

       cd mrtrix3
       ./configure

   If this does not work, examine the 'configure.log' file that is
   generated by this step, it may give clues as to what went wrong.

2. Build the binaries:

   ::

       ./build

Set up *MRtrix3*
--------------

1. Set your PATH in the shell startup file:

   ::

       echo "export PATH=$(pwd)/release/bin:$(pwd)/scripts:\$PATH" >> ~/.bashrc

   Note that although the scripts provided with MRtrix will appear in
   your path, many of these will not work on a Windows installation due
   to their dependency on FSL; a virtual machine with both MRtrix3 and
   FSL installed would be required to run these scripts in this
   scenario.

2. Close the terminal and start another one to ensure the startup file
   is read

3. type ``mrview`` to check that everything works

4. You may also want to have a look through the :ref:`mrtrix_config_options`, and set anything you think
   might be required on your system.

Keeping *MRtrix3* up to date
--------------------------

1. You can update your installation at any time by typing:

   ::

       git pull
       ./build

2. If this doesn't work immediately, it may be that you need to re-run
   the configure script:

   ::

       ./configure

   and re-run step 1 again.

Compiling external projects with ``msys2``
------------------------------------------

In ``msys2``, the ``ln -s`` command actually creates a *copy* of the
target, *not* a symbolic link. By doing so, the build script is unable
to identify the location of the MRtrix libraries when trying to compile
an external module.

The simplest way around this is simply to invoke the build script of the main
MRtrix install directly. For example, if compiling an external project called
``myproject``, residing in a folder alongside the main ``mrtrix3`` folder, the
build script can be invoked with::

    # current working directory is 'myproject':
    ../mrtrix3/build

If you really want a symbolic link, one solution is to use a standard Windows
command prompt, with Administrator priveleges: In the file explorer, go to
``C:\Windows\system32``, locate the file ``cmd.exe``, right-click and
select 'Run as administrator'. Within this prompt, use the ``mklink``
command (note that the argument order passed to ``mklink`` is reversed
with respect to ``ln -s``; i.e. provide the location of the link, *then*
the target). Make sure that you provide the *full path* to both link and
target, e.g.:

::

        mklink C:\msys64\home\username\src\my_project\build C:\msys64\home\username\src\MRtrix3\build

, and ``msys64`` should be able to interpret the softlink path correctly
(confirm with ``ls -la``).


.. _windows_trouble_shooting:

Troubleshooting
-----

.. WARNING:: 
    To be added


