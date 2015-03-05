set(WXWIDGETS_TARBALL "wxWidgets-3.0.2")

# If we're crosscompiling then we need to set the target hose correctly.
if(MINGW AND CMAKE_CROSSCOMPILING)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(HOST x86_64-w64-mingw32)
    else()
        set(HOST i686-w64-mingw32)
    endif()
endif()

if(MINGW AND NOT CMAKE_CROSSCOMPILING)
    set(CONFIGURE_COMMAND mingw32-make -C build/msw -f makefile.gcc SHARED=0 UNICODE=1 BUILD=release PREFIX=${CMAKE_BINARY_DIR}/external/dist)
else()
    set(CONFIGURE_COMMAND ./configure --host=${HOST} --target=${HOST} --disable-shared --prefix=${CMAKE_BINARY_DIR}/external/dist)
endif()

include(ExternalProject)
ExternalProject_Add(wxWidgets
    URL http://downloads.sourceforge.net/wxwindows/${WXWIDGETS_TARBALL}.tar.bz2
    BUILD_IN_SOURCE 1
    INSTALL_DIR external/dist
    CONFIGURE_COMMAND ${CONFIGURE_COMMAND}
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
)

ExternalProject_Get_Property(wxWidgets install_dir)
message(STATUS "wxWidgets install dir: ${install_dir}")
set(WXCONFIG "${install_dir}/bin/wx-config")
if(EXISTS ${WXCONFIG})
    set(BS_WX_DONE TRUE)
endif()
