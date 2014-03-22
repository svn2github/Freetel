/// \file user_interface.cpp
/// The virtual base class for user interface drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  UserInterface::UserInterface(const char * _name, const char * _parameters, Interfaces * _interfaces)
  : IODevice(_name, _parameters), interfaces(_interfaces)
  {
  }

  UserInterface::~UserInterface()
  {
  }
}
