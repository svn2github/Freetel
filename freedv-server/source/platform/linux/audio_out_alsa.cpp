/// \file platform/linux/audio_out_alsa.cpp
/// ALSA audio output device driver, for use on Linux.
/// There is at least one other operating system that supports an ALSA-like
/// interface, Nucleus, but at this writing (early 2014) this driver is
/// untested on that system.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "alsa.h"
#include "drivers.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

namespace FreeDV {
  std::ostream & ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode);

  /// Audio output "ALSA", Uses the Linux ALSA Audio API.
  ///
  class AudioOutALSA : public AudioOutput {
  private:
    snd_pcm_t *		handle;
    char * const	parameters;
    bool		started;

    // Copy constructor and operator=() disabled.
    AudioOutALSA(const AudioOutALSA &);
    AudioOutALSA & operator=(const AudioOutALSA &);

    NORETURN void
    do_throw(const int error, const char * message = 0);
  public:

	/// Instantiate the audio output.
  		AudioOutALSA(const char * parameters);
		~AudioOutALSA();

        /// Return file descriptors for poll()
 	/// \param array The address of an array that will be written
	/// with a sequence of file descriptors.
        /// \param space The maximum number of file descriptors that may be
        /// stored in the array.
        /// \return The number of file descriptors written to the array.
	virtual int
		poll_fds(PollType * array, int space);

	/// Return the number of audio samples the device can handle in
	/// a write without blocking.
        virtual std::size_t
		ready();

        /// Start the audio device.
        /// Output devices: prepare for samples to be sent.
        virtual void
		start();
    
        /// Stop the audio device.
        /// Output devices: stop playing samples.
        virtual void
		stop();

        /// Write audio from the "short" type.
	virtual std::size_t
		write16(const std::int16_t * array, std::size_t length);
  };

  AudioOutALSA::AudioOutALSA(const char * p)
  : AudioOutput("alsa", p), handle(0), parameters(strdup(p)), started(false)
  {
    handle = ALSASetup(
     p,
     SND_PCM_STREAM_PLAYBACK,
     0,
     SND_PCM_FORMAT_S16,
     SND_PCM_ACCESS_RW_INTERLEAVED,
     1,
     SampleRate,
     AudioFrameSamples,
     AudioFrameSamples * 2);

    if ( handle == 0 )
      do_throw(-ENODEV);
  }

  AudioOutALSA::~AudioOutALSA()
  {
    snd_pcm_drop(handle);
    snd_pcm_close(handle);
    free(parameters);
  }

  NORETURN void
  AudioOutALSA::do_throw(const int error, const char * message)
  {
    std::ostringstream str;

    str << "Error on ALSA audio output \"" << parameters << "\": ";
     if ( message )
       str << message << ": ";
     str << snd_strerror(error) << '.';
    throw std::runtime_error(str.str().c_str());
  }

  // Write audio into the "short" type.
  std::size_t
  AudioOutALSA::write16(const std::int16_t * array, std::size_t length)
  {
    if ( !started ) {
      // Preload the audio output queue with a fraction of a frame of silence.
      // This makes underruns less likely.
      // This delays the output from where we would otherwise have started it.
      // Otherwise we tend to underrun repeatedly at startup time.
      //
      // Note that all inputs and outputs of the typical user do not run on
      // a shared clock. There is the potential for overruns or underruns
      // during long operation because of this. Professional studios use
      // a shared clock, and the more expensive equipment that supports it,
      // to avoid this problem.
      //
      int16_t	buf[AudioFrameSamples];
      memset(buf, 0, sizeof(buf));
      snd_pcm_writei(handle, buf, sizeof(buf) / sizeof(*buf));
    }

    int error = snd_pcm_writei(handle, array, length);
    started = true;
    if ( error == -EPIPE ) {
      std::cerr << "ALSA output \"" << parameters << "\": write underrun." << std::endl;
      snd_pcm_recover(handle, error, 1);
      error = snd_pcm_writei(handle, array, length);
    }

    if ( error >= 0 )
      return error;
    else
      do_throw(error, "Write");
  }

  AudioOutput *
  Driver::AudioOutALSA(const char * parameter)
  {
    return new ::FreeDV::AudioOutALSA(parameter);
  }

  static std::ostream &
  AudioOutALSAEnumerator(std::ostream & stream)
  {
    return ALSAEnumerate(stream, SND_PCM_STREAM_PLAYBACK);
  }

  int
  AudioOutALSA::poll_fds(PollType * array, int space)
  {
    const int size = snd_pcm_poll_descriptors_count(handle);
    
    snd_pcm_poll_descriptors(
     handle,
     array,
     space);
    
    return size;
  }

  std::size_t
  AudioOutALSA::ready()
  {
    snd_pcm_sframes_t	available = 0;
    snd_pcm_sframes_t	delay = 0;
    int			error;

    if ( !started )
      return AudioFrameSamples;

    error = snd_pcm_avail_delay(handle, &available, &delay);
    if ( delay > (AudioFrameSamples * 8) ) {
      const double seconds = (double)delay / (double)SampleRate;

      std::cerr << "ALSA output \"" << parameters
       << "\": overlong delay, dropped  "
       << seconds << " seconds of output." << std::endl;
      snd_pcm_drop(handle);
      snd_pcm_prepare(handle);
      snd_pcm_pause(handle, 1);
      started = false;
      return AudioFrameSamples;
    }

    if ( error == -EPIPE ) {
      std::cerr << "ALSA output \"" << parameters << "\": ready underrun." << std::endl;
      snd_pcm_recover(handle, error, 1);
      snd_pcm_pause(handle, 1);
      started = false;

      return AudioFrameSamples;
    }

    if ( error == 0 )
      return available;
    else
      do_throw(error, "Get Frames Available for Write");
  }

  void
  AudioOutALSA::start()
  {
    snd_pcm_drop(handle);
    snd_pcm_prepare(handle);
    snd_pcm_pause(handle, 1);
  }

  void
  AudioOutALSA::stop()
  {
    snd_pcm_drop(handle);
    snd_pcm_prepare(handle);
    snd_pcm_pause(handle, 1);
    started = false;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_output(
     "alsa",
     Driver::AudioOutALSA,
     AudioOutALSAEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
