/// \file keying.cpp
/// Implementation of the KeyingOutput device driver class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  KeyingOutput::KeyingOutput(const char * name, const char * parameters)
  : IODevice(name, parameters)
  {
  }

  KeyingOutput::~KeyingOutput()
  {
  }
}
