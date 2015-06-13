set(WXWIDGETS_TARBALL "wxWidgets-3.0.2")

# If we're cross-compiling then we need to set the target host manually.
if(MINGW AND CMAKE_CROSSCOMPILING)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(HOST x86_64-w64-mingw32)
    else()
        set(HOST i686-w64-mingw32)
    endif()
endif()

# If not cross-compiling then use the built-in makefile, otherwise use standard configure.
if(MINGW AND NOT CMAKE_CROSSCOMPILING)
#    set(CONFIGURE_COMMAND "true")
#    set(MAKE_COMMAND $(MAKE) -C build/msw -f makefile.gcc SHARED=0 UNICODE=1 BUILD=release PREFIX=${CMAKE_BINARY_DIR}/external/dist)
    set(CONFIGURE_COMMAND ./configure --disable-shared --prefix=${CMAKE_BINARY_DIR}/external/dist)
	set(MAKE_COMMAND $(MAKE))
else()
    set(CONFIGURE_COMMAND ./configure --host=${HOST} --target=${HOST} --disable-shared --prefix=${CMAKE_BINARY_DIR}/external/dist)
	set(MAKE_COMMAND $(MAKE))
endif()

include(ExternalProject)
ExternalProject_Add(wxWidgets
    URL http://downloads.sourceforge.net/wxwindows/${WXWIDGETS_TARBALL}.tar.bz2
    BUILD_IN_SOURCE 1
    INSTALL_DIR external/dist
    CONFIGURE_COMMAND ${CONFIGURE_COMMAND}
    BUILD_COMMAND ${MAKE_COMMAND}
    INSTALL_COMMAND $(MAKE) install
)

ExternalProject_Get_Property(wxWidgets install_dir)
message(STATUS "wxWidgets install dir: ${install_dir}")
if(NOT WXCONFIG)
    set(WXCONFIG "${install_dir}/bin/wx-config")
endif()
if(EXISTS ${WXCONFIG})
    set(BS_WX_DONE TRUE)
endif()
