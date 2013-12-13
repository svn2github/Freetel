#include "drivers.h"

// Modem "no-op", just copies its input to its output. For plain SSB voice, and for testing.
namespace FreeDV {
  class ModemNoOp : public Modem {
  public:

  		ModemNoOp(const char *);
		~ModemNoOp();

	void	key(bool value);

  };

  ModemNoOp::ModemNoOp(const char *)
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
