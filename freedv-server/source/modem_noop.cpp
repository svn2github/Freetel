/// \file modem_noop.cpp
/// The no-op modem, for plain SSB voice and testing.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Modem "no-op", just copies its input to its output.
  /// For plain SSB voice, and for testing.
  class ModemNoOp : public Modem {
  public:

	/// Instantiate the no-op modem.
	///
  			ModemNoOp(const char *);

	virtual		~ModemNoOp();

    /// Return the number of data bytes output in a single modem frame.
    /// The data buffer provided to demodulate16 must be a multiple of
    /// this value. The result is invariant.
    /// \return The number of data bytes necessary to store a modem frame.
    virtual std::size_t
    			bytes_per_frame() const;

    /// Demodulate from audio samples to data.
    /// \param i The array of audio samples to be demodulated, in an array
    /// of signed 16-bit integers.
    /// \param o The demodulated data, in an array of unsigned 8-bit integers.
    /// \param data_length The number of bytes of data that may be demodulated.
    /// \param sample_length On call: The number of audio samples to be
    /// demodulated. On return: The number of audio samples consumed.
    /// \return The number of bytes of data that were actually decoded.
    virtual std::size_t
    			demodulate16(
			 const std::int16_t * i,
			 std::uint8_t * o,
			 std::size_t data_length,
             		 std::size_t * sample_length);

    /// Modulate from data to audio samples.
    /// \param i The data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after modulation, in an array
    /// of signed 16-bit integers.
    /// \param data_length On call: The number of bytes of data to be
    /// modulated. On return: The number of bytes consumed.
    /// \return The number of 16-bit audio samples in the modulated array.
    virtual std::size_t
    			modulate16(
                         const std::uint8_t * i,
			 std::int16_t * o, \
             		 std::size_t *data_length,
			 std::size_t sample_length);

    /// Return the minimum duration of a frame in milliseconds.
    /// \return The minimum duration of a frame in milliseconds.
    virtual int
    			min_frame_duration() const;
  };

  ModemNoOp::ModemNoOp(const char * parameters)
  : Modem("no-op", parameters)
  {
  }

  ModemNoOp::~ModemNoOp()
  {
  }

  std::size_t
  ModemNoOp::bytes_per_frame() const
  {
    return sizeof(std::int16_t);
  }

  std::size_t
  ModemNoOp::demodulate16(
   const std::int16_t * i,
   std::uint8_t * o,
   std::size_t data_length,
   std::size_t * sample_length)
  {
    const std::size_t length = min(data_length / 2, *sample_length);

    memcpy(o, i, length * 2);
    *sample_length = length;
    return length * 2;
  }

  std::size_t
  ModemNoOp::modulate16(
   const std::uint8_t * i,
   std::int16_t * o,
   std::size_t *data_length,
   std::size_t sample_length)
  {
    const std::size_t length = min(*data_length / 2, sample_length);

    memcpy(o, i, length * 2);
    *data_length = length * 2;
    return length;
  }

  int
  ModemNoOp::min_frame_duration() const
  {
    return 1;
  }

  Modem *
  Driver::ModemNoOp(const char * parameter)
  {
    return new ::FreeDV::ModemNoOp(parameter);
  }

  static std::ostream &
  ModemNoOpEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_modem("no-op", Driver::ModemNoOp, ModemNoOpEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
