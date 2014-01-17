/// The No-Op Codec, for testing and plain SSB voice.

#include "drivers.h"

namespace FreeDV {
  /// Codec "no-op", just copies its input to its output. For plain SSB voice, and for testing.
  class CodecNoOp : public Codec {
  public:

	/// Instantiate the no-op codec.
  		CodecNoOp(const char *);
		~CodecNoOp();

	/// Encode from an array of the signed 16-bit integer type to an
	/// array of the unsigned 8-bit integer type (this is usually
	/// implemented as unsigned char).
	/// \param i The array of audio samples to be encoded, in an array
	/// of signed 16-bit integers.
	/// \param o The encoded data, in an array of unsigned 8-bit integers.
	/// \param length The number of audio samples to be encoded.
	///  This  must be a multiple of frame_size().
	/// \return The number of uint8_t elements in the encoded array.
	virtual std::size_t
		encode16(const int16_t * i, uint8_t * o, \
         	 std::size_t length);

	/// Return the size of uint8_t array necessary to encode the given
	/// number of audio samples. Sample rate is 8K samples/second.
	/// The result is invariant for a given input.
	/// \param sample_count The number of audio samples to encode.
	/// Must be a multiple of frame_size().
	/// \return The size of uint8_t array necessary to encode the given
	/// number of audio samples.
	virtual std::size_t const	
		encoded_buffer_size(std::size_t sample_count) const;

	/// Return the duration of a frame in milliseconds.
	/// \return The duration of a frame in milliseconds.
	virtual int const
			frame_duration() const;

	/// Return the number of audio samples necessary to decode the given
	/// encoded buffer size. Sample rate is 8K samples/second.
	/// \param buffer_size is the size of the encoded buffer. It must
	///  encode a multiple of frame_size() audio samples.
	/// \return The number of audio samples necessary to decode the given
	/// encoded buffer size. The result is invariant for a given input.
	virtual std::size_t const	
		samples_per_buffer(std::size_t buffer_size) const;

	/// Return the number of audio samples expected to create a codec
	/// frame. Samples provided to encode_buffer_size must be a multiple
	/// of this value. Sample rate is 8K samples/second.
	/// The result for a particular input is invariant.
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

  std::size_t
  CodecNoOp::encode16(const int16_t * i, uint8_t * o, std::size_t length)
  {
    return 0;
  }

  std::size_t const	
  CodecNoOp::encoded_buffer_size(std::size_t sample_count) const
  {
    return 0;
  }

  int const
  CodecNoOp::frame_duration() const
  {
    return 0;
  }

  std::size_t const	
  CodecNoOp::samples_per_buffer(std::size_t buffer_size) const
  {
    return 0;
  }
 

  std::size_t const
  CodecNoOp::samples_per_frame() const
  {
    return 0;
  }

#ifndef NO_INITIALIZERS
  static Codec *
  creator(const char * parameter)
  {
    return new CodecNoOp(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_codec("no-op", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
