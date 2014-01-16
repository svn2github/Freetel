/// The AudioOutput class.

#include "drivers.h"

namespace FreeDV {
  AudioOutput::AudioOutput(const char * parameters)
  {
  }

  AudioOutput::~AudioOutput()
  {
  }

  bool const
  AudioOutput::captive() const
  {
    return false;
  }
}
