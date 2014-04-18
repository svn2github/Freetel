/// \file platform/posix/poll.cpp
/// Implementation of FreeDV::IODevice::poll() on POSIX.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

int
FreeDV::IODevice::poll(PollType * array, int length, int timeout)
{
  return ::poll(array, length, timeout);
}
