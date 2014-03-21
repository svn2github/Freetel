/// \file audio_in_default.cpp
/// Used to configure the default audio input device in embedded systems.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  /// Open the default audio input.
  /// This function is intended for use in embedded programs that don't use
  /// the device driver manager.
  AudioInput *
  Driver::AudioInDefault()
  {
#if __linux__
    return Driver::AudioInALSA("default");
#else
    #error Please fill in Driver::AudioInDefault for this platform.
#endif
  }
}
