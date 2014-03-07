/// The No-Op Framer, for digital voice that is not framed in a protocol.
/// FreeDV would be the obvious example.

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Framer "no-op", just copies its input to its output.
  class FramerNoOp : public ::FreeDV::Framer {
  public:
    /// Create a framer instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
      			FramerNoOp(const char * parameters);

    /// Destroy a framer instance.
    virtual		~FramerNoOp();

    /// Return the maximum number of data bytes expected to store a wrapped
    /// protocol frame. The result is invariant for a particular configuration
    /// which may include such things as length of addresses and protocol
    /// options.
    /// \return The maximum number of data bytes expected to store a wrapped
    /// protocol frame.
    /// frame.
    virtual std::size_t const
    			max_wrapped_bytes_per_frame() const;

    /// Return the maximum number of data bytes expected to store the unwrapped
    /// codec data. The result is invariant for a particular configuration.
    /// \return The maximum number of data bytes expected to store the unwrapped
    /// codec data.
    /// frame.
    virtual std::size_t const
    			max_unwrapped_bytes_per_frame() const;

    /// Return the minimum number of data bytes expected to store the unwrapped
    /// codec data. The result is invariant for a particular configuration.
    /// \return The minimum number of data bytes expected to store the unwrapped
    /// codec data.
    /// frame.
    virtual std::size_t const
    			min_unwrapped_bytes_per_frame() const;

    /// Return the minimum number of data bytes expected to store a wrapped
    /// protocol frame. The result is invariant for a particular configuration
    /// which may include such things as length of addresses and protocol
    /// options.
    /// \return The minimum number of data bytes expected to store a wrapped
    /// protocol frame.
    /// frame.
    virtual std::size_t const
    			min_wrapped_bytes_per_frame() const;

    /// Decode from modem data to codec frames, removing the wrapping protocol.
    /// \param i The encoded data, in an array of unsigned 8-bit integers.
    /// \param o The array of codec data after decoding, in an array
    /// of unsigned 8-bit integers.
    /// \param input_length When called: The number of bytes of data that are
    /// available to be unwrap. On return: the number of bytes of data
    /// that were consumed.
    /// \param output_length The number of codec data bytes that can be
    /// unwrapped.
    /// \return The number of data bytes that were actually decoded.
    virtual std::size_t
    			unwrap(const std::uint8_t * i,
			 std::uint8_t * o,
             		 std::size_t * input_length,
			 std::size_t output_length);

    /// Wrap codec data bytes in a protocol for transmission through the modem.
    /// \param i The array of data bytes to be encoded, in an array
    /// of unsigned 8-bit integers.
    /// \param o The encoded data, in an array of unsigned 8-bit integers.
    /// \param input_length The number of data bytes to be wrapped.
    /// \param output_length The number of data bytes available to store the
    /// wrapped data.
    /// \return The number of std::uint8_t elements in the wrapped array.
    virtual std::size_t
    			wrap(
			 const std::uint8_t * i,
			 std::uint8_t * o,
             		 std::size_t * input_length,
			 std::size_t output_length);
  };

  FramerNoOp::FramerNoOp(const char * parameters)
  : Framer("no-op", parameters)
  {
  }

  FramerNoOp::~FramerNoOp()
  {
  }

  std::size_t const
  FramerNoOp::max_wrapped_bytes_per_frame() const
  {
    return 1;
  }

  std::size_t const
  FramerNoOp::max_unwrapped_bytes_per_frame() const
  {
    return 1;
  }

  std::size_t const
  FramerNoOp::min_unwrapped_bytes_per_frame() const
  {
    return 1;
  }

  std::size_t const
  FramerNoOp::min_wrapped_bytes_per_frame() const
  {
    return 1;
  }

  std::size_t
  FramerNoOp::unwrap(const std::uint8_t * i,
   std::uint8_t * o,
   std::size_t * input_length,
   std::size_t output_length)
  {
    const std::size_t length = std::min(*input_length, output_length);
    memcpy(o, i, length);
    *input_length = length;
    return length;
  }

  std::size_t
  FramerNoOp::wrap(
   const std::uint8_t * i,
   std::uint8_t * o,
   std::size_t * input_length,
   std::size_t output_length)
  {
    const std::size_t length = std::min(*input_length, output_length);
    memcpy(o, i, length);
    *input_length = length;
    return length;
  }

  Framer *
  Driver::FramerNoOp(const char * parameter)
  {
    return new ::FreeDV::FramerNoOp(parameter);
  }

  static bool
  initializer()
  {
    driver_manager()->register_framer("no-op", Driver::FramerNoOp);
    return true;
  }
  static const bool initialized = initializer();
}
