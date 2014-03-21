/// \file codec.cpp
/// The base class for all codec drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  Codec::Codec(const char * _name, const char * _parameters)
  : Base(_name, _parameters)
  {
  }

  Codec::~Codec()
  {
  }
}
