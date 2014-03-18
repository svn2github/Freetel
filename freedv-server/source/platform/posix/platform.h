#include <poll.h>

namespace FreeDV {
  /// Type used by poll_fds(). For portability to Windows.
  typedef struct pollfd	PollType;
}
