/// \file framer.cpp
/// The Protocol Framer base class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  Framer::Framer(const char * _name, const char * _parameters)
  : Base(_name, _parameters)
  {
  }

  Framer::~Framer()
  {
  }
}
