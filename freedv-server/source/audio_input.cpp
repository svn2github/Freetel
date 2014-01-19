/// The AudioInput class.

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
