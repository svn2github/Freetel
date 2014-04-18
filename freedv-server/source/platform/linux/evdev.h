#include <cstdint>
#include <linux/input.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

namespace FreeDV {
  class EvDev {
  protected:
    struct device_enumeration {
      char *	special_file;
      char *	name;
      uint8_t	event_types[(EV_MAX + 7) / 8];
    };
  
  private:
  
  			EvDev(const EvDev &);
    EvDev &		operator =(const EvDev &);
  
    static bool		enumerate_device(
  			 const char *			name,
  			 int				fd,
  			 struct device_enumeration &	device);
  
  protected:
    static void		delete_enumeration(
   			 device_enumeration *	data,
  			 std::size_t		count);
  
    static device_enumeration *
  			enumerate(std::size_t & count);
  
  public:
    static char * *	EnumerateButtonDevices();
  
    			EvDev(const char * name);
  			~EvDev();
  };
}
