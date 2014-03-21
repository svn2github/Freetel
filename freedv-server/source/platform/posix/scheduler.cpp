/// \file platform/posix/scheduler.cpp
/// Implementation of real-time scheduler tuning and memory locking on POSIX.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>

#ifdef _POSIX_PRIORITY_SCHEDULING
#include <sched.h>
#endif

#ifdef _POSIX_MEMLOCK_RANGE
#include <sys/mman.h>
#endif

namespace FreeDV {
  static const char privilege_message_a[] =
  "Warning: Insufficient privilege to set a real-time scheduling priority,\n"
  "or to lock memory.\n"
  "This could cause audio to be interrupted while other programs use the CPU.\n" 
  "To fix: As root, run\n"
  "\n"
  "\tsetcap cap_sys_nice+ep "; /* program_name */
  static const char privilege_message_b[] = "\n\tsetcap cap_ipc_lock+ep ";
  /* program_name */
  static const char privilege_message_c[] = "\n\n"
  "That will allow you to use a real-time scheduling priority and locked\n"
  "memory while running as any user.\n"
  "Alternatively, you can execute this program as root.\n\n";
  
  static const char old_kernel_message[] =
  "This kernel doesn't seem to have real-time facilities or memory locking.\n"
  "If audio is sometimes interrupted, try a newer kernel.\n";

  void
  set_scheduler()
  {
    bool insufficient_privilege = false;
    bool old_kernel = false;

#ifdef _POSIX_PRIORITY_SCHEDULING
    // Put this process on the round-robin realtime scheduling queue at the
    // minimum priority. Other real-time processes (perhaps portaudio) can
    // run with a higher priority than this, but this process will run at
    // a higher priority than all normal processes.
    sched_param	p;
    int		policy = SCHED_RR;

    p.sched_priority = sched_get_priority_min(policy);
    const int result = sched_setscheduler(0, policy, &p);
    if ( result < 0 ) {
      std::cerr << "sched_setscheduler: " << strerror(errno) << std::endl;
      if ( errno == EINVAL )
         old_kernel = true;
      if ( errno == EPERM )
         insufficient_privilege = true;
    }
#endif

#ifdef _POSIX_MEMLOCK_RANGE
    if ( mlockall(MCL_CURRENT|MCL_FUTURE) < 0 ) {
      std::cerr << "mlockall: " << strerror(errno) << std::endl;
      if ( errno == EINVAL )
         old_kernel = true;
      if ( errno == EPERM )
         insufficient_privilege = true;
    }
#endif
    if ( old_kernel )
      std::cerr << old_kernel_message;
    else if ( insufficient_privilege ) {
      std::cerr << privilege_message_a;
      std::cerr << program_name;
      std::cerr << privilege_message_b;
      std::cerr << program_name;
      std::cerr << privilege_message_c;
    }
  }
}