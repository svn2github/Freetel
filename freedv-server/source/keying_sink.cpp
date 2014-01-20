/// The KeyingSink class, called as "sink", doesn't key anything, for testing
/// or use with VOX.

#include "drivers.h"
#include <iostream>

namespace FreeDV {
  /// KeyingOutput output "sink", doesn't key anything. For testing or use with VOX.
  class KeyingSink : public KeyingOutput {
  public:

	/// Instantiate keying sink driver.
  		KeyingSink(const char *);
	virtual	~KeyingSink();

	/// If the value is true, transmit. Otherwise receive.
	void	key(bool value);
  };

  KeyingSink::KeyingSink(const char * parameters)
  : KeyingOutput("sink", parameters)
  {
  }

  KeyingSink::~KeyingSink()
  {
  }

  void
  KeyingSink::key(bool value)
  {
    if ( value )
      std::cerr << "keying: TRANSMIT" << std::endl;
    else
      std::cerr << "keying: RECEIVE" << std::endl;
  }

  KeyingOutput *
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
