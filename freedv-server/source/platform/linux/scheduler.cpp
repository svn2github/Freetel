#include <unistd.h>
#ifdef _POSIX_PRIORITY_SCHEDULING
#include <errno.h>
#include <sched.h>
#include <cstring>
#include <iostream>
#endif
#include "drivers.h"

static const char insufficient_privilege_message[] =
"Warning: Insufficient privilege to set a real-time scheduling priority.\n"
"This could cause audio to be interrupted while other programs use the CPU.\n" 
"To fix: As root, run\n"
"\n"
"\tsetcap cap_sys_nice+ep filename\n"
"\n"
"where filename is the executable file for this program. That will\n"
"allow you to use a real-time scheduling priority while running as any user.\n"
"Alternatively, you can execute this program as root.\n\n";

static const char old_kernel_message[] =
"This kernel doesn't seem to have real-time facilities.\n"
"If audio is sometimes interrupted, try a newer kernel.\n\n";

namespace FreeDV {
  void
  set_scheduler()
  {
#ifdef _POSIX_PRIORITY_SCHEDULING
    sched_param	p;
    int		policy = SCHED_RR;

    p.sched_priority = sched_get_priority_min(policy);
    const int result = sched_setscheduler(0, policy, &p);
    if ( result < 0 ) {
      if ( errno == EPERM )
         std::cerr << insufficient_privilege_message << std::flush;
      else if ( errno == EINVAL )
         std::cerr << old_kernel_message << std::flush;
      else {
        std::cerr << "sched_setscheduler: " << strerror(errno) << std::endl;
      }
    }
  }
#endif
}
