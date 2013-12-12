#include "drivers.h"
#include <iostream>

namespace FreeDV {
  AudioInput::AudioInput()
  {
  }

  AudioInput::~AudioInput()
  {
  }

  bool
  AudioInput::captive()
  {
    return false;
  }
};
