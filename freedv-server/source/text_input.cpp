/// The virtual base class for text input drivers.

#include "drivers.h"

namespace FreeDV {
  TextInput::TextInput(const char * name, const char * parameters)
  : IODevice(name, parameters)
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
