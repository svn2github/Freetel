#include "drivers.h"

namespace FreeDV {
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
