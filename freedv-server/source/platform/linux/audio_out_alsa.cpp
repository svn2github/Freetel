/// The ALSA audio output driver. 

#include <stdlib.h>
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
    char * const	parameters;
    snd_pcm_t *		handle;

    volatile void
    do_throw(const int error, const char * message = 0)
    {
      std::ostringstream str;

      str << "Error on ALSA audio output " << parameters << ": ";
       if ( message )
         str << message << ": ";
       str << snd_strerror(error);
      throw std::runtime_error(str.str().c_str());
    }
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
  : AudioOutput("alsa", p), parameters(strdup(p))
  {
    const snd_pcm_format_t  	format = SND_PCM_FORMAT_S16;
    const snd_pcm_access_t  	access = SND_PCM_ACCESS_RW_INTERLEAVED;
    const unsigned int  	channels = 1;
    const unsigned int  	rate = 48000;
    const int			soft_resample = 0;
    const unsigned int  	latency = 0;
    int				error;

    handle = 0;
    error = snd_pcm_open(
     &handle,
     p,
     SND_PCM_STREAM_PLAYBACK,
     0);

    if ( error != 0 )
      do_throw(error, "Open");

    if ( (error = snd_pcm_set_params(
     handle,
     format,
     access,
     channels,
     rate,
     soft_resample,
     latency)) < 0 )
      do_throw(error, "Set Parameters");

    if ( (error = snd_pcm_prepare(handle)) < 0 )
      do_throw(error, "Prepare");
  }

  AudioOutALSA::~AudioOutALSA()
  {
    snd_pcm_drop(handle);
    snd_pcm_close(handle);
    free(parameters);
  }

  // Write audio into the "short" type.
  std::size_t
  AudioOutALSA::write16(const std::int16_t * array, std::size_t length)
  {
    const int result = snd_pcm_writei(handle, array, length);
    if ( result >= 0 )
      return result;
    else
      do_throw(result, "Write");
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

    if ( (available = snd_pcm_avail(handle)) >= 0 )
      return available;
    else
      do_throw(available, "Get Available Frames");
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
