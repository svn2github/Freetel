/// \file ptt_input.cpp
/// Implement the virtual base class of PTT input device drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  PTTInput::PTTInput(const char * _name, const char * _parameters)
  : IODevice(_name, _parameters)
  {
  }

  PTTInput::~PTTInput()
  {
  }
}
