#include "drivers.h"

namespace FreeDV {
  PTTInput::PTTInput(const char * parameters)
  {
  }

  PTTInput::~PTTInput()
  {
  }

  bool const
  PTTInput::captive() const
  {
    return false;
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
