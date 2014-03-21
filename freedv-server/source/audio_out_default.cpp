/// \file audio_out_default.cpp
/// Used to configure the default audio output in embedded systems.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  /// Open the default audio output.
  /// This function is intended for use in embedded programs that don't
  /// use the device driver manager.
  AudioOutput *
  Driver::AudioOutDefault()
  {
#if __linux__
    return Driver::AudioOutALSA("default");
#else
    #error Please fill in Driver::AudioOutDefault for this platform.
#endif
  }
}
