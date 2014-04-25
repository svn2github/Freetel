/// \file drivers.h
/// FreeDV driver interface definitions.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///


#include <cstdint>
#include <ostream>
#include <assert.h>
#include "platform.h"

/// Portable way to use the [[unused]] C++11 attribute before all compilers
/// have caught up with C++11.
///
#ifndef	UNUSED
# if __cplusplus > 199711L
#  if __GNUC__ && ( __GNUC_MAJOR__ <= 4 || __GNUC_MINOR__ >= 8 )
#   define UNUSED __attribute__((unused))
#  else
#   define UNUSED [[unused]]
#  endif
# else
#  define UNUSED
# endif
#endif

/// Portable way to use the [[noreturn]] C++11 attribute before all compilers
/// have caught up with C++11.
///
#ifndef	NORETURN
# if __cplusplus > 199711L
#  if __GNUC__ && ( __GNUC_MAJOR__ <= 4 || __GNUC_MINOR__ >= 8 )
#   define NORETURN __attribute__((noreturn))
#  else
#   define NORETURN [[noreturn]]
#  endif
# else
#  define NORETURN
# endif
#endif

/// \namespace FreeDV Namespace used for all code in the FreeDV program.
///
namespace FreeDV {
/// This propogates argv[0] so that it can be used in error messages.
///
extern const char *	program_name;

/// The sample rate used by all audio interfaces in the program.
/// the sound cards are in general driven at 48000 because that's
/// the only reliable sample rate they all have in common. SampleRate
/// may be lower than that and thus there may be resampling in the
/// drivers.
const unsigned int	SampleRate = 48000;

/// The number of audio samples per millisecond, at SampleRate.
///
const unsigned int	SamplesPerMillisecond = ((double)SampleRate / 1000.0);

/// The audio frame duration in milliseconds. The audio interfaces will
/// use this as a period size. It should be 1/2 of the smallest codec frame
/// size we expect to use.
const unsigned int	AudioFrameDuration = 10;

/// The number of audio samples in an audio frame. Audio frames must be a
/// power of two (this is a common hardware requirement) and must be shorter
/// than any codec/modem frame in the program.
const unsigned int	AudioFrameSamples = 512;

/// The number of audio samples in the maximum-duration frame.
/// This must be a power of two (this is a common hardware requirement) and
/// must be at least twice the value of AudioFrameSamples.
///
const unsigned int	MaximumFrameSamples = 32768;

/// Allocate memory and copy a string into it, so that it is permanently
/// stored.
/// \param s The string to be copied.
/// \return The new copy. It's the caller's responsibility to free this data,
///  or a memory leak will occurr.
char *	copy_string(const char * s);

/// Simple C++ FIFO buffer class with zero copy (most of the time).
/// Not thread-safe on its own, you must have a mutex for access to it.
/// Doesn't grow, size is specified at instantiation.
/// Written to avoid STL templates, Boost, etc. in order to keep down the
/// size of the embedded version of this program.
class FIFO {
private:
    uint8_t * const		buffer;
    const uint8_t * const	buffer_end;
    uint8_t *			in;
    const uint8_t *		out;

    void	get_overrun() const;
    uint8_t *	reorder(std::size_t length);

    // Copy constructor and operator =() disabled.
    FIFO(const FIFO &);
    FIFO & operator=(const FIFO &);
public:
    /// Create the FIFO object.
    /// \param length The size of the fifo, in bytes.
    FIFO(std::size_t length);

    ~FIFO();

    /// Returns the amount of data available to read.
    /// \return The amount of data, in bytes, available to read.
    inline std::size_t	get_available() const {
        return in - out;
    }

    /// Return the address of output data of the requested length.
    /// \param length The amount of data requested. This must be smaller
    /// than or equal to the amount returned by get_available().
    /// \return The address of the data to be read.
    const uint8_t *
    get(std::size_t length);

    /// Finish the I/O after get().
    /// \param length The amount of data, in bytes, actually read.
    /// This must be smaller than or equal to the amount passed to
    /// get().
    void		get_done(std::size_t length);

    /// Returns the amount of space available for incoming data.
    /// \return The amount of space, in bytes, available for incoming data.
    std::size_t	put_space() const {
        return (buffer_end) - in + (out - buffer);
    }

