/// The No-Op Codec, for testing and plain SSB voice.

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Codec "no-op", just copies its input to its output. For plain SSB voice, and for testing.
  class CodecNoOp : public Codec {
  public:

    /// Instantiate the no-op codec.
  			CodecNoOp(const char *);
    			~CodecNoOp();

    /// Return the number of data bytes that store a single codec frame.
    /// Data Bytes provided to decode16 and encode16 must be a multiple
    /// of this value. The result is invariant.
    /// \return The number of data bytes necessary to store a codec frame.
    virtual std::size_t const
    			bytes_per_frame() const;

    /// Decode from data bytes to audio samples.
    /// \param i The encoded data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after decoding, in an array
    /// of signed 16-bit integers.
    /// \param data_length When called: The number of bytes of data that are
    /// available to be decoded. On return: the number of bytes of data
    /// that were consumed.
    /// \param sample_length The number of audio samples that may be decoded.
    /// \return The number of audio samples that were actually decoded.
    virtual std::size_t
    			decode16(const std::uint8_t * i,
			 std::int16_t * o,
             		 std::size_t * data_length,
			 std::size_t sample_length);

    /// Encode from audio samples to data bytes.
    /// \param i The array of audio samples to be encoded, in an array
    /// of signed 16-bit integers.
    /// \param o The encoded data, in an array of unsigned 8-bit integers.
    /// \param length The number of audio samples to be encoded.
    /// \return The number of std::uint8_t elements in the encoded array.
    virtual std::size_t
    			encode16(const std::int16_t * i, std::uint8_t * o, \
             		 std::size_t length);

    /// Return the duration of a frame in milliseconds.
    /// \return The duration of a frame in milliseconds.
    virtual int const
    			frame_duration() const;

    /// Return the number of audio samples expected to create a codec
    /// frame at SampleRate. Samples provided to encode16 and decode16
    /// must be a multiple of this value. The result is invariant for
    /// a given SampleRate.
    /// \return The number of audio samples expected to create a codec
    /// frame.
    virtual std::size_t const
    			samples_per_frame() const;
  };

  CodecNoOp::CodecNoOp(const char * parameters)
  : Codec("no-op", parameters)
  {
  }

  CodecNoOp::~CodecNoOp()
  {
  }

  std::size_t const
  CodecNoOp::bytes_per_frame() const
  {
    return sizeof(std::int16_t);
  }

  std::size_t
  CodecNoOp::decode16(const std::uint8_t * i, std::int16_t * o, std::size_t * data_length, std::size_t sample_length)
  {
    const std::size_t length = std::min(*data_length / 2, sample_length);
    memcpy(o, i, length * 2);
    *data_length = length * 2;
    return length;
  }

  std::size_t
  CodecNoOp::encode16(const std::int16_t * i, std::uint8_t * o, std::size_t length)
  {
    memcpy(o, i, length);
    return length;
  }

  int const
  CodecNoOp::frame_duration() const
  {
    return 1;
  }

  std::size_t const
  CodecNoOp::samples_per_frame() const
  {
    return 1;
  }

  Codec *
  Driver::CodecNoOp(const char * parameter)
  {
    return new ::FreeDV::CodecNoOp(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    driver_manager()->register_codec("no-op", Driver::CodecNoOp);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
