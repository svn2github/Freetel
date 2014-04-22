/// \file evdev.cpp
/// Implementation of the evdev PTT driver.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///
#include "evdev.h"
#include <stdexcept>
#include <iostream>
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
#include <assert.h>

namespace FreeDV {
  inline static bool
  bit_is_set(unsigned int bit, const uint8_t * field)
  {
    return ((field[bit / 8] & (1 << (bit % 8))) != 0);
  }

  NORETURN static void
  do_throw(
   const int error,
   const char * name,
   const char * special_file,
   const char * message = 0)
  {
    std::ostringstream str;

    str << "EVDEV ";

    str << '\"' << name << "\" device \"" << special_file
     << "\" set-up error: ";
    if ( message ) {
      str << message;
      if ( error )
        str << ": ";
    }
    if ( error )
      str << strerror(error);

    str << '.';
    throw std::runtime_error(str.str().c_str());
  }

  // Remove redundant elements from string.
  static char *
  clean_string(char * string)
  {
    // Remove trailing space.
    std::size_t	length = strlen(string);
    char *	s = string;
  
    // Remove extra spaces from string, in place.
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
  
    // Remove trailing space.
    length = strlen(string);
    if ( length > 1 && string[length - 1] == ' ' )
      string[length - 1] = '\0';
  
    // If a segment of a string ending with a space or tab is repeated
    // immediately afterward, as with the Manufacturer field and the
    // Product field containing the same initial string, remove the
    // initial part. This gets rid of repeated manufacturer names.
    for ( unsigned int i = 1; i < length; i++ ) {
      if ( (string[i] == ' ' || string[i] == '\t') && string[i+1] != '\0' ) {
        if ( strncmp(string, &string[i + 1], i + 1) == 0 ) {
          memmove(string, &string[i + 1], length - i);
          length = (length - i) - 1;
          i = 0;
        }
      }
    }

    return string;
  }
  
  bool
  EvDev::button_state(unsigned int index)
  {
    uint8_t	buttons[(KEY_MAX + 7) / 8];

    
    if ( ioctl(fd, EVIOCGKEY(sizeof(buttons)), buttons) < 0 )
      do_throw(errno, name, special_file, "ioctl EVIOCGKEY");

    return bit_is_set(index, buttons);
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
    struct device_enumeration	devices[64];
  
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
  
    if ( bit_is_set(EV_KEY, device.event_types) == 0
    || ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(device.buttons)), device.buttons)
     < 0 )
      memset(device.buttons, 0, sizeof(device.buttons));

    return true;
  }
  
  bool
  EvDev::has_button(unsigned int index)
  {
    uint8_t	buttons[(KEY_MAX + 7) / 8];

    if ( ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(buttons)), buttons) < 0 )
      return false;

    return bit_is_set(index, buttons);
  }

  int
  EvDev::poll_fds(PollType * array, int length)
  {
    if ( length >= 0 ) {
      array[0].fd = fd;
      array[0].events = POLLIN|POLLERR|POLLNVAL;
      return 1;
    }
    else
      throw std::invalid_argument("poll_fds() called with zero-size array.");
  }

  std::size_t
  EvDev::read_events(input_event * data, std::size_t count)
  {
    const int result = read(fd, data, count * sizeof(*data));
    if ( result < 0 ) {
      if ( errno == EAGAIN ) {
        return 0;
      }
      else
        do_throw(errno, name, special_file, "read");
    }
    else
      return result / sizeof(*data);
  }

  EvDev::EvDev(const char * _name)
  : fd(-1), name(strdup(_name)), special_file(0)
  {
    static std::size_t		count;
    static device_enumeration *	enumeration = 0;

    if ( (fd = open(name, O_RDWR|O_NONBLOCK) >= 0 ) ) {
      special_file = strdup(name);
      return;
    }

    if ( enumeration == 0 )
      enumeration = enumerate(count);

    std::size_t length = strlen(name);

    for ( std::size_t i = 0; i < count; i++ ) {
      if ( strncmp(name, enumeration[i].name, length) == 0 ) {
        special_file = strdup(enumeration[i].special_file);
        fd = open(special_file, O_RDWR|O_NONBLOCK);

        if ( fd < 0 )
          do_throw(errno, name, special_file, "open");
      }
    } 
  }
  
  EvDev::~EvDev()
  {
    close(fd);
    delete name;
    delete special_file;
  }
}
