#include "drivers.h"
#include <string.h>

namespace FreeDV {
  char *
  copy_string(const char * s)
  {
    const size_t length(strlen(s) + 1);
    char * copy = new char[length];
    memcpy(copy, s, length);
    return copy;
  }
}
