#include "drivers.h"

// Audio output "sink", discards the audio, for testing.

namespace FreeDV {
  class AudioSink : public AudioOutput {
  public:

  		AudioSink(const char *);
		~AudioSink();

	// Get the current audio level, normalized to the range of 0.0 to 1.0.
	float	level();

	// Set the current audio level within the range of 0.0 to 1.0.
	float	level(float value);

        // Write audio into the "short" type.
	int	write_short(short * array, int length);
  };

  AudioSink::AudioSink(const char *)
  {
  }

  AudioSink::~AudioSink()
  {
  }

  float
  AudioSink::level()
  {
    return 0;
  }

  float
  AudioSink::level(float value)
  {
    return value;
  }

  // Write audio into the "short" type.
  int
  AudioSink::write_short(short * array, int length)
  {
    return 0;
  }

#ifndef NO_INITIALIZERS
  static AudioOutput *
  creator(const char * parameter)
  {
    return new AudioSink(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_audio_output("sink", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
