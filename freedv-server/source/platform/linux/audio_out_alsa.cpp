/// The ALSA audio output driver. 

#include "drivers.h"
#include <alsa/asoundlib.h>
#include <sys/ioctl.h>
#include <sstream>
#include <stdexcept>

namespace FreeDV {
  std::ostream & ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode);

  /// Audio output "ALSA", Uses the Linux ALSA Audio API.
  class AudioOutALSA : public AudioOutput {
  private:
    snd_pcm_t * handle;

  public:

	/// Instantiate the audio output.
  		AudioOutALSA(const char * parameters);
		~AudioOutALSA();

	/// Return the number of audio samples the device can handle in
	/// a write without blocking.
        virtual std::size_t
		ready();

        /// Write audio from the "short" type.
	virtual std::size_t
		write16(const std::int16_t * array, std::size_t length);
  };

  AudioOutALSA::AudioOutALSA(const char * p)
  : AudioOutput("alsa", p)
  {
    const int error = snd_pcm_open(
     &handle,
     p,
     SND_PCM_STREAM_PLAYBACK,
     0);

    if ( error ) {
      std::ostringstream str;

      str << "Can't open audio device " << p << ": " << snd_strerror(error);
      throw std::runtime_error(str.str().c_str());
    }
  }

  AudioOutALSA::~AudioOutALSA()
  {
    snd_pcm_close(handle);
  }

  // Write audio into the "short" type.
  std::size_t
  AudioOutALSA::write16(const std::int16_t * array, std::size_t length)
  {
    return length;
  }

  AudioOutput *
  Driver::AudioOutALSA(const char * parameter)
  {
    return new ::FreeDV::AudioOutALSA(parameter);
  }

  std::ostream &
  Enumerator::AudioOutALSA(std::ostream & stream)
  {
    return ALSAEnumerate(stream, SND_PCM_STREAM_PLAYBACK);
  }

  std::size_t
  AudioOutALSA::ready()
  {
    snd_pcm_sframes_t	available = 0;

    if ( (available = snd_pcm_avail(handle)) <= 0 )
      return available;
    else
      return 0;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_output(
     "alsa",
     Driver::AudioOutALSA,
     Enumerator::AudioOutALSA);
    return true;
  }
  static const bool initialized = initializer();
}
