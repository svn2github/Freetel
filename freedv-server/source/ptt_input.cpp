/// \file ptt_input.cpp
/// Implement the virtual base class of PTT input device drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  PTTInput::PTTInput(const char * name, const char * parameters)
  : IODevice(name, parameters)
  {
  }

  PTTInput::~PTTInput()
  {
  }
}
