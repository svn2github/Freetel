#include "drivers.h"

#if 0
namespace FreeDV {
  Interpolator16::Interpolator16(std::size_t _in_rate, std::size_t _out_rate)
  : in_rate(_in_rate), out_rate(_out_rate), number_of_channels(1),
    io_spec(soxr_io_spec(SOXR_INT16_S, SOXR_INT16_S),
    quality_spec(SOXR_QQ, SOXR_NO_DITHER),
    runtime_spec(soxr_runtime_spec(1))
  {
  }

  Interpolator16::~Interpolator16()
  {
  };

  void
  Interpolator16::process(
   const std::int16_t *	in,
   std::int16_t *	out, 
   std::size_t		in_length,
   std::size_t		out_length)
  {
    std::size_t	in_done = 0;
    std::size_t	out_done = 0;

    // FIX: Use the state-saving version instead of the one-shot version,
    // to prevent ringing. Is there a pipeline delay?

    const soxr_error_t error = soxr_oneshot(
     in_rate,
     out_rate,
     number_of_channels, // Number of channels.
     in,
     in_length,
     &in_done,
     out,
     out_length,
     &out_done,
     io_spec,
     quality_spec,
     runtime_spec);  
  }
};
#endif
