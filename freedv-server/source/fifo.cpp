#include "drivers.h"
#include <stdexcept>
#include <cstring>

namespace FreeDV {
  FIFO::FIFO(std::size_t size)
  : buffer(new uint8_t[size]), buffer_end(buffer + size)
  {
    reset();
  }

  FIFO::~FIFO()
  {
    delete buffer;
  }

  void
  FIFO::out_overrun() const
  {
    throw std::runtime_error("FIFO outgoing data overrun.");
  }

  uint8_t *
  FIFO::reorder(std::size_t size)
  {
    const std::size_t bytes = in - out;

    if ( bytes > 0 && out > buffer ) {

      // memmove() is specified to handle overlap properly.
      memmove(buffer, out, bytes);
#ifdef DEBUG
      std::cerr << "FIFO reorder with copy. Size = " << size
       << ", buffer = " << (void *)buffer << ", buffer_end = "
       << (void *)buffer_end << ", in = " << (void *)in
       << ", out = " << (void *)out
       << std::endl;
#endif
    }
    out = buffer;
    in = buffer + bytes;

    if ( size > (std::size_t)(buffer_end - in) )
      throw std::runtime_error("FIFO incoming data overrun.");

    return in;
  }

  void
  FIFO::reset()
  {
    in = buffer;
    out = buffer;
  }
}
