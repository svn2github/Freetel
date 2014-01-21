/// The virtual base class of PTT input drivers.

#include "drivers.h"

namespace FreeDV {
  PTTInput::PTTInput(const char * name, const char * parameters)
  : IODevice(name, parameters)
  {
  }

  PTTInput::~PTTInput()
  {
  }

  void
  PTTInput::changed(bool value)
  {
    if ( callback )
      (*callback)(value);
  }

  void
  PTTInput::set_callback(void (*c)(bool))
  {
    callback = c;
  }
}
