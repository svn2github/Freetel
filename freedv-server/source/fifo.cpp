/// \file fifo.cpp
/// Implementation of the FIFO class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <stdexcept>
#include <cstring>

namespace FreeDV {
  FIFO::FIFO(std::size_t size)
  : buffer(new uint8_t[size]), buffer_end(buffer + size), in(buffer), out(buffer)
  {
    reset();
  }

  FIFO::~FIFO()
  {
    delete buffer;
  }

  const uint8_t *
  FIFO::get(std::size_t length) {
      assert(length % 2 == 0);
      if ( length > (std::size_t)(in - out) )
          get_overrun();
      return out;
  }

  void
  FIFO::get_done(std::size_t length) {
      assert(length % 2 == 0);
      out += length;
      assert(out >= buffer && out <= buffer_end);
      if ( out == in )
          out = in = buffer;
  }

  void
  FIFO::get_overrun() const
  {
    throw std::runtime_error("FIFO outgoing data overrun.");
  }

  uint8_t *
  FIFO::put(std::size_t length) {
    assert(length % 2 == 0);

    if ( (in + length) > buffer_end )
      return reorder(length);
    else
      return in;
  }

  void
  FIFO::put_done(std::size_t length) {
    assert(length % 2 == 0);
    in += length;
    assert(in >= buffer && in <= buffer_end);
  }

  uint8_t *
  FIFO::reorder(std::size_t size)
  {
    const std::size_t bytes = in - out;

    if ( bytes > 0 && out > buffer ) {

      // memmove() is specified to handle overlap properly.
      memmove(buffer, out, bytes);
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
