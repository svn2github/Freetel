#include "evdev.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>

namespace FreeDV {
  // Remove extra spaces from string, in place.
  char *
  clean_string(char * string)
  {
    char * s = string;
  
    while ( *s != 0 ) {
      if ( *s == ' ' && s[1] == ' ' ) {
        s++;
        char * end = s;
        while ( *end == ' ' )
          end++;
        memmove(s, end, strlen(end) + 1);
      }
      s++;
    }
  
    const std::size_t length = strlen(string);
  
    if ( length > 1 && string[length - 1] == ' ' )
      string[length - 1] = '\0';
  
    return string;
  }
  
  void
  EvDev::delete_enumeration(
   EvDev::device_enumeration *	data,
   std::size_t			count)
  {
    for ( std::size_t i = 0; i < count; i++ ) {
      if ( data[i].special_file )
        delete data[i].special_file;
      if ( data[i].name )
        delete data[i].name;
    }
    delete data;
  }
  
  struct EvDev::device_enumeration *
  EvDev::enumerate(std::size_t & count)
  {
    struct device_enumeration	devices[100];
  
    count = 0;
  
    for ( std::size_t i = 0; i < (sizeof(devices) / sizeof(*devices)); i++ ) {
      std::ostringstream	str;
  
      str << "/dev/input/event" << i;
  
      const char * const special_file = str.str().c_str();
  
      const int fd = open(special_file, O_RDONLY);
  
      if ( fd >= 0 ) {
        if ( enumerate_device(special_file, fd, devices[count]) )
          count++;
        close(fd);
      }
    }
    device_enumeration * const r = new device_enumeration[count];
    
    memcpy(r, devices, count * sizeof(*r));
  
    return r;
  }
  
  bool
  EvDev::enumerate_device(
   const char *		special_file,
   int			fd,
   device_enumeration &	device)
  {
    char			buf[100];
    std::ostringstream	str;
  
    device.special_file = strdup(special_file);
  
    if ( ioctl(fd, EVIOCGNAME(sizeof(buf)), buf) < 0
    ||   ioctl(fd, EVIOCGBIT(0, sizeof(device.event_types)), device.event_types)
     < 0 )
      return false;
  
    str << clean_string(buf);
  
    if ( ioctl(fd, EVIOCGUNIQ(sizeof(buf)), buf) >= 0
    && buf[0] != '\0'
    && buf[0] != ' ' )
      str << ' ' << clean_string(buf);
  
    if ( ioctl(fd, EVIOCGPHYS(sizeof(buf)), buf) >= 0 )
      str << " at " << clean_string(buf);
  
    device.name = strdup(str.str().c_str());
  
    if ( test_bit(EV_KEY, device.event_types) == 0
    || ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(device.buttons)), device.buttons)
     < 0 )
      memset(device.buttons, 0, sizeof(device.buttons));

    return true;
  }
  
  char * *
  EvDev::EnumerateButtonDevices()
  {
    std::size_t			count = 0;
    std::size_t			length = 0;
    device_enumeration * const	devices = enumerate(count);
  
    for ( std::size_t i = 0; i < count; i++ ) {
      if ( test_bit(EV_KEY, devices[i].event_types) )
        length++;
    }
  
    char * * names = new char * [length + 1];
  
    std::size_t	j = 0;
    for ( std::size_t i = 0; i < count; i++ ) {
      if ( test_bit(EV_KEY, devices[i].event_types) )
        names[j++] = devices[i].name;
        devices[i].name = 0;
    }
    names[j] = 0;
  
    delete_enumeration(devices, count);
    return names;
  }
  
  EvDev::EvDev(const char *)
  {
  }
  
  EvDev::~EvDev()
  {
  }
}
