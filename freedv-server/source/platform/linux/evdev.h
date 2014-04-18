#include <cstdint>
#include <linux/input.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

class EvDev {
private:

			EvDev(const EvDev &);
  EvDev &		operator =(const EvDev &);

protected:
  struct device_enumeration {
    const char *	device;
    const char *	name;
    uint8_t		event_types[(EV_MAX + 7) / 8];
  };

  static const device_enumeration *
			enumerate(std::size_t & size);

public:
  static const char * const *
			EnumerateButtonDevices();

  			EvDev(const char * name);
			~EvDev();
};
