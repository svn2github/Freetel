/// \file audio_output.cpp
/// Implementation of the AudioOutput class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  AudioOutput::AudioOutput(const char * _name, const char * _parameters)
  : AudioDevice(_name, _parameters)
  {
  }

  AudioOutput::~AudioOutput()
  {
  }
}
