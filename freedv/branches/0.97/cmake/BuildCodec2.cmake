if(CMAKE_CROSSCOMPILING)
    set(CODEC2_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()

include(ExternalProject)
ExternalProject_Add(codec2
   SVN_REPOSITORY https://svn.code.sf.net/p/freetel/code/codec2
   CMAKE_ARGS ${CODEC2_CMAKE_ARGS}
   INSTALL_COMMAND ""
)
set(CODEC2_LIBRARIES
    ${CMAKE_BINARY_DIR}/codec2-prefix/src/codec2-build/src/libcodec2.a)
include_directories(${CMAKE_BINARY_DIR}/codec2-prefix/src/codec2/src)
list(APPEND FREEDV_LINK_LIBS ${CODEC2_LIBRARIES})
list(APPEND FREEDV_STATIC_DEPS codec2)
