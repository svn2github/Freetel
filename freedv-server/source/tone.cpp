#include "drivers.h"
#include <iostream>

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
    virtual size_t	read_short(short * array, size_t length);
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

  size_t
  Tone::read_short(short * array, size_t length)
  {
    return 0;
  }

  static AudioInput *
  creator(const char * parameter)
  {
    return new Tone(parameter);
  }

  static bool
  initializer()
  {
    driver_manager.register_audio_input("tone", creator);
    return true;
  }
  static const bool initialized = initializer();
};
