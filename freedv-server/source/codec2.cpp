/// \file codec_codec2.cpp
/// Codec2, for FreeDV, "HT of the Future", and other modes using Codec2.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Codec2.
  class Codec2 : public Codec {
  public:

    /// Instantiate the no-op codec.
  			Codec2(const char *);
    			~Codec2();

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

  Codec2::Codec2(const char * _parameters)
  : Codec("no-op", _parameters)
  {
  }

  Codec2::~Codec2()
  {
  }

  std::size_t
  Codec2::decode16(const std::uint8_t * i, std::int16_t * o, std::size_t * data_length, std::size_t sample_length)
  {
    return 0;
  }

  std::size_t
  Codec2::encode16(
   const std::int16_t * i,
   std::uint8_t * o,
   std::size_t data_length,
   std::size_t *sample_length)
  {
    return 0;
  }

  std::size_t
  Codec2::samples_per_frame() const
  {
    return 0;
  }

  Codec *
  Driver::Codec2(const char * parameter)
  {
    return new ::FreeDV::Codec2(parameter);
  }

  static std::ostream &
  Codec2Enumerator(std::ostream & stream)
  {
    stream << "codec2:" << std::endl;
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_codec("codec2", Driver::Codec2, Codec2Enumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
