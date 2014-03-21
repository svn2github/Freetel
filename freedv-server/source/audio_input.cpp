/// \file audio_input.cpp
/// Implementation of the AudioInput class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  AudioInput::AudioInput(const char * name, const char * parameters)
  : AudioDevice(name, parameters)
  {
  }

  AudioInput::~AudioInput()
  {
  }
}
