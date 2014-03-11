/// The AudioSink driver, called as "sink", discards the audio, for testing.

#include "drivers.h"

namespace FreeDV {
  /// Audio output "sink", discards the audio, for testing.
  class AudioSink : public AudioOutput {
  public:

	/// Instantiate the audio sink.
  		AudioSink(const char * parameters);
		~AudioSink();

	/// Return the number of audio samples the device can handle in
	/// a write without blocking. This version always returns SIZE_MAX.
        virtual std::size_t
		ready();

        /// Write audio into the "short" type.
	virtual std::size_t
		write16(const std::int16_t * array, std::size_t length);
  };

  AudioSink::AudioSink(const char * p)
  : AudioOutput("sink", p)
  {
  }

  AudioSink::~AudioSink()
  {
  }

  // Write audio into the "short" type.
  std::size_t
  AudioSink::write16(const std::int16_t *, std::size_t length)
  {
    return length;
  }

  std::size_t
  AudioSink::ready()
  {
    return SIZE_MAX;
  }

  AudioOutput *
  Driver::AudioSink(const char * parameter)
  {
    return new ::FreeDV::AudioSink(parameter);
  }

  std::ostream &
  Enumerator::AudioSink(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_output("sink", Driver::AudioSink, Enumerator::AudioSink);
    return true;
  }
  static const bool initialized = initializer();
}
