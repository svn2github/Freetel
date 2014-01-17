/// The Codec base class.

#include "drivers.h"

namespace FreeDV {
  Codec::Codec(const char * name, const char * parameters)
  : Base(name, parameters)
  {
  }

  Codec::~Codec()
  {
  }
}
