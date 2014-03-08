/// The tone audio input driver, for testing.

#include "drivers.h"
#include <sstream>
#define _USE_MATH_DEFINES
#include <cmath>

namespace FreeDV {
  /// This is a test driver that provides tones.
  class Tone : public AudioInput {
  private:
    unsigned int	clock;

    struct tone_info {
      float	frequency;
      float	amplitude;
    };

    tone_info	tones[101];

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

  Tone::Tone(const char * parameters)
  : AudioInput("tone", parameters), clock(0)
  {
    unsigned int	index = 0;
    unsigned int	input = 0;
    std::stringstream	p(parameters);

    while ( !p.eof() ) {
      float frequency = 0.0;
      float amplitude = 1.0;

      if ( index >= ((sizeof(tones) / sizeof(*tones)) - 1) ) {
        std::cerr << "Too many tones, number " << index
         << " and subsequent were discarded." << std::endl;
      }
      p >> frequency;
      if ( !p.fail() ) {
        char c;
	p >> c;
        if ( !p.fail() && c == ',' ) {
          p >> amplitude;
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
      if ( frequency == 0.0 || amplitude == 0.0 ) {
        input++;
        continue;
      }
      if ( frequency < 0.0 || frequency > SampleRate ) {
        std::cerr << "tone: frequency must be in range of 0.0.."
         << SampleRate / 2 << ", is "
         << frequency << '.' << std::endl;
	break;
      }
      if ( amplitude < 0.0 || amplitude > 1.0 ) {
        std::cerr << "tone: amplitude must be in range of 0.0..1.0, is "
         << frequency << '.' << std::endl;
	break;
      }
      tones[index].frequency = frequency;
      tones[index].amplitude = amplitude;
      index++;
      input++;
    }
    tones[index].frequency = 0.0;
    tones[index].amplitude = 0.0;
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
      for ( unsigned int j = 0; j < array_length && tones[j].amplitude > 0.0;
       j++ ) {
        value += (sine_wave(tones[j].frequency, i + clock)
         * tones[j].amplitude);
	// FIX: Hoist this out of the inner loop after it's tested.
        sumOfAmplitudes += tones[j].amplitude;
      }
      // If the sum of amplitudes is greater than 1.0, normalize so that the
      // sum of amplitudes is 1.0.
      if ( sumOfAmplitudes > 1.0 )
        value /= sumOfAmplitudes;
      array[i] = (std::int16_t)rint(value * master_amplitude * ((1 << 15) - 1));
    }
    clock = (clock + length) % SampleRate;
  }

  std::size_t
  Tone::ready()
  {
    return SIZE_MAX;
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

  std::ostream &
  Enumerator::Tone(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_input("tone", Driver::Tone, Enumerator::Tone);
    return true;
  }
  static const bool initialized = initializer();
}
