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
		amplitude();

	/// Set the current audio level within the range of 0.0 to 1.0.
	virtual void
		amplitude(float value);

        /// Write audio into the "short" type.
	virtual std::size_t
		write16(const int16_t * array, std::size_t length);
  };

  AudioSink::AudioSink(const char * p)
  : AudioOutput("sink", p)
  {
  }

  AudioSink::~AudioSink()
  {
  }

  float
  AudioSink::amplitude()
  {
    return 0;
  }

  void
  AudioSink::amplitude(float value)
  {
  }

  // Write audio into the "short" type.
  std::size_t
  AudioSink::write16(const int16_t * array, std::size_t length)
  {
    return 0;
  }

  AudioOutput *
  Driver::AudioSink(const char * parameter)
  {
    return new ::FreeDV::AudioSink(parameter);
  }

#ifndef NO_INITIALIZERS

  static bool
  initializer()
  {
    init_driver_manager().register_audio_output("sink", Driver::AudioSink);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
