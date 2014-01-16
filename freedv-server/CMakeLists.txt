#
# FreeDV - HF Digital Voice for Radio Amateurs
#
# CMake configuration started with the version written by Richard Shaw KF5OIM
# for the original Qt FreeDV.
# Hacked to bits for command-line/server version by Bruce Perens K6BP.
#
# Note: There is an embedded and not-embedded version of FreeDV built here.
# The embedded version avoids use of the C++ standard template library,
# templates, and anything that would make it large. It uses tiny_main.
# The non-embedded version uses big_main and driver_manager. It uses
# STL templates, and iostreams, and probably has a tremendous user interface
# driver so none of this matters.
#
# Please report questions, comments, problems, or patches to the freetel
# mailing list: https://lists.sourceforge.net/lists/listinfo/freetel-codec2
#

# Prevent in-source builds to protect automake/autoconf config.
# If an in-source build is attempted, you will still need to clean up a few
# files manually.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds in ${CMAKE_BINARY_DIR} are not "   
   "allowed, please remove ./CMakeCache.txt and ./CMakeFiles/, create a "
   "separate build directory and run cmake from there.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

cmake_minimum_required(VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(freedv-server)

include(cmake/UseDoxygen.cmake)

# Parameters are stored as .txt files in the parameters/ subdirectory.
# They use semicolon to separate arguments, because this is what cmake uses
# in its list type. Thus, parameters/version.txt looks like this:
#
#	0;1;2
#
# Those are easy to separate at each semicolon into the major, minor,
# and patch version.
#
# You can override parameters on the command line, using
# -DParam.name=value
# 
macro(load_parameters)
  foreach(name ${ARGN})
    set(filename "${CMAKE_SOURCE_DIR}/parameters/${name}.txt")
    if("${Param.${name}}" STREQUAL "" AND EXISTS ${filename})
      file(STRINGS ${filename} string)
      set("Param.${name}" ${string})
    endif("${Param.${name}}" STREQUAL "" AND EXISTS ${filename})
  endforeach(name)
  unset(filename)
endmacro(load_parameters)

#
# Parse FreeDV version, code-name from parameter.
#
load_parameters(version codename build-type)
list(GET Param.version 0 Version.major)
list(GET Param.version 1 Version.minor)
list(GET Param.version 2 Version.patch)
unset(list)

#
# Parse platform information
#
set(Platform.name ${CMAKE_SYSTEM_NAME})
if(Platform.name STREQUAL Linux)
  execute_process(
   COMMAND lsb_release -s -i
   OUTPUT_VARIABLE Platform.distribution
   OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(
   COMMAND lsb_release -s -r
   OUTPUT_VARIABLE Platform.release
   OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(Platform.name STREQUAL Linux)

message(STATUS "${Param.build-type} ${Version.major}.${Version.minor}.${Version.patch}, "
 "code name \"${Param.codename}\", "
 "on ${Platform.name} ${Platform.distribution} ${Platform.release}.")

#
# Set cmake internal variables from parameters.
#
set(Compile.sources
  source/audio_input.cpp
  source/audio_output.cpp
  source/audio_sink.cpp
  source/big_main.cpp
  source/blank_panel.cpp
  source/codec.cpp
  source/codec_noop.cpp
  source/driver_manager.cpp
  source/keying.cpp
  source/keying_sink.cpp
  source/modem.cpp
  source/modem_noop.cpp
  source/ptt_input.cpp
  source/ptt_constant.cpp
  source/run.cpp
  source/text_constant.cpp
  source/text_input.cpp
  source/tiny_main.cpp
  source/tone.cpp
  source/user_interface.cpp
)

load_parameters(cxx-flags)
set_source_files_properties(${Compile.sources} PROPERTIES COMPILE_FLAGS "${Param.cxx-flags}")


add_executable(freedv-server ${Compile.sources})

find_path(Codec2.include codec2.h PATH_SUFFIXES codec2)
find_library(Codec2.lib NAMES codec2)
if( Codec2.lib AND Codec2.include )
  message(STATUS "Getting Codec2 from ${Codec2.include} and ${Codec2.lib}")
  include_directories(${Codec2.include})
  target_link_libraries(freedv-server ${Codec2.lib})
else( Codec2.lib AND Codec2.include )
  message(FATAL_ERROR "Can't find Codec2 library.")
endif( Codec2.lib AND Codec2.include )

install(TARGETS freedv-server RUNTIME DESTINATION bin)
