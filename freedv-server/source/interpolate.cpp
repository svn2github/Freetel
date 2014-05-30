#if 0
#include <soxr.h>
#include "drivers.h"

namespace FreeDV {
  void
  Interpolate16(
   const std::int16_t *	in,
   std::int16_t *	out, 
   std::size_t		in_rate,
   std::size_t		out_rate, 
   std::size_t		in_length,
   std::size_t		out_length)
  {
    std::size_t	out_done = 0;

    /// The "Gibbs effect" may be seen because I don't save data from the
    /// previous block. Fix this by making the resampler a class,
    /// instantiate it to save state, and use the iterative rather than the
    /// one-shot version.
    const soxr_error_t error = soxr_oneshot(
     in_rate,
     out_rate,
     1, // Number of channels.
     in,
     in_length,
     NULL,
     out,
     out_length,
     &out_done,
     NULL,
     NULL,
     NULL);  
  }
};
#endif
