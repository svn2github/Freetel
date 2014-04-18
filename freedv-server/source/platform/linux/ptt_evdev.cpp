/// \file ptt_evdev.cpp
/// Implementation of the evdev PTT driver.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "evdev.h"
#include "drivers.h"
#include <iostream>
#include <stdexcept>

namespace FreeDV {
  /// PTT driver using Linux evdev.
  ///
  class PTT_EvDev : public PTTInput {
  private:
    /// This is true if ready has not yet been sent.
    ///
    bool		pressed;
    bool		ready_one_shot;
  public:
    /// Instantiate.
    ///
    			PTT_EvDev(const char * parameters);
    virtual		~PTT_EvDev();
    
    /// Return file descriptors for poll()
    /// \param array The address of an array that will be written
    /// with a sequence of file descriptors.
    /// \param space The maximum number of file descriptors that may be
    /// stored in the array.
    /// \return The number of file descriptors written to the array.
    virtual int	poll_fds(PollType * array, int space);

    /// Return the amount of bytes ready for read.
    ///
    std::size_t	ready();

    /// Return true if the PTT input is pressed.
    ///
    bool	state();
  };

  PTT_EvDev::PTT_EvDev(const char * _parameters)
  : PTTInput("constant", _parameters), pressed(false), ready_one_shot(true)
  {
  }

  PTT_EvDev::~PTT_EvDev()
  {
  }

  int
  PTT_EvDev::poll_fds(PollType *, int)
  {
    return 0;
  }

  std::size_t
  PTT_EvDev::ready()
  {
    if ( ready_one_shot )
      return 1;
    else
      return 0;
  }

  bool
  PTT_EvDev::state()
  {
    return false;
  }

  PTTInput *
  Driver::PTT_EvDev(const char * parameter)
  {
    return new ::FreeDV::PTT_EvDev(parameter);
  }

  static std::ostream &
  PTT_EvDevEnumerator(std::ostream & stream)
  {
    char * *	devices = EvDev::EnumerateButtonDevices();

    char * *	d = devices;

    while ( *d != 0 )
      stream << "\"evdev:" << *d++ << '\"' << std::endl;

    d = devices;

    while ( *d != 0 )
      delete *d++;

    delete devices;

    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_ptt_input("evdev", Driver::PTT_EvDev, PTT_EvDevEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
