/// The virtual base class for modem drivers.

#include "drivers.h"

namespace FreeDV {
  Modem::Modem(const char * name, const char * parameters)
  : Base(name, parameters)
  {
  }

  Modem::~Modem()
  {
  }
}
