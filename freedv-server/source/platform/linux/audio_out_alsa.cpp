/// The ALSA audio output driver. 

#include "alsa.h"
#include "drivers.h"
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

namespace FreeDV {
  std::ostream & ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode);

  /// Audio output "ALSA", Uses the Linux ALSA Audio API.
  class AudioOutALSA : public AudioOutput {
  private:
    char * const	parameters;
    snd_pcm_t *		handle;
    std::size_t		period_size;
    bool		started;

    void
    do_throw(const int error, const char * message = 0)
    {
      std::ostringstream str;

      str << "Error on ALSA audio output " << parameters << ": ";
       if ( message )
         str << message << ": ";
       str << snd_strerror(error) << '.';
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
  : AudioOutput("alsa", p), parameters(strdup(p)),
    period_size(
     (int)ceil(((double)SampleRate / 1000.0) * MinimumFrameDuration)),
    started(false)
  {
    handle = ALSASetup(
     p,
     SND_PCM_STREAM_PLAYBACK,
     0,
     SND_PCM_FORMAT_S16,
     SND_PCM_ACCESS_RW_INTERLEAVED,
     1,
     SampleRate,
     period_size,
     (int)ceil(((double)SampleRate / 1000.0) * MaximumFrameDuration));
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
    int error = snd_pcm_writei(handle, array, length);
    started = true;
    if ( error == -EPIPE ) {
      std::cerr << "ALSA output " << parameters << ": write underrun." << std::endl;
      snd_pcm_recover(handle, error, 1);
      error = snd_pcm_writei(handle, array, length);
    }

    if ( error >= 0 )
      return error;
    else {
      do_throw(error, "Write");
      return 0; // do_throw doesn't return.
    }
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
    snd_pcm_sframes_t	delay = 0;
    int			error;

    if ( !started )
      return period_size * 2;

    error = snd_pcm_avail_delay(handle, &available, &delay);

    if ( error == -EPIPE ) {
      std::cerr << "ALSA output " << parameters << ": ready underrun." << std::endl;
      snd_pcm_recover(handle, error, 1);
      snd_pcm_pause(handle, 1);
      started = false;
      return (int)ceil(((double)SampleRate / 1000.0) * MinimumFrameDuration);

      if ( error < 0 )
        return 0;
    }

    if ( error == 0 )
      return available;
    else if ( error == -EPIPE )
      return 0;
    else {
      do_throw(error, "Get Frames Available for Write");
      return 0; // do_throw doesn't return.
    }
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
