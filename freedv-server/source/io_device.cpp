/// The I/O Device class.

#include "drivers.h"

namespace FreeDV {
  IODevice::IODevice(const char * name, const char * parameters)
  : Base(name, parameters)
  {
  }

  IODevice::~IODevice()
  {
  }
}
