/// The AudioOutput class.

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
