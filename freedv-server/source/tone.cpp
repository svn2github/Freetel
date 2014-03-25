/// \file tone.cpp
/// The tone audio input driver, for testing.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///


#include "drivers.h"
#include <sstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>


namespace FreeDV {
  /// This is a test driver that provides tones.
  ///
  class Tone : public AudioInput {
  private:
    unsigned int	clock;

    struct tone_info {
      float	frequency;
      float	amplitude;
    };

    tone_info	tones[101];

    /// Return file descriptors for poll()
    /// \param size The address of a variable that will be written
    /// with the number of file descriptors in the array.
    /// \return The address of an array of integers containing the
    /// file descriptors.
    virtual int	poll_fds(PollType * array, int space);

    /// Return the amount of audio samples for read. In this case, it always
    /// returns SIZE_MAX.
    std::size_t	ready();

    /// Generate a sine wave.
    float	sine_wave(float frequency, unsigned int step);
  public:
    /// Instantiate the tone driver.
    			Tone(const char * parameters);

    virtual		~Tone();
    
    // Read audio into the "short" type.
    virtual std::size_t	read16(std::int16_t * array, std::size_t length);
  };

  Tone::Tone(const char * _parameters)
  : AudioInput("tone", _parameters), clock(0)
  {
    unsigned int	index = 0;
    unsigned int	input = 0;
    std::stringstream	p(parameters);

    while ( !p.eof() ) {
      float frequency = 0.0;
      float _amplitude = 1.0;

      if ( index >= ((sizeof(tones) / sizeof(*tones)) - 1) ) {
        std::cerr << "Too many tones, number " << index
         << " and subsequent were discarded." << std::endl;
      }
      p >> frequency;
      if ( !p.fail() ) {
        char c;
	p >> c;
        if ( !p.fail() && c == ',' ) {
          p >> _amplitude;
          if ( !p.fail() ) {
	    c = '\0';
            p >> c;
          }
        }
        if ( !p.eof() && !p.fail() && c != ':' ) {
          std::cerr << "tone: bad character \"" << c << "\" in tone string at tone "
           << input << '.' << std::endl;
          break;
        }
      }
      if ( frequency == 0.0 || _amplitude == 0.0 ) {
        input++;
        continue;
      }
      if ( frequency < 0.0 || frequency > (SampleRate / 2) ) {
        std::cerr << "tone: frequency must be in range of 0.0.."
         << SampleRate / 2 << ", is "
         << frequency << '.' << std::endl;
	break;
      }
      if ( _amplitude < 0.0 || _amplitude > 1.0 ) {
        std::cerr << "tone: amplitude must be in range of 0.0..1.0, is "
         << _amplitude << '.' << std::endl;
	break;
      }
      tones[index].frequency = frequency;
      tones[index].amplitude = _amplitude;
      index++;
      input++;
    }
    tones[index].frequency = 0.0;
    tones[index].amplitude = -1.0;
  }

  Tone::~Tone()
  {
  }

  std::size_t
  Tone::read16(std::int16_t * array, std::size_t length)
  {
    const unsigned int array_length = ((sizeof(tones) / sizeof(*tones)) - 1);

    for ( unsigned int i = 0; i < length; i++ ) {
      float value = 0;
      float sumOfAmplitudes = 0;
      for ( unsigned int j = 0; j < array_length && tones[j].amplitude >= 0.0;
       j++ ) {
        value += (sine_wave(tones[j].frequency, clock + i)
         * tones[j].amplitude);
	// FIX: Hoist this out of the inner loop after it's tested.
        sumOfAmplitudes += tones[j].amplitude;
      }
      // FIX: Hoist this out of the inner loop after it's tested.
      // If the sum of amplitudes is greater than 1.0, normalize so that the
      // sum of amplitudes is 1.0.
      if ( sumOfAmplitudes > 1.0 )
        value /= sumOfAmplitudes;
      const std::int16_t v = (std::int16_t)rint(
       value * master_amplitude * ((1 << 15) - 1));
      array[i] = v;
    }
    clock = (clock + length) % SampleRate;

    return length;
  }

  int
  Tone::poll_fds(PollType *, int)
  {
    return 0;
  }

  std::size_t
  Tone::ready()
  {
    return AudioFrameSamples;
  }

  float
  Tone::sine_wave(float frequency, unsigned int step)
  {
    // The number of samples per cycle for the given frequency and sample
    // rate.
    const float	samplesPerCycle = SampleRate / frequency;

    // Add 1/4 turn to the step so that at the Nyquist frequency we
    // render the peaks rather than the zero crossings.
    const float	bias = step + (samplesPerCycle / 4);

    // Angular measure where 1.0 = 360 degrees.
    const float	portionOfCycle = fmod(bias, samplesPerCycle) / samplesPerCycle;

    // Convert to radians.
    const float radians = portionOfCycle * M_PI * 2;

    const float sine = sin(radians);

    return sine;
  }

  AudioInput *
  Driver::Tone(const char * parameter)
  {
    return new ::FreeDV::Tone(parameter);
  }

  static std::ostream &
  ToneEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_input("tone", Driver::Tone, ToneEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
