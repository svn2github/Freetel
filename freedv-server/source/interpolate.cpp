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
    std::size_t			in_done = 0;
    std::size_t			out_done = 0;
    const unsigned int		number_of_channels = 1;
    const soxr_io_spec_t	io_spec = soxr_io_spec(SOXR_INT16_S, SOXR_INT16_S);
    const soxr_quality_spec_t	quality_spec = soxr_quality_spec(
				 SOXR_QQ, SOXR_NO_DITHER);
    const soxr_runtime_spec_t	runtime_spec = soxr_runtime_spec(1);

    /// The "Gibbs effect" may be seen because I don't save data from the
    /// previous block. Fix this by making the resampler a class,
    /// instantiate it to save state, and use the iterative rather than the
    /// one-shot version.
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
