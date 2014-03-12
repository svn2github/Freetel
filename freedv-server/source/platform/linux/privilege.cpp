#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <grp.h>

static const char insufficient_privilege_message[] =
"Warning: The user running the program is not a member of the "
"\"audio\" group.\n"
"This may make it impossible to access audio devices.\n"
"To fix, use one of these solutions:\n"
"\tadd the user to the \"audio\" group.\n"
"\tadd the setgid-audio privilege to the executable file\n"
"with these commands, as root:\n"
"\n"
"\t\tchgrp audio filename\n"
"\t\tchmod 2755 filename\n"
"\n"
"Alternatively, you can execute this program as root.\n\n";

namespace FreeDV {
  void
  check_privileges()
  {
    const int uid = getuid();
    const int euid = geteuid();

    if ( uid == 0 || euid == 0 )
      return;

    const struct group *	audio = getgrnam("audio");
    const int			gid = getgid();
    const int			egid = getgid();

    if ( audio ) {
      gid_t	groups[NGROUPS_MAX];
      int	length = sizeof(groups) / sizeof(*groups);

      if ( gid == audio->gr_gid || egid == audio->gr_gid )
        return;

      if ( getgroups(length, groups) == 0 ) {
        for ( int i = 0; i < length; i++ ) {
          if ( groups[i] == audio->gr_gid )
	    return;
        }
      }
      std::cerr << insufficient_privilege_message << std::endl;
    }
  }
}