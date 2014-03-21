/// \file utility.cpp
/// Catch-all for utility functions.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  char *
  copy_string(const char * s)
  {
    const std::size_t length(strlen(s) + 1);
    char * copy = new char[length];
    memcpy(copy, s, length);
    return copy;
  }
}
