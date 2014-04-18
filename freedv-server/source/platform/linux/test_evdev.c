#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

process_device(const char * device_name, int fd)
{
  char name[256];
  char address[256];
  uint8_t event_bits[(EV_MAX + 7) / 8];
  int length;

  if ( ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0
  ||   ioctl(fd, EVIOCGPHYS(sizeof(address)), address) < 0
  ||   ioctl(fd, EVIOCGBIT(0, sizeof(event_bits)), event_bits) < 0 ) {
      perror(device_name);
  }
  else {
    char unique[256];
    int got_unique = ioctl(fd, EVIOCGUNIQ(sizeof(unique)), unique) >= 0;

    char * s = name;
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
    length = strlen(name);

    if ( length > 1 && name[length - 1] == ' ' )
      name[length - 1] = '\0';

    if ( test_bit(EV_KEY, event_bits) ) {
      printf("\"evdev:%s", name);
      if ( got_unique && unique[0] != '\0' && unique[0] != ' ' )
      printf(" %s", unique);
      printf(" at %s\"\n", address);
    }
  }
}

int
main(int argc, char * * argv)
{
  int i;
  for ( i = 0; i < 1024; i++ ) {
    char	name[100];

    sprintf(name, "/dev/input/event%d", i);

    int	fd = open(name, O_RDONLY);
    if ( fd >= 0 ) {
      process_device(name, fd);
      close(fd);
    }
  }
  return 0;
}
