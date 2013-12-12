#include "drivers.h"
#include <iostream>

// Keying output "sink", doesn't key anything. For testing or use with VOX.
namespace FreeDV {
  class KeyingSink : public Keying {
  public:

  		KeyingSink(const char *);
		~KeyingSink();

	void	key(bool value);

  };

  KeyingSink::KeyingSink(const char *)
  {
  }

  KeyingSink::~KeyingSink()
  {
  }

  void
  KeyingSink::key(bool value)
  {
  }


  static Keying *
  creator(const char * parameter)
  {
    return new KeyingSink(parameter);
  }

  static bool
  initializer()
  {
    driver_manager.register_keying("sink", creator);
    return true;
  }
  static const bool initialized = initializer();
};
