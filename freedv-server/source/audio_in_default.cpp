#include "drivers.h"

namespace FreeDV {
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
