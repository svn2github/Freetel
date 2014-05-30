/// \file modem_noop.cpp
/// The FDMDV2 modem.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <stdexcept>
#include <string.h>
#include "codec2.h"
#include "codec2_fdmdv.h"

namespace FreeDV {
  /// Modem "FDMDV2".
  class ModemFDMDV2 : public Modem {
  public:
    /// Modem operation mode. These are given as bit-rate + attributes.
    enum Mode {
      M_Invalid = -1,
      M_1400_V0_91_Legacy = 0,	// Broken, and hopefully off-the-air by now.
      M_1400 = 1,		// 1400 bps codec, no FEC.
      M_1600 = 2,		// 1400 bps codec + 300 bps FEC.
      M_2000 = 3,		// 1400 bps codec + 600 bps FEC.
      M_1600_Wide = 4,		// M_1600 with double-width carrier spacing.
    };


  private:
        static const std::size_t	MaxBitsPerFDMDVFrame = 40;

	FDMDV *		f;
	int		carriers;
	Mode		mode;
        int		bits_per_frame;

	// Disable the copy constructor and operator = ().
       			ModemFDMDV2(const ModemFDMDV2 &);
	ModemFDMDV2 &	operator = (const ModemFDMDV2 &);
  public:

	/// Instantiate the FDMDV2 modem.
	///
  			ModemFDMDV2(const char *);

	virtual		~ModemFDMDV2();

    /// Return the number of data bytes output in a single modem frame.
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
    /// \param sample_length The number of audio samples which may be
    /// modulated.
    /// \return The number of 16-bit audio samples in the modulated array.
    virtual std::size_t
    			modulate16(
                         const std::uint8_t * i,
			 std::int16_t * o, \
             		 std::size_t *data_length,
			 std::size_t sample_length);
  };

  ModemFDMDV2::ModemFDMDV2(const char * _parameters)
  : Modem("FDMDV2", _parameters), f(0), carriers(0), mode(M_Invalid),
    bits_per_frame(0)
  {
    switch ( atoi(parameters) ) {
    case 1600:
      mode = M_1600;
      carriers = 16;
      break;
    default:
      throw std::runtime_error(
       "fdmdv2: must specify rate," \
       " the only valid value (for now) is 1600.");
    }
    f = fdmdv_create(carriers);

    if ( f == 0 )
      throw std::runtime_error("fdmdv2: failed to create.");

    bits_per_frame = fdmdv_bits_per_frame(f);
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
    while ( *sample_length > 0 ) {
      COMP    rx_fdm[FDMDV_MAX_SAMPLES_PER_FRAME];
      COMP    rx_fdm_offset[FDMDV_MAX_SAMPLES_PER_FRAME];
      int     rx_bits[MaxBitsPerFDMDVFrame];
      COMP    g_RxFreqOffsetPhaseRect = { 0., 0. };
      COMP    g_RxFreqOffsetFreqRect = { 0., 0. };
      float   g_RxFreqOffsetHz = 0;
      int     reliable_sync_bit = 0;

      for(std::size_t n = 0; n < FDMDV_MAX_SAMPLES_PER_FRAME; n++) {
          rx_fdm[n].real = (float)i[n] / FDMDV_SCALE;
          rx_fdm[n].imag = 0.0;
      }

      int length = min(*sample_length, FDMDV_MAX_SAMPLES_PER_FRAME);

      fdmdv_freq_shift(rx_fdm_offset, rx_fdm, g_RxFreqOffsetHz, &g_RxFreqOffsetPhaseRect, &g_RxFreqOffsetFreqRect, length);
      fdmdv_demod(f, rx_bits, &reliable_sync_bit, rx_fdm_offset, &length);
    }
    return data_length;
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
