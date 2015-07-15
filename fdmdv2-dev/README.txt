==================================
 Building and installing on Linux
==================================

Quickstart 1
-----------

Builds static versions of wxWidgets, portaudio, codec2-dev, which are commonly
missing on many Linux systems, or of the wrong (older) version.

1/ Assuming the fdmdv2-dev source is checked out into ~/fdmdv2-dev:

  $ sudo apt-get install libgtk2.0-dev libhamlib-dev libsamplerate-dev libasound2-dev libao-dev
  $ cd fdmdv2-dev
  $ mkdir build_linux
  $ cd build_linux
  $ cmake -DBOOTSTRAP_WXWIDGETS=TRUE ~/fdmdv2-dev
  $ make

2/ Then you can configure FreeDV using your local codec-dev, something like:

  $ cmake -DCMAKE_BUILD_TYPE=Debug -DBOOTSTRAP_WXWIDGETS=TRUE -DCODEC2_INCLUDE_DIRS=/path/to/codec2-dev/src -DCODEC2_LIBRARY=/path/to/codec2-dev/build_linux/src/libcodec2.so -DUSE_STATIC_CODEC2=FALSE -DUSE_STATIC_PORTAUDIO=TRUE -DUSE_STATIC_SOX=TRUE ../

3/ OR build a local copy of codec2-dev:

  $ cmake -DBOOTSTRAP_WXWIDGETS=TRUE -DUSE_STATIC_CODEC2=TRUE -DUSE_STATIC_PORTAUDIO=TRUE -DUSE_STATIC_SOX=TRUE ../
  
4/ Build and run FreeDV:

   $ make
   $ ./src/freedv

Quickstart 2
------------

1/ Assuming you have all the dependant libraries:

  $ cd /path/to/fdmdv2
  $ mkdir build_linux
  $ cd build_linux
  $ cmake ../ (defaults to /usr/local, use CMAKE_INSTALL_PREFIX to override)
  (if no errors)
  $ make
  (as root)
  $ make install


=======================================================
 Building for Windows on Ubuntu Linux (Cross compiling)
=======================================================

1/ Install the cross compiling toolchain:

  $ sudo apt-get install mingw-w64

2/ Patch cmake using: http://www.cmake.org/gitweb?p=stage/cmake.git;a=patch;h=33286235048495ceafb636d549d9a4e8891967ae

3/ Checkout a fresh copy of codec2-dev and build for Windows, pointing to the generate_codebook built by a linux build of generate_codebook, using this cmake line

  $ cmake .. -DCMAKE_TOOLCHAIN_FILE=../fdmdv2-dev/cmake/Toolchain-Ubuntu-mingw32.cmake -DUNITTEST=FALSE -DGENERATE_CODEBOOK=/home/david/codec2-dev/build_linux/src/generate_codebook 

4/ Build WxWidgets

  $ cd /path/to/fdmdv2-dev
  $ mkdir build_windows
  $ cd build_windows
  $ cmake -DBOOTSTRAP_WXWIDGETS=TRUE .. -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-Ubuntu-mingw32.cmake -DCMAKE_BUILD_TYPE=Debug
  $ make

5/ Download and install the Windows version of Hamlib:

  $ wget http://internode.dl.sourceforge.net/project/hamlib/hamlib/1.2.15.3/hamlib-win32-1.2.15.3.zip
  $ unzip hamlib-win32-1.2.15.3.zip

6/ Build All the libraries and FreeDV:

  $ cmake -DBOOTSTRAP_WXWIDGETS=TRUE -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-Ubuntu-mingw32.cmake -DUSE_STATIC_PORTAUDIO=TRUE -DUSE_STATIC_SNDFILE=TRUE -DUSE_STATIC_SAMPLERATE=TRUE -DUSE_STATIC_SOX=TRUE -DUSE_STATIC_CODEC2=FALSE -DCODEC2_INCLUDE_DIRS=/home/david/tmp/codec2-dev/src -DCODEC2_LIBRARY=/home/david/tmp/codec2-dev/build_windows/src/libcodec2.dll.a -DHAMLIB_INCLUDE_DIR=hamlib-win32-1.2.15.3/include -DHAMLIB_LIBRARY=hamlib-win32-1.2.15.3/lib/gcc/libhamlib.dll.a -DCMAKE_BUILD_TYPE=Debug ..
  $ make

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

[ ] test frames 
    [X] freedv API support
    [X] BER displayed on GUI for 700 and 1600
    [ ] plot error patterns for 700 and 1600
        + callback for error patterns, or poll via stats interface
    [ ] plot error histograms for 700 and 1600
        + map bit error to carrier, have done this in tcohpsk?
        + how to reset histogram?  On error reset?
        + histogram screen ... new code?
        + test with filter
    [ ] Plot per carrier average Es/No, or even just Es
        + good proxy for error histograms if No is constant
        + this is just a "slow" FFT option I think......

[X] Mel Bugs
    [X] resync issue
    [X] equalise power on 700 and 1600
    [X] research real and complex PAPR
    [X] waterfall and spectrum in analog mode
    [X] The waterfall in analog mode appears to quit working sometimes?

    [X] On TX, intermittently PTT will cause signal to be heard in speakers.  Toggle PTT or 
        Stop/Start toggle and then starts working.
    [X] Squelch control on 1600 mode will not open up squelch to 0 (appears to be around 2 dB)
    [X] space bar keys PTT when entering text info box
    [X] checksum based txt reception
        + only print if valid
    [ ] short varicode doesn't work
        + #ifdef-ed out for now
        + cld be broken in freedv_api

[ ] FreeDV 700 improvements
    [ ] bpf filter after clipping to remove clicks
        [ ] tcohpsk first, measure PAPR, impl loss
    [ ] error masking
        [ ] excitation params
        [ ] training
    [ ] plotting other demod stats like ch ampl and phase ests
    [ ] profile with perf, different libresample routine
    [ ] check for occassional freedv 700 loss of sync
        + scatter seems to jump

[X] win32
    [X] X-compile works
    [X] basic installer
    [X] Win32 installer
        + Richard has taken care of this

[ ] Small fixes
    [X] Playfile bug
    [X] running again
    [X] bump ver number
    [X] long varicode default
    [X] option to _not_ require checksum, on by default
    [X] default squelch 2dB
    [ ] cmd line file decode
    [ ] Waterfall direction
    [ ] documentation or use, walk through,you tube, blog posts
    [ ] scatter diagram agc with hysteresis
    [ ] scatter diagram useful on fading channels in real time (longer persistance?)

[ ] Web support for Presence/spotting hooks