    /// Return the address of an incoming data buffer of the requested size.
    /// Throws an error if we run the buffer out of space. Well-behaved code
    /// won't allocate a size that can't be drained before it is further
    /// written.
    /// You must call put_done(length) when the I/O is completed.
    /// The length passed to put_done() must be smaller than or equal
    /// to the length passed to put().
    /// \param length The size of buffer in chars requested.
    /// \return The address of the buffer for incoming data.
    uint8_t *	put(std::size_t length);

    /// Complete the I/O after put().
    /// \param length The amount of data actually written. This must be
    /// smaller than or equal to the length passed to put().
    void		put_done(std::size_t length);

    /// Discard any buffered data.
    ///
    void		reset();
};

/// Set the real-time parameters in the scheduler before running our main
/// loop. This function call wraps a platform-dependent implementation.
/// This may effect more than one scheduler, for example the process
/// scheduler and - if there is one - the I/O scheduler.
/// Where facilities are not available or implemnented, or the process
/// has insufficient privilege, this may emit a warning and/or do nothing.
void		set_scheduler();

/// Check the user's privileges, and warn if they are inappropriate.
///
void		check_privileges();

/// Virtual base class for all driver classes.
///
class Base {
private:
    /// The copy constructor is private to prevent it from being used.
    /// \param that Not used.
    Base(const Base & that);

    /// The assignment operator is private to prevent it from being used.
    /// \param that Not used.
    Base &		operator = (const Base & that);

protected:
    /// The name of the driver. This must be the same as the name it
    /// is registered under. It's expected to be from a per-class static
    /// string and thus should not be deleted.
    const char *
    name;

    /// The parameters to this instance of the driver. They are
    /// copied here so that we can print them later in operator<<() .
    /// the copy is deleted when in the ~Base() destructor.
    const char *
    parameters;

    /// Constructor for the virtual base class.
    /// \param _name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param _parameters Driver-specific configuration parameters.
    Base(const char * _name, const char * _parameters);

public:
    /// Destroy the base class.
    virtual		~Base() = 0;

    /// Return true if the object is owned by a UserInterface object and
    /// should not be destroyed separately.
    /// The result is invariant for a particular object (or possibly
    /// class).
    virtual bool
    captive() const;

    /// Write the driver information onto a stream, for debugging and
    /// for dumping the configuration information.
    /// \param stream A reference to an instance of ostream upon which the
    ///  object information is to be rendered.
    /// \return A reference to the provided stream, meant for the
    ///  usual successive call paradigm of ostream operator << .
    std::ostream &	print(std::ostream & stream) const;
};

/// Write the driver information from the Base object onto a stream,
/// for debugging and dumping the configuration information.
/// \param stream A reference to an instance of ostream upon which the
///  object information is to be rendered.
/// \param base A reference to the Base class providing the information.
/// \return A reference to the provided stream, meant for the
///  usual successive call paradigm of ostream operator << .
inline std::ostream &
operator << (std::ostream & stream, const Base & base) {
    return base.print(stream);
}

/// Virtual base class for all drivers that perform non-blocking I/O.
/// These are AudioInput and AudioOutput, PTTInput, TextInput,
/// KeyingOutput and UserInterface.
class IODevice : public ::FreeDV::Base {
protected:
    /// Construct an I/O device.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    IODevice(const char * name, const char * parameters);

public:
    /// Return the number of audio samples, or bytes for devices other than
    /// audio interfaces, that can be read or written. Return zero if there
    /// is not enough readable or writable data to do some work, for example
    /// if the device is a PTT input and the internal driver detects less
    /// readable data than necessary to read the HID status.
    ///
    /// All drivers present a unidirectional interface.
    /// If the underlying device is bidirectional that detail is hidden and
    /// we present one or more separate read and write drivers.
    /// \return The number of audio samples or bytes that can be read or
    /// written.
    virtual std::size_t	ready() = 0;

    /// Type used by poll_fds(). For portability to Windows.
    ///
    typedef struct pollfd	PollType;

    /// Poll file descriptors for available I/O.
    ///
    static  int		poll(PollType * array, int length, int timeout);

    /// Get the file descriptors to be used to poll for available I/O.
    ///
    virtual int		poll_fds(PollType * array, int space) = 0;

    virtual		~IODevice() = 0;
};

/// Virtual base class for AudioInput and AudioOutput.
///
class AudioDevice : public ::FreeDV::IODevice {
protected:
    /// The master volume control for the device.
    ///
    float		master_amplitude;

