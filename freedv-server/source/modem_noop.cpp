/// The no-op modem, for plain SSB voice and testing.

#include "drivers.h"

namespace FreeDV {
  /// Modem "no-op", just copies its input to its output.
  /// For plain SSB voice, and for testing.
  class ModemNoOp : public Modem {
  public:

	/// Instantiate the no-op modem.
  			ModemNoOp(const char *);

	virtual		~ModemNoOp();

    /// Return the number of data bytes output in a single modem frame.
    /// The data buffer provided to demodulate16 must be a multiple of
    /// this value. The result is invariant.
    /// \return The number of data bytes necessary to store a modem frame.
    virtual std::size_t const
    			bytes_per_frame() const;

    /// Demodulate from audio samples to data.
    /// \param i The array of audio samples to be demodulated, in an array
    /// of signed 16-bit integers.
    /// \param o The demodulated data, in an array of unsigned 8-bit integers.
    /// \param length The number of audio samples to be demodulated.
    /// \return The number of std::uint8_t elements in the demodulated array.
    virtual std::size_t
    			demodulate16(const std::int16_t * i, std::uint8_t * o, \
             		 std::size_t length);

    /// Modulate from data to audio samples.
    /// \param i The data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after modulation, in an array
    /// of signed 16-bit integers.
    /// \param length The number of bytes of data to be modulated.
    /// \return The number of std::int16_t elements in the modulated array.
    virtual std::size_t
    			modulate16(const std::uint8_t * i, std::int16_t * o, \
             		 std::size_t length);

    /// Return the duration of a frame in milliseconds.
    /// \return The duration of a frame in milliseconds.
    virtual int const
    			frame_duration() const;

    /// Return the number of audio samples expected to create a codec
    /// frame at SampleRate. Samples provided to modulate16 and
    /// demodulate16 must be a multiple of this value. The result is
    /// invariant for a given SampleRate.
    /// \return The number of audio samples expected to create a codec
    /// frame.
    virtual std::size_t const
    			samples_per_frame() const;
  };

  ModemNoOp::ModemNoOp(const char * parameters)
  : Modem("no-op", parameters)
  {
  }

  ModemNoOp::~ModemNoOp()
  {
  }

  std::size_t const
  ModemNoOp::bytes_per_frame() const
  {
    return sizeof(std::int16_t);
  }

  std::size_t
  ModemNoOp::demodulate16(const std::int16_t * i, std::uint8_t * o, std::size_t length)
  {
    return length;
  }

  std::size_t
  ModemNoOp::modulate16(const std::uint8_t * i, std::int16_t * o, std::size_t length)
  {
    return length;
  }

  int const
  ModemNoOp::frame_duration() const
  {
    return 1;
  }

  std::size_t const
  ModemNoOp::samples_per_frame() const
  {
    return 1;
  }

  Modem *
  Driver::ModemNoOp(const char * parameter)
  {
    return new ::FreeDV::ModemNoOp(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_modem("no-op", Driver::ModemNoOp);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
