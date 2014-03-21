/// \file audio_output.cpp
/// Implementation of the AudioOutput class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  AudioOutput::AudioOutput(const char * name, const char * parameters)
  : AudioDevice(name, parameters)
  {
  }

  AudioOutput::~AudioOutput()
  {
  }
}
