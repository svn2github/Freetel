/// \file user_interface.cpp
/// The virtual base class for user interface drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  UserInterface::UserInterface(const char * name, const char * parameters, Interfaces *)
  : IODevice(name, parameters)
  {
  }

  UserInterface::~UserInterface()
  {
  }
}
