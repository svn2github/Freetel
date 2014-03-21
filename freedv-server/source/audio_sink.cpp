/// \file audio_sink.cpp Audio Sink Driver.
/// The AudioSink driver, called as --loudspeaker="sink" or
/// --transmitter="sink". It discards the audio, for testing.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  /// Audio output "sink", discards the audio, for testing.
  ///
  class AudioSink : public AudioOutput {
  public:

	/// Instantiate the audio sink.
 	///
  		AudioSink(const char * parameters);
		~AudioSink();

        /// Return file descriptors for poll()
 	/// \param array The address of an array that will be written
	/// with a sequence of file descriptors.
        /// \param space The maximum number of file descriptors that may be
        /// stored in the array.
        /// \return The number of file descriptors written to the array.
	virtual int
		poll_fds(PollType * array, int space);

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

  std::size_t
  AudioSink::write16(const std::int16_t *, std::size_t length)
  {
    return length;
  }

  // 
  int
  AudioSink::poll_fds(PollType *, int)
  {
    return 0;
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

  /// Enumerate the AudioSink driver, for displaying the available device
  /// drivers to the user.
  /// This version is silent, because "sink" is for testing rather than
  /// normal use.
  static std::ostream &
  AudioSinkEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_audio_output(
     "sink",
     Driver::AudioSink,
     AudioSinkEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
