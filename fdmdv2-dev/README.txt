==================================
 Building and installing on Linux
==================================

Quickstart 1
-----------

Builds static versions of wxWidgets, portaudio, codec2-dev, which are commonly
missing on many Linux systems, or of the wrong (older) version.

Assuming the freedv-dev sources is checked out into ~/fdmdv2-dev:

$ sudo apt-get install libgtk2.0-dev libhamlib-dev libsamplerate-dev libasound2-dev libao-dev
$ cd fdmdv2-dev
$ mkdir build_linux
$ cd build_linux
$ cmake -DBOOTSTRAP_WXWIDGETS=TRUE ~/fdmdv2-dev
$ make
[wxWidgets builds]

Then you can use your local codec-dev, something like:

$ cmake -DCMAKE_BUILD_TYPE=Debug -DBOOTSTRAP_WXWIDGETS=TRUE -DCODEC2_INCLUDE_DIRS=/home/david/codec2-dev/src -DCODEC2_LIBRARY=/home/david/codec2-dev/build_linux/src/libcodec2.so -DUSE_STATIC_CODEC2=FALSE -DUSE_STATIC_PORTAUDIO=TRUE -DUSE_STATIC_SOX=TRUE ../

OR build a local copy of codec2-dev:

$ cmake cmake -DBOOTSTRAP_WXWIDGETS=TRUE -DUSE_STATIC_CODEC2=TRUE -DUSE_STATIC_PORTAUDIO=TRUE -DUSE_STATIC_SOX=TRUE ../

$ make
[FreeDV builds]
$ ./src/freedv

Note: add "-DCMAKE_BUILD_TYPE=Debug" the list above for debug (gcc -g) buildthat include source line numbers.

Quickstart 2
------------

Assuming you have all the dependant libraries:

$ cd /path/to/fdmdv2
$ mkdir build_linux
$ cd build_linux
$ cmake ../ (defaults to /usr/local, use CMAKE_INSTALL_PREFIX to override)
(if no errors)
$ make
(as root)
$ make install

====================================
 Building and installing on Windows
====================================

The windows build is similar to linux and follows the same basic workflow.

Only MinGW is supported. While it is likely possible to perform a pure MinGW
build, installing MSYS will make your life easier.

CMake may not automatically detect that you're in the MSYS environment. If this
occurs you need to pass cmake the proper generator:

cmake -G"MSYS Makefiles" [other options] </path/to/source>

===============================
 Bootstrapping wxWidgets build
===============================

If wxWidgets (>= 3.0) is not available then one option is to have CMake boot-
strap the build for FreeDV.

This is required because the tool wx-config is used to get the correct compiler
and linker flags of the wxWidgets components needed by FreeDV. Since this is
normally done at configure time, not during "make", it is not possible for CMake
or have this information prior to building wxWidgets.

In order to work around this issue you can "bootstrap" the wxWidgets build using
the CMake option, "BOOTSTRAP_WXWIDGETS". wxWidgets will be built using static
libraries.

NOTE: This forces "USE_STATIC_WXWIDGETS" to be true internally regarless of the
value set manually.

(from any prefered, but empty directory outside of the source)
$ cmake -DBOOTSTRAP_WXWIDGETS=TRUE /path/to/fdmdv2
$ make
(wxWidgets is downloaded and built)
$ cmake .
$ make
(if all goes well, as root)
$ make install

====================================
 Building and installing on OSX
====================================

====================================
 Building and installing on FreeBSD
====================================

As per "Quickstart 2" above but change build_linux to build_freebsd

=======
Editing
=======

Please make sure your text editor does not insert tabs, and
used indents of 4 spaces.  The following .emacs code was used to
configure emacs:

(setq-default indent-tabs-mode nil)

(add-hook 'c-mode-common-hook
          (function (lambda ()
                      (setq c-basic-offset 4)
                      )))
====
TODO
====

[ ] FreeDV 700 mode
    [ ] Playfile bug
    [ ] integrate freedv API
    [ ] add 700 to freedv API
    [ ] how to handle Fs=7500 Hz?
    [ ] test mode
        + how to plot error histogram
[ ] Small fixes
    [ ] long varicode default
    [ ] option to _not_ require checksum
    [ ] Waterfall direction
    [ ] bump ver number
[ ] Presence/spotting hooks

