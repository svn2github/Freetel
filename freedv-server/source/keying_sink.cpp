/// \file keying_sink.cpp
/// The KeyingSink class, called as "sink", doesn't key anything, for testing
/// or use with VOX.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///


#include "drivers.h"
#include <iostream>

namespace FreeDV {
  /// KeyingOutput output "sink", doesn't key anything.
  /// For testing or use with VOX.
  class KeyingSink : public KeyingOutput {
  public:
    /// Instantiate keying sink driver.
    ///
			KeyingSink(const char *);
    virtual		~KeyingSink();

    /// Return file descriptors for poll()
    /// \param array The address of an array that will be written
    /// with a sequence of file descriptors.
    /// \param space The maximum number of file descriptors that may be
    /// stored in the array.
    /// \return The number of file descriptors written to the array.
    int			poll_fds(PollType * array, int space);

    /// If the value is true, transmit. Otherwise receive.
    ///
    void		key(bool value);

    /// Return the amount of bytes ready for read. In this case, it always
    /// returns SIZE_MAX.
    std::size_t		ready();
  };

  KeyingSink::KeyingSink(const char * _parameters)
  : KeyingOutput("sink", _parameters)
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

  int
  KeyingSink::poll_fds(PollType *, int)
  {
    return 0;
  }

  std::size_t
  KeyingSink::ready()
  {
    return SIZE_MAX;
  }

  KeyingOutput *
  Driver::KeyingSink(const char * parameter)
  {
    return new ::FreeDV::KeyingSink(parameter);
  }

  static std::ostream &
  KeyingSinkEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_keying_output("sink", Driver::KeyingSink, KeyingSinkEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
