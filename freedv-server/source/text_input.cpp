/// \file
/// Implement the virtual base class for text input drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  TextInput::TextInput(const char * _name, const char * _parameters)
  : IODevice(_name, _parameters)
  {
  }

  TextInput::~TextInput()
  {
  }

  void
  TextInput::set(const char *)
  {
  }
}
