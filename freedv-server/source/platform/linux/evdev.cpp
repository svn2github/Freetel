#include "evdev.h"

const struct EvDev::device_enumeration *
EvDev::enumerate(std::size_t &)
{
  return 0;
}

const char * const *
EvDev::EnumerateButtonDevices()
{
  return 0;
}

EvDev::EvDev(const char *)
{
}

EvDev::~EvDev()
{
}
