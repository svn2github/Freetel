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

  Modem *
  Driver::ModemNoOp(const char * parameter)
  {
    return new ::FreeDV::ModemNoOp(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_modem("no-op", Driver::ModemNoOp);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
