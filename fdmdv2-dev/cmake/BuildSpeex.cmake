set(SPEEXDSP_TARBALL "speexdsp-1.2rc3.tar.gz")
#set(SPEEXDSP_TARBALL "speex-1.0.5.tar.gz")

include(ExternalProject)
ExternalProject_Add(speex
   URL http://downloads.xiph.org/releases/speex/${SPEEXDSP_TARBALL}
   BUILD_IN_SOURCE 1
   INSTALL_DIR external/dist
   CONFIGURE_COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/external/dist --disable-examples
   BUILD_COMMAND $(MAKE)
   INSTALL_COMMAND $(MAKE) install
)

set(SPEEXDSP_LIBRARIES ${CMAKE_BINARY_DIR}/external/dist/lib/libspeexdsp.a)
include_directories(${CMAKE_BINARY_DIR}/external/dist/include)
list(APPEND FREEDV_LINK_LIBS ${SPEEXDSP_LIBRARIES})
list(APPEND FREEDV_STATIC_DEPS speex)
