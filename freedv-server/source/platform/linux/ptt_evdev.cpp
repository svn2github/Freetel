/// \file ptt_evdev.cpp
/// Implementation of the evdev PTT driver.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "evdev.h"
#include "drivers.h"
#include <iostream>
#include <stdexcept>

static bool
bit_set(unsigned int bit, const uint8_t * field)
{
  return ((field[bit / 8] & (1 << (bit % 8))) != 0);
}

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
    std::size_t				count = 0;
    EvDev::device_enumeration * const	devices = EvDev::enumerate(count);
  
    for ( std::size_t i = 0; i < count; i++ ) {
      if ( bit_set(EV_KEY, devices[i].event_types) ) {
        int	low = -1;
        int	high = -1;

        for ( int j = KEY_F1; j <= KEY_MAX; j++ ) {
          if ( bit_set(j, devices[i].buttons) ) {
            low = j;
            break;
          }
        }
        for ( int j = KEY_MAX; j >= KEY_F1; j-- ) {
          if ( bit_set(j, devices[i].buttons) ) {
            high = j;
            break;
          }
        }
        if ( low >= 0 ) {
          stream << '\"' << devices[i].name << ',' << low << '\"';
         
          if ( high > low )
            stream << " (" << low << '-' << high << ')';
  
          stream << std::endl;
        }
      }
    }
  
    EvDev::delete_enumeration(devices, count);
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
