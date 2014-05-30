/// \file codec2.cpp
/// Codec2, for FreeDV, "HT of the Future", and other modes using Codec2.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <stdexcept>
#include <stdlib.h>
extern "C" {
  #include <codec2.h>
}

namespace FreeDV {
  /// Codec2.
  class Codec2 : public Codec {
  private:
    // Override copy constructor and operator = ().
    			Codec2(const Codec2 &);
    Codec2 &		operator = (const Codec2 &);

    struct CODEC2 *	c;
    std::size_t		samples_per;
    std::size_t		bytes_per;

  public:

    /// Instantiate the codec.
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
    virtual std::size_t	samples_per_frame() const;
  };

  Codec2::Codec2(const char * _parameters)
  : Codec("codec2", _parameters), c(0), samples_per(0), bytes_per(0)
  {
    int	mode = CODEC2_MODE_1600;

    switch ( atoi(parameters) ) {
    case 0:
    default:
      throw std::runtime_error(
       "codec2: must specify rate," \
       " valid values are 1200,1300,1400,1600,2400.");
       break; // NOTREACHED
    case 1200:
      mode = CODEC2_MODE_1200;
      break;
    case 1300:
      mode = CODEC2_MODE_1300;
      break;
    case 1400:
      mode = CODEC2_MODE_1400;
      break;
    case 1600:
      mode = CODEC2_MODE_1600;
      break;
    case 2400:
      mode = CODEC2_MODE_2400;
      break;
    }
    c = codec2_create(mode);

    if ( c == 0 )
      throw std::runtime_error("codec2: failed to create.");

    samples_per = codec2_samples_per_frame(c);
    bytes_per = (codec2_bits_per_frame(c) + 7) / 8;
  }

  Codec2::~Codec2()
  {
    codec2_destroy(c);
  }

  std::size_t
  Codec2::decode16(
   const std::uint8_t *	i,
   std::int16_t *	o,
   std::size_t *	data_length,
   std::size_t		sample_length)
  {
    std::size_t	bytes_read = 0;
    std::size_t	samples_read = 0;

    while ( *data_length >= bytes_per && sample_length >= samples_per ) {

      codec2_decode(c, o, i);

      i += bytes_per;
      bytes_read += bytes_per;
      *data_length -= bytes_per;

      o += samples_per;
      samples_read += samples_per;
      sample_length -= samples_per;
    }
    *data_length = bytes_read;
    return samples_read;
  }

  std::size_t
  Codec2::encode16(
   const std::int16_t *	i,
   std::uint8_t *	o,
   std::size_t		data_length,
   std::size_t *	sample_length)
  {
    std::size_t	bytes_read = 0;
    std::size_t	samples_read = 0;

    while ( data_length >= bytes_per && *sample_length >= samples_per ) {
      // FIX: Const cast away. Remove if we can get const correctness in
      // libcodec2.
      codec2_encode(c, o, (std::int16_t *)i);

      o += bytes_per;
      bytes_read += bytes_per;
      data_length -= bytes_per;

      i += samples_per;
      samples_read += samples_per;
      *sample_length -= samples_per;
    }
    *sample_length = samples_read;
    return bytes_read;
  }

  std::size_t
  Codec2::samples_per_frame() const
  {
    return samples_per;
  }

  Codec *
  Driver::Codec2(const char * parameter)
  {
    return new ::FreeDV::Codec2(parameter);
  }

  static std::ostream &
  Codec2Enumerator(std::ostream & stream)
  {
    stream << "\"codec2:1600\" (1200,1300,1400,1600,2400,3200)" << std::endl;
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
