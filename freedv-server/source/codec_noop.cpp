/// \file codec_noop.cpp
/// The No-Op Codec, for testing and plain SSB voice.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Codec "no-op", just copies its input to its output.a
  /// For plain SSB voice, and for testing.
  class CodecNoOp : public Codec {
    static const std::size_t	FrameDuration = 40;
    static const std::size_t	FrameSamples = SamplesPerMillisecond
				 * FrameDuration;

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
    /// \param data_length The number of 8-bit data to be encoded.
    /// \param sample_length On call: The number of 16-bit audio samples to
    /// be encoded. On return: the number of samples that were consumed.
    /// \return The number of 8-bit data elements in the encoded array.
    virtual std::size_t
    			encode16(
			 const std::int16_t * i,
			 std::uint8_t * o, \
			 std::size_t data_length,
             		 std::size_t *sample_length);

    /// \return The number of samples required to encode a frame in the
    /// currently-selected mode.
    virtual std::size_t
    samples_per_frame() const;
  };

  CodecNoOp::CodecNoOp(const char * _parameters)
  : Codec("no-op", _parameters)
  {
  }

  CodecNoOp::~CodecNoOp()
  {
  }

  std::size_t
  CodecNoOp::decode16(const std::uint8_t * i, std::int16_t * o, std::size_t * data_length, std::size_t sample_length)
  {
    std::size_t length = min(*data_length / 2, sample_length);
    length -= length % FrameSamples;

    if ( length < FrameSamples ) {
      *data_length = 0;
      return 0;
    }
    memcpy(o, i, length * 2);
    *data_length = length * 2;
    return length;
  }

  std::size_t
  CodecNoOp::encode16(
   const std::int16_t * i,
   std::uint8_t * o,
   std::size_t data_length,
   std::size_t *sample_length)
  {
    std::size_t length = min(data_length / 2, *sample_length);
    length -= length % FrameSamples;

    if ( length < FrameSamples ) {
      *sample_length = 0;
      return 0;
    }
    memcpy(o, i, length * 2);
    *sample_length = length;
    return length * 2;
  }

  std::size_t
  CodecNoOp::samples_per_frame() const
  {
    return FrameSamples;
  }

  Codec *
  Driver::CodecNoOp(const char * parameter)
  {
    return new ::FreeDV::CodecNoOp(parameter);
  }

  static std::ostream &
  CodecNoOpEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_codec("no-op", Driver::CodecNoOp, CodecNoOpEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
