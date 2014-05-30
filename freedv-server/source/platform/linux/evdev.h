/// \file evdev.h
/// Implementation of the evdev PTT driver.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///
#include <cstdint>
#include <linux/input.h>
#include "drivers.h"

namespace FreeDV {
  /// Class used to access Linux event devices.
  class EvDev {
  public:
    /// Structure used to enumerate Linux event devices.
    struct device_enumeration {
      char *	special_file;
      char *	name;
      uint8_t	event_types[(EV_MAX + 7) / 8];
      uint8_t	buttons[(KEY_MAX + 7) / 8];
    };
  
  private:
    int			fd;
    const char *	name;
    const char *	special_file;
  
  			EvDev(const EvDev &);
    EvDev &		operator =(const EvDev &);
  
    static bool		enumerate_device(
  			 const char *			name,
  			 int				fd,
  			 struct device_enumeration &	device);
  
  public:
    bool		button_state(unsigned int index);

    static void		delete_enumeration(
   			 device_enumeration *	data,
  			 std::size_t		count);
  
    const char *	device_name() { return special_file; }

    static device_enumeration *
  			enumerate(std::size_t & count);
  
    bool		has_button(unsigned int index);

    const char *	long_name() { return name; }

    int			poll_fds(PollType *, int);

    std::size_t		read_events(input_event * data, std::size_t count);

    			EvDev(const char * name);
  			~EvDev();
  };
}
