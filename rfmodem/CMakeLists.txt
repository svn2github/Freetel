#
# Codec2 - Next-Generation Digital Voice for Two-Way Radio
#
# CMake configuration contributed by Richard Shaw (KF5OIM)
# Please report questions, comments, problems, or patches to the freetel
# mailing list: https://lists.sourceforge.net/lists/listinfo/freetel-codec2
#

cmake_minimum_required(VERSION 2.8)

#
# Prevent in-source builds
# If an in-source build is attempted, you will still need to clean up a few
# files manually.
#
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(rfmodem)


#
# Set project version information. This should probably be done via external
# file at some point.
#
set(RFMODEM_VERSION_MAJOR 0)
set(RFMODEM_VERSION_MINOR 1)
# Set to patch level is needed, otherwise leave FALSE.
set(RFMODEM_VERSION_PATCH FALSE)
set(RFMODEM_VERSION "${RFMODEM_VERSION_MAJOR}.${RFMODEM_VERSION_MINOR}")
# Patch level version bumps should not change API/ABI.
set(SOVERSION "${RFMODEM_VERSION_MAJOR}.${RFMODEM_VERSION_MINOR}")
if(RFMODEM_VERSION_PATCH)
    set(PROJECT_VERSION "${RFMODEM_VERSION}.${RFMODEM_VERSION_PATCH}")
endif(RFMODEM_VERSION_PATCH)
message(STATUS "codec2 version: ${RFMODEM_VERSION}")

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

#
# Setup Windows/MinGW specifics here.
#
if(MINGW)
    message(STATUS "System is MinGW.")
    foreach(RUNTIME
        libgcc_s_sjlj-1.dll
        libstdc++-6.dll)
        message(STATUS "Checking for ${RUNTIME}")
        find_library(${RUNTIME}_LIB ${RUNTIME})
        message(STATUS "runtime found: ${${RUNTIME}_LIB}")
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${${RUNTIME}_LIB})
    endforeach()
endif(MINGW)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")

#
# Default options
#a
option(BUILD_SHARED_LIBS
    "Build shared library. Set to OFF for static library." ON)


# Math library is automatic on windows
#if(UNIX)
#    set(CMAKE_REQUIRED_INCLUDES math.h)
#    set(CMAKE_REQUIRED_LIBRARIES m)
#endif(UNIX)

include(CheckIncludeFiles)
check_include_files("stdlib.h" HAVE_STDLIB_H)
check_include_files("stdio.h" HAVE_STRING_H)
check_include_files("assert.h" HAVE_ASSERT_H)

#include(CheckFunctionExists)

#
# rfmodem library
#
#add_subdirectory(src)

set(RFMODEM_SRCS golay23.c)

set(RFMODEM_PUBLIC_HEADERS golay23.h)

add_library(rfmodem ${RFMODEM_SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rfmodem ${CMAKE_REQUIRED_LIBRARIES})
set_target_properties(rfmodem PROPERTIES SOVERSION ${SOVERSION})

install(TARGETS rfmodem
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    RUNTIME DESTINATION bin)

install(FILES ${RFMODEM_PUBLIC_HEADERS}
    DESTINATION include/rfmodem)


#
# Cpack NSIS installer configuration for Windows.
# See: http://nsis.sourceforge.net/Download
#
# *nix systems should use "make install" and/or appropriate
# distribution packaging tools.
#
if(WIN32)
    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Next-Generation Digital Voice for Two-Way Radio")
    set(CPACK_PACKAGE_VENDOR "CMake")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
    set(CPACK_PACKAGE_VERSION_MAJOR ${RFMODEM_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${RFMODEM_VERSION_MINOR})
    if(RFMODEM_VERSION_PATCH)
        set(CPACK_PACKAGE_VERSION_PATCH ${RFMODEM_VERSION_PATCH})
    endif(RFMODEM_VERSION_PATCH)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Codec2")
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\codec2.org")
    set(CPACK_NSIS_MODIFY_PATH ON)
    include(CPack)
endif(WIN32)
