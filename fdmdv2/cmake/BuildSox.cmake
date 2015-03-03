set(SOX_TARBALL "sox-14.4.1")

# required linking libraries on linux. Not sure about windows.
find_library(ALSA_LIBRARIES asound)
find_library(AO_LIBRARIES ao)

if(UNIX AND NOT ALSA_LIBRARIES)
    message(ERROR "Could not find alsa library.
On Linux systems try installing:
    alsa-lib-devel  (RPM based systems)
    libasound2-dev  (DEB based systems)"
    )
endif(UNIX AND NOT ALSA_LIBRARIES)

if(UNIX AND NOT AO_LIBRARIES)
    message(ERROR "Could not find libao.
On Linux systems try installing:
    libao-devel  (RPM based systems)
    libao-dev    (DEB based systems)"
    )
endif(UNIX AND NOT AO_LIBRARIES)

# For some reason libsox is not linked against linsndfile. We workaround it
# here but need to know if we're using the system provided library or not.
#if(USE_STATIC_SNDFILE)
#    # Use known location of internal build.
#    set(SNDFILE_LIB ${SNDFILE_LIBRARIES})
#else()
#    # Use found system library.
#    set(SNDFILE_LIB ${LIBSNDFILE})
#endif()

include(ExternalProject)
ExternalProject_Add(sox
    URL http://downloads.sourceforge.net/sox/${SOX_TARBALL}.tar.gz
    BUILD_IN_SOURCE 1
    INSTALL_DIR external/dist
    CONFIGURE_COMMAND ./configure --enable-shared=no --without-id3tag --without-png --disable-gomp --with-oggvorbis=no --with-oss=no --with-flac=no --with-amrnb=no --with-amrwb=no --with-mp3=no --with-wavpack=no --disable-dl-sndfile --with-pulseaudio=no --without-magic --prefix=${CMAKE_BINARY_DIR}/external/dist
    BUILD_COMMAND $(MAKE) V=1 LIBS=${SNDFILE_LIB}
    INSTALL_COMMAND $(MAKE) install
)
set(SOX_LIBRARIES ${CMAKE_BINARY_DIR}/external/dist/lib/libsox.a)
if(UNIX)
    list(APPEND SOX_LIBRARIES ${ALSA_LIBRARIES} ${AO_LIBRARIES})
endif()
if(MINGW)
    list(APPEND SOX_LIBRARIES winmm)
endif()
include_directories(${CMAKE_BINARY_DIR}/external/dist/include)
list(APPEND FREEDV_LINK_LIBS ${SOX_LIBRARIES})
list(APPEND FREEDV_STATIC_DEPS sox)