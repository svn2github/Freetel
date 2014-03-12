/// The ALSA audio input driver. 

#include <stdlib.h>
#include <errno.h>
#include "drivers.h"
#include <alsa/asoundlib.h>
#include <sys/ioctl.h>
#include <sstream>
#include <stdexcept>

namespace FreeDV {
  std::ostream & ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode);

  /// Audio input "ALSA", Uses the Linux ALSA Audio API.
  class AudioInALSA : public AudioInput {
  private:
    static const int	overlong_delay = 300;
    static const int	delay_goal = 50;

    char * const	parameters;
    snd_pcm_t *		handle;

    void
    do_throw(const int error, const char * message = 0)
    {
      std::ostringstream str;

      str << "Error on ALSA audio input " << parameters << ": ";
       if ( message )
         str << message << ": ";
       str << snd_strerror(error);
      throw std::runtime_error(str.str().c_str());
    }
  public:

	/// Instantiate the audio input.
  		AudioInALSA(const char * parameters);
		~AudioInALSA();

	/// Return the number of audio samples the device can provide in
	/// a read without blocking.
        virtual std::size_t
		ready();

        /// Read audio from the "short" type.
	virtual std::size_t
		read16(std::int16_t * array, std::size_t length);
  };

  AudioInALSA::AudioInALSA(const char * p)
  : AudioInput("alsa", p), parameters(strdup(p))
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
     SND_PCM_STREAM_CAPTURE,
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

    snd_pcm_start(handle);
  }

  AudioInALSA::~AudioInALSA()
  {
    snd_pcm_drop(handle);
    snd_pcm_close(handle);
    free(parameters);
  }

  // Read audio into the "short" type.
  std::size_t
  AudioInALSA::read16(std::int16_t * array, std::size_t length)
  {
    int result = snd_pcm_readi(handle, array, length);
    if ( result == -EPIPE ) {
      snd_pcm_recover(handle, result, 1);
      result = snd_pcm_readi(handle, array, length);
      std::cerr << "ALSA input: read underrun." << std::endl;
      if ( result == -EPIPE )
        return 0;
    }
    if ( result >= 0 ) {
      return result;
    }
    else {
      do_throw(result, "Read");
      return 0; // do_throw doesn't return.
    }
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
    snd_pcm_sframes_t	available = 0;
    snd_pcm_sframes_t	delay = 0;
    int			error;

    error = snd_pcm_avail_delay(handle, &available, &delay);

    // If the program has been paused, there will be a long queue of incoming
    // audio samples and a corresponding delay. This can also happen if the
    // incoming audio interface runs at a faster rate than the outgoing one.
    if ( delay >= overlong_delay && available > 0 ) {
      int dropped = 0;

      snd_pcm_drop(handle);
      snd_pcm_prepare(handle);
      snd_pcm_start(handle);

      const double seconds = (double)delay / (double)SampleRate;
      std::cerr << "ALSA input: program paused, dropped "
       << seconds << " seconds of queued audio samples." << std::endl;
    }

    if ( error == -EPIPE ) {
      snd_pcm_recover(handle, error, 1);
      available = snd_pcm_avail_delay(handle, &available, &delay);
      std::cerr << "ALSA input: read underrun." << std::endl;
    }

    if ( error >= 0 )
      return available;

    else if ( error == -EPIPE )
      return 0;
    else {
      do_throw(error, "Get Frames Available for Read");
      return 0; // do_throw doesn't return.
    }
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
