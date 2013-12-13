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
    virtual float	level(float value);
    
    // Read audio into the "short" type.
    virtual int	read_short(short * array, int length);
  };

  Tone::Tone(const char * parameter)
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

  float
  Tone::level(float value)
  {
    return value;
  }

  int
  Tone::read_short(short * array, int length)
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
