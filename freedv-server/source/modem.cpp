/// \file modem.cpp
/// Implementation of the virtual base class for modem algorithm drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  Modem::Modem(const char * name, const char * parameters)
  : Base(name, parameters)
  {
  }

  Modem::~Modem()
  {
  }
}
