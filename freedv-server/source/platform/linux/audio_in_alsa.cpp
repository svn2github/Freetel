/// The ALSA audio output driver. 

#include "drivers.h"
#include <alsa/asoundlib.h>
#include <sstream>
#include <stdexcept>

namespace FreeDV {
  std::ostream & ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode);

  /// Audio output "sink", discards the audio, for testing.
  class AudioInALSA : public AudioInput {
  private:
    snd_pcm_t * handle;

  public:

	/// Instantiate the audio sink.
  		AudioInALSA(const char * parameters);
		~AudioInALSA();

	/// Return the number of audio samples the device can handle in
	/// a write without blocking.
        virtual std::size_t
		ready();

        /// Read audio into the "short" type.
	virtual std::size_t
		read16(std::int16_t * array, std::size_t length);
  };

  AudioInALSA::AudioInALSA(const char * p)
  : AudioInput("alsa", p)
  {
    const int error = snd_pcm_open(
     &handle,
     p,
     SND_PCM_STREAM_CAPTURE,
     0);

    if ( error ) {
      std::ostringstream str;

      str << "Can't open audio device " << p << ": " << snd_strerror(error);
      throw std::runtime_error(str.str().c_str());
    }
  }

  AudioInALSA::~AudioInALSA()
  {
  }

  // Read audio into the "short" type.
  std::size_t
  AudioInALSA::read16(std::int16_t * array, std::size_t length)
  {
    return length;
  }

  AudioInput *
  Driver::AudioInALSA(const char * parameter)
  {
    return new ::FreeDV::AudioInALSA(parameter);
  }

  std::ostream &
  Enumerator::AudioInALSA(std::ostream & stream)
  {
    return ALSAEnumerate(stream, SND_PCM_STREAM_CAPTURE);
  }

  std::size_t
  AudioInALSA::ready()
  {
    return SIZE_MAX;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_input(
     "alsa",
     Driver::AudioInALSA,
     Enumerator::AudioInALSA);
    return true;
  }
  static const bool initialized = initializer();
}
