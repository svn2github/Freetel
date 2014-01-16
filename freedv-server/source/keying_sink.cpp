#include "drivers.h"

namespace FreeDV {
  /// Keying output "sink", doesn't key anything. For testing or use with VOX.
  class KeyingSink : public Keying {
  public:

	/// Instantiate keying sink driver.
  		KeyingSink(const char *);
	virtual	~KeyingSink();

	virtual void
		key(bool value);

  };

  KeyingSink::KeyingSink(const char * parameters)
  : Keying(parameters)
  {
  }

  KeyingSink::~KeyingSink()
  {
  }

  void
  KeyingSink::key(bool value)
  {
  }

#ifndef NO_INITIALIZERS
  static Keying *
  creator(const char * parameter)
  {
    return new KeyingSink(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_keying_output("sink", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
