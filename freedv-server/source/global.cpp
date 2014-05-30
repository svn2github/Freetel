#include "drivers.h"

namespace FreeDV {
  /// Global name of the program, for error messages.
  const char * program_name = 0;

  /// Global driver manager object.
  DriverManager * _globalDriverManager = 0;
}
