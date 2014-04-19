/// \file ptt_evdev.cpp
/// Implementation of the evdev PTT driver.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "evdev.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace FreeDV {
  NORETURN static void
  do_throw(
   const char * name,
   const char * message = 0)
  {
    std::ostringstream str;

    str << "EVDEV ";

    str << '\"' << name << "\": ";
    if ( message ) {
      str << message;
    }

    str << '.';
    throw std::runtime_error(str.str().c_str());
  }

  inline static bool
  bit_is_set(unsigned int bit, const uint8_t * field)
  {
    return ((field[bit / 8] & (1 << (bit % 8))) != 0);
  }
  
  /// PTT driver using Linux evdev.
  ///
  class PTT_EvDev : public PTTInput {
  private:
    EvDev		* dev;
    int			button_index;
    bool		pressed;
    bool		changed;

    			PTT_EvDev(const PTT_EvDev &);
    			PTT_EvDev & operator =(const PTT_EvDev &);

    void		process_events();

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

    /// Return the amount of events ready for read.
    ///
    std::size_t	ready();

    /// Return true if the PTT input is pressed.
    ///
    bool	state();
  };

  PTT_EvDev::PTT_EvDev(const char * _parameters)
  : PTTInput("evdev", _parameters), dev(0), button_index(0), pressed(false),
    changed(false)
  {
    char *	p = strdup(parameters);
    char *	number = index(p, ',');

    if ( number == 0 )
      do_throw(
       parameters,
       "The device name must be followed by a comma and a button number."); 

    *number++ = '\0';

    button_index = atoi(number);
    
    dev = new EvDev(p);
    delete p;

    if ( !dev->has_button(button_index) ) {
      std::ostringstream str;

      str << "There is no button " << button_index << " in this device.";

      do_throw(parameters, str.str().c_str());
    }

    pressed = dev->button_state(button_index);
  }

  PTT_EvDev::~PTT_EvDev()
  {
    delete dev;
  }

  int
  PTT_EvDev::poll_fds(PollType * array, int count)
  {
    return dev->poll_fds(array, count);
  }

  void
  PTT_EvDev::process_events()
  {
    while ( dev->ready() > 0 ) {
      input_event	events[10];

      const std::size_t count = dev->read_events(
       events,
       sizeof(events) / sizeof(*events));

      for ( std::size_t i = 0; i < count; i++ ) {
        const input_event * const event = &events[i];
        if ( event->type == EV_KEY && event->code == button_index ) {
          switch ( event->value ) {
          case 0:
            if ( pressed )
	      changed = true;
	    pressed = false;
            break;
          case 1:
            if ( !pressed )
	      changed = true;
            pressed = true;
            break;
	  default:
	    ;
	  }
        }
      }
    }
  }

  std::size_t
  PTT_EvDev::ready()
  {
    return dev->ready();
  }

  bool
  PTT_EvDev::state()
  {
    process_events();
    changed = false;
    return pressed;
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
      if ( bit_is_set(EV_KEY, devices[i].event_types) ) {
        int	low = -1;
        int	high = -1;

        for ( int j = KEY_F1; j <= KEY_MAX; j++ ) {
          if ( bit_is_set(j, devices[i].buttons) ) {
            low = j;
            break;
          }
        }
        for ( int j = KEY_MAX; j >= KEY_F1; j-- ) {
          if ( bit_is_set(j, devices[i].buttons) ) {
            high = j;
            break;
          }
        }
        if ( low >= 0 ) {
          stream << "\"evdev:" << devices[i].name << ',' << low << '\"';
         
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
