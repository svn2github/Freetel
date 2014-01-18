/// The tone audio input driver, for testing.

#include "drivers.h"
#include <stdexcept>

namespace FreeDV {
  /// This is a test driver that provides tones.
  class Tone : public AudioInput {
  private:
    float		volume;

  public:
    /// Instantiate the tone driver.
    			Tone(const char * parameter);
    virtual		~Tone();
    
    // Get the current audio level, normalized to the range of 0.0 to 1.0.
    virtual float	level();
    
    // Set the current audio level within the range of 0.0 to 1.0.
    virtual void	level(float value);
    
    // Read audio into the "short" type.
    virtual std::size_t	read16(int16_t * array, std::size_t length);
  };

  Tone::Tone(const char * parameters)
  : AudioInput("tone", parameters), volume(1.0)
  {
  }

  Tone::~Tone()
  {
  }

  float
  Tone::level()
  {
    return volume;
  }

  void
  Tone::level(float value)
  {
    if ( value < 0.0 || value > 1.0 )
      throw std::runtime_error(
       "Level set to value outside of the range 0.0..1.0");
    volume = value;
  }

  std::size_t
  Tone::read16(int16_t * array, std::size_t length)
  {
    return 0;
  }

  AudioInput *
  Driver::Tone(const char * parameter)
  {
    return new ::FreeDV::Tone(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_audio_input("tone", Driver::Tone);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
