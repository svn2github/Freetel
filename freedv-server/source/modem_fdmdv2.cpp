/// \file modem_noop.cpp
/// The FDMDV2 modem.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>

namespace FreeDV {
  /// Modem "FDMDV2".
  class ModemFDMDV2 : public Modem {
  public:

	/// Instantiate the FDMDV2 modem.
	///
  			ModemFDMDV2(const char *);

	virtual		~ModemFDMDV2();

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
  };

  ModemFDMDV2::ModemFDMDV2(const char * _parameters)
  : Modem("FDMDV2", _parameters)
  {
  }

  ModemFDMDV2::~ModemFDMDV2()
  {
  }

  std::size_t
  ModemFDMDV2::bytes_per_frame() const
  {
    return 0;
  }

  std::size_t
  ModemFDMDV2::demodulate16(
   const std::int16_t * i,
   std::uint8_t * o,
   std::size_t data_length,
   std::size_t * sample_length)
  {
    return 0;
  }

  std::size_t
  ModemFDMDV2::modulate16(
   const std::uint8_t * i,
   std::int16_t * o,
   std::size_t *data_length,
   std::size_t sample_length)
  {
    return 0;
  }

  Modem *
  Driver::ModemFDMDV2(const char * parameter)
  {
    return new ::FreeDV::ModemFDMDV2(parameter);
  }

  static std::ostream &
  ModemFDMDV2Enumerator(std::ostream & stream)
  {
    stream << "fdmdv2:" << std::endl;
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_modem("fdmdv2", Driver::ModemFDMDV2, ModemFDMDV2Enumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
