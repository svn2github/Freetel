#include <cstdint>
#include <linux/input.h>
#include "drivers.h"

namespace FreeDV {
  class EvDev {
  public:
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
    static void		delete_enumeration(
   			 device_enumeration *	data,
  			 std::size_t		count);
  
    static device_enumeration *
  			enumerate(std::size_t & count);
  
    int			poll_fds(PollType *, int);

    std::size_t		ready();

    std::size_t		read_events(input_event * data, std::size_t count);

    			EvDev(const char * name);
  			~EvDev();
  };
}
