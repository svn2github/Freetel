/// The No-Op Codec, for testing and plain SSB voice.

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Codec "no-op", just copies its input to its output. For plain SSB voice, and for testing.
  class CodecNoOp : public Codec {
    static const std::size_t	FrameDuration = 40;

  public:

    /// Instantiate the no-op codec.
  			CodecNoOp(const char *);
    			~CodecNoOp();

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

    /// Return the minimum duration of a frame in milliseconds.
    /// \return The minimum duration of a frame in milliseconds.
    virtual int
    			min_frame_duration() const;
  };

  CodecNoOp::CodecNoOp(const char * parameters)
  : Codec("no-op", parameters)
  {
  }

  CodecNoOp::~CodecNoOp()
  {
  }

  std::size_t
  CodecNoOp::decode16(const std::uint8_t * i, std::int16_t * o, std::size_t * data_length, std::size_t sample_length)
  {
    const std::size_t length = min(*data_length / 2, sample_length);
    if ( length < (std::size_t)(((double)SampleRate / 1000.0) * FrameDuration) )
    {
      *data_length = 0;
      return 0;
    }
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

  int
  CodecNoOp::min_frame_duration() const
  {
    return FrameDuration;
  }

  Codec *
  Driver::CodecNoOp(const char * parameter)
  {
    return new ::FreeDV::CodecNoOp(parameter);
  }

  std::ostream &
  Enumerator::CodecNoOp(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_codec("no-op", Driver::CodecNoOp, Enumerator::CodecNoOp);
    return true;
  }
  static const bool initialized = initializer();
}
