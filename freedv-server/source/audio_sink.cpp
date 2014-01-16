/// The AudioSink driver, called as "sink", discards the audio, for testing.

#include "drivers.h"

namespace FreeDV {
  /// Audio output "sink", discards the audio, for testing.
  class AudioSink : public AudioOutput {
  public:

	/// Instantiate the audio sink.
  		AudioSink(const char * parameters);
		~AudioSink();

	/// Get the current audio level, normalized to the range of 0.0 to 1.0.
	virtual float
		level();

	/// Set the current audio level within the range of 0.0 to 1.0.
	virtual void
		level(float value);

        /// Write audio into the "short" type.
	virtual std::size_t
		write16(const int16_t * array, std::size_t length);
  };

  AudioSink::AudioSink(const char * parameters)
  : AudioOutput(parameters)
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

  void
  AudioSink::level(float value)
  {
  }

  // Write audio into the "short" type.
  std::size_t
  AudioSink::write16(const int16_t * array, std::size_t length)
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
