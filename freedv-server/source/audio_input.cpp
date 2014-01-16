#include "drivers.h"

namespace FreeDV {
  AudioInput::AudioInput(const char * parameters)
  {
  }

  AudioInput::~AudioInput()
  {
  }

  bool const
  AudioInput::captive() const
  {
    return false;
  }
}
