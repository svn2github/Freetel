/// The KeyingSink class, called as "sink", doesn't key anything, for testing
/// or use with VOX.

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
  : Keying("sink", parameters)
  {
  }

  KeyingSink::~KeyingSink()
  {
  }

  void
  KeyingSink::key(bool value)
  {
  }

  Keying *
  Driver::KeyingSink(const char * parameter)
  {
    return new ::FreeDV::KeyingSink(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_keying_output("sink", Driver::KeyingSink);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