    /// Create an AudioDevice instance.
    /// \param name The name of the child driver. This is expected to be a
    /// static string.
    /// \param parameters Driver-specific configuration parameters.
    AudioDevice(const char * name, const char * parameters);

public:
    /// Destroy an AudioDevice instance.
    virtual		~AudioDevice() = 0;

    /// Get the current audio level.
    /// \return The current audio level.
    /// The value is normalized to the range of 0.0 to 1.0.
    virtual float	amplitude();

    /// Set the current audio level.
    /// \param value The new value for the current audio level.
    /// The value must be normalized within the range of 0.0 to 1.0.
    virtual void	amplitude(float value);

    /// Start the audio device.
    /// Input devices: start digitizing samples for the program to subsequently
    /// read.
    /// Output devices: prepare for samples to be sent.
    /// The default implementation of this function does nothing.
    virtual void	start();

    /// Stop the audio device.
    /// Input devices: stop digitizing samples.
    /// Output devices: stop playing samples.
    /// The default implementation of this function does nothing.
    virtual void	stop();
};

/// Virtual base class for audio input drivers.
///
class AudioInput : public ::FreeDV::AudioDevice {
protected:
    /// Create an AudioInput device instance.
    /// \param name The name of the child driver. This is expected to be a
    /// static string.
    /// \param parameters Driver-specific configuration parameters.
    AudioInput(const char * name, const char * parameters);

public:
    /// Destroy an AudioInput device instance.
    ///
    virtual		~AudioInput() = 0;

    /// Read audio into an array of the signed 16-bit integer type.
    /// Depending on the underlying device and its non-blocking status,
    /// this may write fewer bytes than requested. It's permissible for
    /// it to write no bytes and return 0.
    /// \param array The array of audio samples to be read.
    /// \param length The number of 16-bit audio samples which are to be read.
    /// \return The number of 16-bit audio samples that were actually read.
    /// This may be smaller than *length*, or it may be zero.
    virtual std::size_t

    read16(std::int16_t * array, std::size_t length) = 0;
};

/// Virtual base class for audio output drivers.
///
class AudioOutput : public ::FreeDV::AudioDevice {
protected:
    /// Create an AudioOutput device instance.
    /// \param name The name of the child driver. This is expected to be a
    /// static string.
    /// \param parameters Driver-specific configuration parameters.
    AudioOutput(const char * name, const char * parameters);

public:
    /// Destroy an AudioOutput device instance.
    ///
    virtual		~AudioOutput() = 0;

    /// Wait for all output to be emitted by the audio device and then
    // return.
    virtual void	drain() = 0;

    /// Write audio from an array of the signed 16-bit integer type.
    /// Depending on the underlying device and its non-blocking status,
    /// this may write fewer bytes than requested. It's permissible for
    /// it to write no bytes and return 0.
    /// \param array The array of audio samples to be written.
    /// \param length The number of 16-bit audio samples which are to be
    /// written.
    /// \return The number of 16-bit audio samples that were actually written.
    /// This may be smaller than *length*, or it may be zero.
    virtual std::size_t
    write16(const std::int16_t * array, std::size_t length) = 0;
};

/// Virtual base class for codecs.
///
class Codec : public ::FreeDV::Base {
protected:
    /// Create a codec instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    Codec(const char * name, const char * parameters);

public:
    /// Destroy a codec instance.
    ///
    virtual		~Codec() = 0;

    /// Decode from data bytes to audio samples.
    /// Depending on the internal frame size for this particular codec
    /// and the status of the incoming signal, this may consume and produce
    /// less data than requested. It need not consume any data until it is
    /// presented with enough to produce a full frame. It may throw away
    /// as much incoming data as it likes without producing any.
    /// \param i The encoded data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after decoding, in an array
    /// of signed 16-bit integers.
    /// \param data_length When called: The number of bytes of data that are
    /// available to be decoded. On return: the number of bytes of data
    /// that were consumed.
    /// \param sample_length The number of audio samples that may be decoded.
    /// \return The number of audio samples that were actually decoded.
    virtual std::size_t
    decode16(
     const std::uint8_t * i,
     std::int16_t * o,
     std::size_t * data_length,
     std::size_t sample_length) = 0;


