#include "drivers.h"
#include <iostream>

namespace FreeDV {
  AudioOutput::AudioOutput()
  {
  }

  AudioOutput::~AudioOutput()
  {
  }

  bool
  AudioOutput::captive()
  {
    return false;
  }
};
