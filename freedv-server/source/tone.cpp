#include "drivers.h"

// This is a test driver that provides tones.
namespace FreeDV {
  class Tone : public AudioInput {
  public:
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
  : AudioInput(parameters)
  {
  }

  Tone::~Tone()
  {
  }

  float
  Tone::level()
  {
    return 0;
  }

  void
  Tone::level(float value)
  {
  }

  std::size_t
  Tone::read16(int16_t * array, std::size_t length)
  {
    return 0;
  }

#ifndef NO_INITIALIZERS
  static AudioInput *
  creator(const char * parameter)
  {
    return new Tone(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_audio_input("tone", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