    /// Encode from audio samples to data bytes.
    /// Depending on the internal frame size for this particular codec,
    /// this may consume and produce less data than requested. It need not
    /// consume any data until it is presented with enough to produce a full
    /// frame.
    /// \param i The array of audio samples to be encoded, in an array
    /// of signed 16-bit integers.
    /// \param o The encoded data, in an array of unsigned 8-bit integers.
    /// \param sample_length On call: The number of audio samples to be
    /// encoded. On return: The number of 16-bit audio samples that were
    /// consumed.
    /// \return The number of std::uint8_t elements in the encoded array.
    virtual std::size_t
    encode16(
     const std::int16_t * i,
     std::uint8_t * o,
     std::size_t data_length,
     std::size_t * sample_length) = 0;

    /// Return the minimum duration of a frame in milliseconds.
    /// \return The minimum duration of a frame in milliseconds.
    virtual int
    min_frame_duration() const = 0;
};

/// Virtual base class for protocol framers.
///
class Framer : public ::FreeDV::Base {
protected:
    /// Create a framer instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    Framer(const char * name, const char * parameters);

public:
    /// Destroy a framer instance.
    ///
    virtual		~Framer() = 0;

    /// Return the minimum duration of a frame in milliseconds.
    /// \return The minimum duration of a frame in milliseconds.
    virtual int
    min_frame_duration() const = 0;

    /// Decode from modem data to codec frames, removing the wrapping protocol.
    /// \param i The encoded data, in an array of unsigned 8-bit integers.
    /// \param o The array of codec data after decoding, in an array
    /// of unsigned 8-bit integers.
    /// \param input_length When called: The number of bytes of data that are
    /// available to be unwrap. On return: the number of bytes of data
    /// that were consumed.
    /// \param output_length The number of codec data bytes that can be
    /// unwrapped.
    /// \return The number of data bytes that were actually decoded.
    virtual std::size_t
    unwrap(const std::uint8_t * i,
           std::uint8_t * o,
           std::size_t * input_length,
           std::size_t output_length) = 0;

    /// Wrap codec data bytes in a protocol for transmission through the modem.
    /// \param i The array of data bytes to be encoded, in an array
    /// of unsigned 8-bit integers.
    /// \param o The encoded data, in an array of unsigned 8-bit integers.
    /// \param input_length The number of data bytes to be wrapped.
    /// \param output_length The number of data bytes available to store the
    /// wrapped data.
    /// \return The number of std::uint8_t elements in the wrapped array.
    virtual std::size_t
    wrap(
        const std::uint8_t * i,
        std::uint8_t * o,
        std::size_t * input_length,
        std::size_t output_length) = 0;
};

/// Radio device keying driver.
///
class KeyingOutput : public ::FreeDV::IODevice {
protected:
    /// Create an radio keying output device instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    KeyingOutput(const char * name, const char * parameters);

public:
    /// Destroy the radio keying device instance.
    ///
    virtual		~KeyingOutput() = 0;

    /// Key or un-key the transmitter.
    /// \param value If true, key the transmitter. If false, un-key.
    virtual void	key(bool value) = 0;
};

/// Softmodem driver.
///
class Modem : public ::FreeDV::Base {
protected:
    /// Create a softmodem device instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    Modem(const char * name, const char * parameters);

public:
    virtual		~Modem() = 0;

    /// Demodulate from audio samples to data.
    /// Depending on the internal frame size for this particular modem and
    /// the status of the incoming signal, this may consume and produce less
    /// data than requested. It need not consume any data until it is
    /// presented with enough to produce a full frame. It may throw away as
    /// much incoming data as it likes without producing any.
    /// \param i The array of audio samples to be demodulated, in an array
    /// of signed 16-bit integers.
    /// \param o The demodulated data, in an array of unsigned 8-bit integers.
    /// \param data_length The number of bytes of data that may be demodulated.
    /// \param sample_length On call: The number of audio samples to be
    /// demodulated. On return: The number of audio samples consumed.
    /// \return The number of bytes of data that were actually decoded.
    virtual std::size_t
    demodulate16(
        const std::int16_t * i,
        std::uint8_t * o,
        std::size_t data_length,
        std::size_t * sample_length) = 0;

    /// Modulate from data to audio samples.
    /// Depending on the internal frame size for this particular modem,
    /// this may consume and produce less data than requested. It need not
    /// consume any data until it is presented with enough to produce a full
    /// frame.
    /// \param i The data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after modulation, in an array
    /// of signed 16-bit integers.
    /// \param data_length On call: The number of bytes of data to be
    /// modulated. On return: the number of bytes that were consumed.
    /// \param sample_length On call: The number of audio samples to be
    /// modulated.
    /// \return The number of 16-bit audio samples that were produced.
    virtual std::size_t
    modulate16(
     const std::uint8_t * i,
     std::int16_t * o,
     std::size_t * data_length,
     std::size_t sample_length) = 0;

