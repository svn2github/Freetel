/// \file audio_device.cpp
/// Implementation of AudioDevice, which is the base class for all audio
/// devices.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <stdexcept>

namespace FreeDV {
  AudioDevice::AudioDevice(const char * name, const char * parameters)
  : IODevice(name, parameters), master_amplitude(1.0)
  {
  }

  AudioDevice::~AudioDevice()
  {
  }

  float
  AudioDevice::amplitude()
  {
    return master_amplitude;
  }

  void
  AudioDevice::amplitude(float value)
  {
    if ( value < 0.0 || value > 1.0 )
      throw std::runtime_error(
       "Amplitude set to value outside of the range 0.0..1.0");
    master_amplitude = value;
  }
}
