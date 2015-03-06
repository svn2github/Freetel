include(ExternalProject)
ExternalProject_Add(codec2
   SVN_REPOSITORY https://svn.code.sf.net/p/freetel/code/codec2-dev
   CMAKE_ARGS -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/dist
   INSTALL_COMMAND ""
)
set(CODEC2_LIBRARIES
    ${CMAKE_BINARY_DIR}/codec2-prefix/src/codec2-build/src/libcodec2.a)
include_directories(${CMAKE_BINARY_DIR}/codec2-prefix/src/codec2/src)
list(APPEND FREEDV_LINK_LIBS ${CODEC2_LIBRARIES})
list(APPEND FREEDV_STATIC_DEPS codec2)
