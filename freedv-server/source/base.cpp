/// \file base.cpp
/// Implementation of the general base class for drivers, both algorithm
/// drivers like Modem and device drivers like AudioInput.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  Base::Base(const char * _name, const char * _parameters)
  : name(_name), parameters(copy_string(_parameters))
  {
  }

  Base::~Base()
  {
    delete parameters;
  }

  bool
  Base::captive() const
  {
    return false;
  }

  std::ostream &
  Base::print(std::ostream & stream) const
  {
    return stream << name << ':' << parameters;
  }
}
