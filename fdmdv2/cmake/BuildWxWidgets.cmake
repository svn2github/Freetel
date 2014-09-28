set(WXWIDGETS_TARBALL "wxWidgets-3.0.0")

include(ExternalProject)
ExternalProject_Add(wxWidgets
    URL http://downloads.sourceforge.net/wxwindows/${WXWIDGETS_TARBALL}.tar.bz2
    BUILD_IN_SOURCE 1
    INSTALL_DIR external/dist
    if(MINGW)
        CONFIGURE_COMMAND mingw32-make -f ${source_dir}/build/msw/makefile.gcc SHARED=0 UNICODE=1 BUILD=release PREFIX=${CMAKE_BINARY_DIR}/external/dist
    else(MINGW)
        CONFIGURE_COMMAND ./configure --disable-shared --prefix=${CMAKE_BINARY_DIR}/external/dist
    endif(MINGW)
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
)

ExternalProject_Get_Property(wxWidgets install_dir)
set(WXCONFIG "${install_dir}/bin/wx-config")
set(WXRC "${install_dir}/bin/wxrc")
if(EXISTS ${WXCONFIG})
    set(BS_WX_DONE TRUE)
endif()
