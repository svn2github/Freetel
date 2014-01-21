#include <unistd.h>
#ifdef _POSIX_PRIORITY_SCHEDULING
#include <errno.h>
#include <sched.h>
#include <cstring>
#include <iostream>
#endif
#include "drivers.h"

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
      if ( errno == EPERM ) {
        std::cerr << "Insufficient privilege to set real-time priority. "
	 "Try runing as root." << std::endl;
      }
      else if ( errno == EINVAL ) {
        std::cerr << "This kernel doesn't seem to have real-time facilities. "
         "If audio is sometimes interrupted, try a newer kernel." << std::endl;
      }
      else {
        std::cerr << "sched_setscheduler: " << strerror(errno) << std::endl;
      }
    }
  }
#endif
}