    /// Return the minimum duration of a frame in milliseconds.
    /// \return The minimum duration of a frame in milliseconds.
    virtual int
    min_frame_duration() const = 0;
};

/// Push-to-talk input driver.
///
class PTTInput : public ::FreeDV::IODevice {
protected:
    /// Create a push-to-talk switch instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    PTTInput(const char * name, const char * parameters);
public:
    virtual		~PTTInput() = 0;

    /// Return true if the PTT is pressed.
    /// ready() must return true before this member is called.
    /// \return True if the PTT is pressed.
    virtual bool	state() = 0;
};

/// Driver for the text message source function.
///
class TextInput : public ::FreeDV::IODevice {
protected:
    /// The child class calls this member in its parent to set the text.
    ///
    void		set(const char * text);


    /// Create a text source instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    TextInput(const char * name, const char * parameters);

public:
    /// Read the text data.
    ///
    virtual std::size_t	read(char * buffer, std::size_t length) = 0;

    virtual		~TextInput() = 0;
};

class Interfaces;

/// Generic base class for user interfaces.
/// They may be graphical, they may be server-client interfaces,
/// they may be specialized hardware devices, especially if this
/// software is embedded.
/// There must be inputs and callbacks for many things here.
/// UserInterfaces may provide their own drivers for microphone,
/// loudspeaker, TextInput, and both forms of PTT.
class UserInterface : public ::FreeDV::IODevice {
private:
    // Copy constructor and operator =() disabled.
    UserInterface(const UserInterface &);
    UserInterface & operator=(const UserInterface &);
protected:
    /// The external Interfaces object.
    ///
    Interfaces *	interfaces;

    /// Create an instance of the UserInterface object.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    /// \param _interfaces An Interface object. The UserInterface object
    /// may set various fields of Interface to be its own captive driver
    /// objects, and they may change during operation if the user changes
    /// device driver parameters.
    UserInterface(const char * name, const char * parameters, Interfaces * _interfaces);

public:
    virtual		~UserInterface() = 0;
};

/// Structure used to pass all of the drivers. Can be modified while the
/// program is running.
class Interfaces {
private:
    // Copy constructor and operator=() disabled.
    		Interfaces(const Interfaces &);
    		Interfaces & operator=(const Interfaces &);
public:
    		Interfaces();
    virtual	~Interfaces() final;

    /// The voice codec in use.
    ///
    Codec *		codec;
    /// The Framer handles the protocol which wraps the codec data.
    /// It can decline to feed any audio on to the codec if the protocol says
    /// that should not happen, for example if the data isn't addressed to us.
    Framer * 		framer;
    /// The output used to key the transmitter.
    ///
    KeyingOutput *	keying_output;
    /// The audio output which drives the loudspeaker or headphone.
    ///
    AudioOutput *	loudspeaker;
    /// The audio input from the microphone.
    ///
    AudioInput *	microphone;
    /// The softmodem.
    ///
    Modem *		modem;
    /// The PTT input that indicates the transmission is to be digital audio.
    ///
    PTTInput *		ptt_input_digital;
    /// The PTT input that indicates the transmission is to be SSB.
    ///
    PTTInput *		ptt_input_ssb;
    /// The audio input from the receiver.
    ///
    AudioInput *	receiver;
    /// The text to be transmitted in our text side-channel.
    ///
    TextInput *		text_input;
    /// The audio output that drives the transmitter.
    ///
    AudioOutput *	transmitter;
    /// The user interface driver. Used for GUIs.
    ///
    UserInterface *	user_interface;

    /// Fill in default drivers if the user or UserInterface hasn't set any.
    ///
    void		fill_in();

