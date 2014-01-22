#include "drivers.h"
#include <stdexcept>
#include <cstring>

namespace FreeDV {
  FIFO::FIFO(std::size_t size)
  : buffer(new char[size]), buffer_end(buffer + size), in(buffer), out(buffer)
  {
    
  }

  FIFO::~FIFO()
  {
    delete buffer;
  }

  char *
  FIFO::reorder(std::size_t size)
  {
    const std::size_t bytes = in - out;

    if ( bytes > 0 && out > buffer ) {

      // memmove() is specified to handle overlap properly.
      memmove(buffer, out, bytes);
#ifdef DEBUG
      std::cerr << "FIFO reorder with copy." << std::endl;
#endif
    }
    out = buffer;
    in = buffer + bytes;

    if ( size > buffer_end - in )
      throw std::runtime_error("FIFO incoming data overrun.");

    return in;
  }

  void
  FIFO::out_overrun(std::size_t size) const
  {
    throw std::runtime_error("FIFO outgoing data overrun.");
  }
}
