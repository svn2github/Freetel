/// The no-op modem, for plain SSB voice and testing.

#include "drivers.h"

namespace FreeDV {
  /// Modem "no-op", just copies its input to its output.
  /// For plain SSB voice, and for testing.
  class ModemNoOp : public Modem {
  public:

	/// Instantiate the no-op modem.
  		ModemNoOp(const char *);
	virtual	~ModemNoOp();
  };

  ModemNoOp::ModemNoOp(const char * parameters)
  : Modem("no-op", parameters)
  {
  }

  ModemNoOp::~ModemNoOp()
  {
  }

#ifndef NO_INITIALIZERS
  static Modem *
  creator(const char * parameter)
  {
    return new ModemNoOp(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_modem("no-op", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