    /// Write the command-line flags necessary to configure the drivers as
    /// they are presently configured to the stream. This is used to save
    /// the configuration or debug the program.
    /// \param stream A reference to an instance of ostream on which the
    /// \param program_name The name of the executable file for this program.
    /// \return A reference to the provided stream, meant for the
    ///  usual successive call paradigm of ostream operator << .
    ///
    virtual std::ostream &
    print(std::ostream & stream, const char * program_name) const;
};

/// Write the driver information from the Interfaces object onto a stream,
/// for debugging and dumping the configuration information.
/// \param stream A reference to an instance of ostream upon which the
///  object information is to be rendered.
/// \param interfaces a reference to an Interfaces object providing
///  information about the device and algorithm drivers to be used.
/// \return A reference to the provided stream, meant for the
///  usual successive call paradigm of ostream operator << .
///
inline std::ostream &
operator << (std::ostream & stream, const Interfaces & interfaces) {
    return interfaces.print(stream, 0);
}

/// Functions for opening device drivers.
/// Most of the functions in the Driver namespace are registered with the
/// driver manager at run-time. Only embedded programs that do not use
/// big_main.cpp and driver_manager.cpp will need to reference them.
///
namespace Driver {
/// The "tone" audio input driver, which plays a combination of multiple sine
/// waves at user-selected frequencies and amplitudes, for testing.
/// \param parameter A list of frequencies and amplitudes which the driver
/// shall synthesize. For example, "1000,0.5;60,0.5" would play a 1 kHz tone
/// and a 60 Hz tone at equal levels. The frequency specified must be greater
/// than zero and less than or equal to 1/2 of SampleRate. The amplitudes
/// selected should add up to 1.0 or less.
/// \return A pointer to the AudioInput instance for the tone generator.
///
AudioInput *	Tone(const char * parameter);

/// An ALSA audio input driver for use on Linux.
/// \param parameter The name of the sound device to open, in the form of
/// "hw:*n*", "plughw:*n*" or a "longname" as returned by
/// sound_pcm_card_get_longname() or a shortened version of that name
/// anchored at the start and with an arbitrary portion of the end removed.
/// In the case of shortened names, the first device with a longname that
/// matches the shortened name provided (anchored at the start of the longname
/// and proceeding for the length of the provided string but not beyond it)
/// will be opened.
/// \return A pointer to the AudioInput instance for the ALSA device selected.
///
AudioInput *	AudioInALSA(const char * parameter);

/// Opens the default audio input interface of the operating system.
/// \return A pointer to an AudioInput instance for the default audio input
/// device.
///
AudioInput *	AudioInDefault();

/// The "sink" audio output driver, which discards all audio sent to it,
/// for testing.
/// \param parameter Not used.
/// \return A pointer to an AudioInput instance for the audio sink.
///
AudioOutput *	AudioSink(const char * parameter);

/// An ALSA audio output driver for use on Linux.
/// \param parameter The name of the sound device to open, in the form of
/// "hw:*n*", "plughw:*n*" or a "longname" as returned by
/// sound_pcm_card_get_longname() or a shortened version of that name
/// anchored at the start and with an arbitrary portion of the end removed.
/// In the case of shortened names, the first device with a longname that
/// matches the shortened name provided (anchored at the start of the longname
/// and proceeding for the length of the provided string but not beyond it)
/// will be opened.
/// \return A pointer to the AudioInput instance for the ALSA device selected.
///
AudioOutput *	AudioOutALSA(const char * parameter);

/// Opens the default audio output interface of the operating system.
///
AudioOutput *	AudioOutDefault();

/// Opens a no-op codec, which copies its input to its output unmodified,
/// for testing.
/// \param parameter Not used.
/// \return A pointer to the Codec instance for the no-op codec.
///
Codec *		CodecNoOp(const char * parameter);

/// Opens a no-op protocol framer, which copies its input to its output
/// unmodified, for testing.
/// \param parameter Not used.
/// \return A pointer to the Framer instance for the no-op codec.
///
Framer *	FramerNoOp(const char * parameter);

/// Opens a "sink" keying device, which doesn't key anything, for testing.
/// \param parameter Not used.
/// \return A pointer to the KeyingOutput instance for the device.
///
KeyingOutput *	KeyingSink(const char * parameter);

/// Opens a no-op modem, which copies its input to its output unmodified,
/// for testing.
/// \param parameter Not used.
/// \return A pointer to the Modem instance for the no-op modem.
///
Modem *		ModemNoOp(const char * parameter);

/// Opens a "constant" push-to-talk device, which has a constant value of
/// pushed or not pushed, for testing.
/// \param parameter "1" if the device is to emit the "pushed" state, "0" if
/// the device is to emit the not-pushed state.
/// \return A pointer to the PTTInput instance for the device.
///
PTTInput *	PTTConstant(const char * parameter);

/// Opens a push-to-talk device based on Linux evdev.
/// \param parameter
/// \return A pointer to the PTT_EvDev instance for the device.
///
PTTInput *	PTT_EvDev(const char * parameter);

/// Opens a "constant" text ionput device, which has a constant text value
/// which is to be transmitted as ancillary data.
/// \param parameter The text to be transmitted.
/// \return A pointer to the TextInput instance for the device.
///
TextInput *	TextConstant(const char * parameter);

/// Opens a "blank" user interface device, which has no displays or controls,
/// for testing.
/// \param parameter Not used.
/// \param interfaces a reference to an Interfaces object providing
///  information about the device and algorithm drivers to be used.
/// \return A pointer to the UserInterface instance for the device.
///
UserInterface *	BlankPanel(const char * parameter, Interfaces * interfaces);
}

// Utility functions.
//

/// Non-template version of min().
///
inline std::size_t
min(std::size_t a, std::size_t b)
{
    return a < b ? a : b;
}

/// Non-template version of max().
///
inline std::size_t
max(std::size_t a, std::size_t b)
{
    return a > b ? a : b;
}

/// Internal structure to DriverManager, used to keep a list of device
/// or protocol drivers.
struct DriverList;

/// Device driver manager. Allows for registration and enumeration of device
/// drivers. Instantiates device drivers on request.
class DriverManager {
private:
    DriverList *	audio_input_drivers;
    DriverList *	audio_output_drivers;
    DriverList *	codecs;
    DriverList *	framers;
    DriverList *	keying_output_drivers;
    DriverList *	modems;
    DriverList *	ptt_input_drivers;
    DriverList *	text_input_drivers;
    DriverList *	user_interface_drivers;

