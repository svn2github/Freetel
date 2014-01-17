/// The virtual base class for user interface drivers.

#include "drivers.h"

namespace FreeDV {
  UserInterface::UserInterface(const char * name, const char * parameters, Interfaces * interfaces)
  : Base(name, parameters)
  {
  }

  UserInterface::~UserInterface()
  {
  }
}