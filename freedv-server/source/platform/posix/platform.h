/// \file platform/posix/platform.h
/// Platform-specific definitions for POSIX systems.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include <poll.h>

namespace FreeDV {
  /// Type used by poll_fds(). For portability to Windows.
  ///
  typedef struct pollfd	PollType;
}