    // Copy constructor and operator=() disabled.
    DriverManager(const DriverManager &);
    DriverManager & operator=(const DriverManager &);
public:

    /// Initialize the driver manager.
    ///
    DriverManager();
    ~DriverManager();

    /// Print the available drivers to the argument stream.
    /// \param stream A reference to an instance of ostream on which the
    ///  information is to be printed.
    std::ostream &	print(std::ostream & stream) const;

    /// Instantiate an AudioInput driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    AudioInput *	audio_input(const char * driver, const char * parameters) const;
    /// Instantiate an AudioOutput driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    AudioOutput *	audio_output(const char * driver, const char * parameters) const;
    /// Instantiate a Codec.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    Codec *		codec(const char * driver, const char * parameters) const;

    /// Instantiate a Framer.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    Framer *		framer(const char * driver, const char * parameters) const;

    /// Instantiate a Keying driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    KeyingOutput *	keying_output(const char * driver, const char * parameters) const;
    /// Instantiate a softmodem.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    Modem *		modem(const char * driver, const char * parameters) const;

    /// Instantiate a PTT input driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    PTTInput *		ptt_input(const char * driver, const char * parameters) const;

    /// Instantiate a text input driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    TextInput *		text_input(const char * driver, const char * parameters) const;
    /// Instantiate a user interface driver.
    /// \param driver The name of the driver.
    /// \param parameters Driver-specific configuration parameters.
    /// \param interfaces Interfaces object used to hold all of the
    ///  current device driver instances.
    UserInterface *	user_interface(const char * driver, const char * parameters, Interfaces * interfaces) const;

    /// Register an audio input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_audio_input(const char * driver, AudioInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register an audio input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_audio_output(const char * driver, AudioOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a codec.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_codec(const char * driver, Codec * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a protocol framer.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_framer(const char * driver, Framer * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a keying output driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_keying_output(const char * driver, KeyingOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a modem driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_modem(const char * driver, Modem * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a PTT input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_ptt_input(const char * driver, PTTInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a text input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_text_input(const char * driver, TextInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a user interface driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    /// \param enumerator The coroutine that will enumerate the available
    /// driver interfaces for the user.
    void		register_user_interface(const char * driver, UserInterface * (*creator)(const char *, Interfaces *), std::ostream & (*enumerator)(std::ostream &));
};

/// Write the driver information from the DriverManager object onto a stream,
/// for debugging and dumping the configuration information.
/// \param stream A reference to an instance of ostream upon which the
///  object information is to be rendered.
/// \param d A reference to the DriverManager class providing the information.
/// \return A reference to the provided stream, meant for the
///  usual successive call paradigm of ostream operator << .
inline std::ostream &
operator << (std::ostream & stream, const DriverManager & d) {
    return d.print(stream);
}

/// Global reference to the driver manager.
///
extern DriverManager * driver_manager();
}
