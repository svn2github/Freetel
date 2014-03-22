/// \file io_device.cpp
/// Implementation of the I/O Device class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  IODevice::IODevice(const char * _name, const char * _parameters)
  : Base(_name, _parameters)
  {
  }

  IODevice::~IODevice()
  {
  }
}
