/// FreeDV driver interface definitions.
#include <cstdint>
#include <iostream>
#include <assert.h>

/// Namespace used for all code in this program.
namespace FreeDV {

  /// The sample rate used by all audio interfaces in the program.
  /// the sound cards are in general driven at 48000 because that's
  /// the only reliable sample rate they all have in common. SampleRate
  /// may be lower than that and thus there may be resampling in the
  /// drivers.
  const unsigned int	SampleRate = 8000;

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

    void	out_overrun() const;
    uint8_t *	reorder(std::size_t length);

  public:
    /// Create the FIFO object.
    /// \param length The size of the fifo, in bytes.
			FIFO(std::size_t length);

			~FIFO();

    /// Returns the amount of space available for incoming data.
    /// \return The amount of space, in bytes, available for incoming data.
    inline std::size_t	incoming_available() const {
			  return (buffer_end) - in + (out - buffer);
			}

    /// Return the address of an incoming data buffer of the requested size.
    /// Throws an error if we run the buffer out of space. Well-behaved code
    /// won't allocate a size that can't be drained before it is further
    /// written.
    /// You must call incoming_done(length) when the I/O is completed.
    /// The length passed to incoming_done() must be smaller than or equal
    /// to the length passed to incoming_buffer().
    /// \param io_length The size of buffer in chars requested.
    /// \return The address of the buffer for incoming data.
    inline uint8_t *	incoming_buffer(std::size_t io_length) {
			  const uint8_t * io_end = in + io_length;

			  if ( io_end > buffer_end )
                            return reorder(io_length);
			  else
			    return in;
			}

    /// Complete the I/O after incoming_buffer().
    /// \param length The amount of data actually written. This must be
    /// smaller than or equal to the length passed to incoming_buffer().
    inline void		incoming_done(std::size_t length) {
			  in += length;
 			  assert(in >= buffer && in <= buffer_end);
			}

    /// Returns the amount of data available to read.
    /// \return The amount of data, in bytes, available to read.
    inline std::size_t	outgoing_available() const {
			  return in - out;
			}

    /// Return the address of output data of the requested length.
    /// \param length The amount of data requested. This must be smaller
    /// than or equal to the amount returned by outgoing_available().
    /// \return The address of the data to be read.
    inline const uint8_t *	outgoing_buffer(std::size_t length) {
			  if ( length > (std::size_t)(in - out) )
			    out_overrun();
			  return out;
			}

    /// Finish the I/O after outgoing_buffer().
    /// \param length The amount of data, in bytes, actually read.
    /// This must be smaller than or equal to the amount passed to
    /// outgoing_buffer().
    inline void		outgoing_done(std::size_t length) {
			  out += length;
			  assert(out >= buffer && out <= buffer_end);
			}

    /// Discard any buffered data.
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
  void		check_privileges();

  /// Virtual base class for all driver classes.
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
	std::ostream &	print(std::ostream &) const;
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
    virtual std::size_t	ready() = 0;

    virtual		~IODevice() = 0;
  };

  /// Virtual base class for AudioInput and AudioOutput.
  class AudioDevice : public ::FreeDV::IODevice {
  protected:
    /// The master volume control for the device.
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
  };

  /// Virtual base class for audio input drivers.
  class AudioInput : public ::FreeDV::AudioDevice {
  protected:
    /// Create an AudioInput device instance.
    /// \param name The name of the child driver. This is expected to be a
    /// static string.
    /// \param parameters Driver-specific configuration parameters.
      			AudioInput(const char * name, const char * parameters);

  public:
    /// Destroy an AudioInput device instance.
    virtual		~AudioInput() = 0;

        /// Read audio into an array of the signed 16-bit integer type.
    virtual std::size_t
    			read16(std::int16_t * array, std::size_t length) = 0;
  };

  /// Virtual base class for audio output drivers.
  class AudioOutput : public ::FreeDV::AudioDevice {
  protected:
    /// Create an AudioOutput device instance.
    /// \param name The name of the child driver. This is expected to be a
    /// static string.
    /// \param parameters Driver-specific configuration parameters.
  			AudioOutput(const char * name, const char * parameters);

  public:
    /// Destroy an AudioOutput device instance.
    virtual		~AudioOutput() = 0;

        /// Write audio from an array of the signed 16-bit integer type.
    virtual std::size_t
    			write16(const std::int16_t * array, std::size_t length) = 0;
  };

  /// Virtual base class for codecs.
  class Codec : public ::FreeDV::Base {
  protected:
    /// Create a codec instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
      			Codec(const char * name, const char * parameters);

  public:
    /// Destroy a codec instance.
    virtual		~Codec() = 0;

    /// Return the number of data bytes that store a single codec frame.
    /// Data Bytes provided to decode16 and encode16 must be a multiple
    /// of this value. The result is invariant for a particular configuration.
    /// \return The number of data bytes necessary to store a codec frame.
    virtual std::size_t
    			bytes_per_frame() const = 0;

    /// Decode from data bytes to audio samples.
    /// \param i The encoded data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after decoding, in an array
    /// of signed 16-bit integers.
    /// \param data_length When called: The number of bytes of data that are
    /// available to be decoded. On return: the number of bytes of data
    /// that were consumed.
    /// \param sample_length The number of audio samples that may be decoded.
    /// \return The number of audio samples that were actually decoded.
    virtual std::size_t
    			decode16(const std::uint8_t * i,
			 std::int16_t * o,
             		 std::size_t * data_length,
			 std::size_t sample_length) = 0;


    /// Encode from audio samples to data bytes.
    /// \param i The array of audio samples to be encoded, in an array
    /// of signed 16-bit integers.
    /// \param o The encoded data, in an array of unsigned 8-bit integers.
    /// \param length The number of audio samples to be encoded.
    /// \return The number of std::uint8_t elements in the encoded array.
    virtual std::size_t
    			encode16(const std::int16_t * i, std::uint8_t * o,
             		 std::size_t length) = 0;

    /// Return the duration of a frame in milliseconds.
    /// \return The duration of a frame in milliseconds.
    virtual int
    			frame_duration() const = 0;

    /// Return the number of audio samples expected to create a codec
    /// frame at SampleRate. The result is invariant for a given SampleRate.
    /// This number will be accurate for transmitted data, but not for received
    /// data because of the potential for clock skew between stations.
    /// \return The number of audio samples expected to create a codec
    /// frame.
    virtual std::size_t
    			samples_per_frame() const = 0;
  };

  /// Event handler class, indirects the event handler of the particular GUI
  /// software or POSIX.
  class EventHandler : public ::FreeDV::Base {
  private:
    bool		do_exit;

  protected:
    /// Bit field of status values for file descriptor events.
    /// This is an argument to the coroutine called by monitor().
    /// This is a simplification on all of the values that POSIX
    /// poll() can return. Events that aren't read or write are mapped
    /// to one of those.

    /// File being monitored is readable or has read error.
    const unsigned int	Read = 1;

    /// File being monitored is writable or has write error.
    const unsigned int	Write = 2;

    /// Create an event handler instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
    			EventHandler(const char * name, const char * parameters)
    			: Base(name, parameters), do_exit(false)
    			{
    			}

    /// If set_exit() has been called, return true once.
    /// \return True if set_exit has been called. The next and subsequent
    ///  calls will return false until set_exit() is called again.
    inline bool		get_exit() {
    		  	  if ( do_exit ) {
    		    	  do_exit = false;
    		    	  return true;
    		  	}
    		  	else
    		    	  return false;
    			}

    /// Run one iteration of the event handler.
    /// The default implementation throws std::runtime_error.
    ///
    /// If iterate() is not implemented in the child class,
    /// the child class must implement loop(), and the child class
    /// implementation of loop() must not call iterate().
    void		iterate();

    /// Cause get_exit() to return true the next time it is called.
    inline void		set_exit() { do_exit = true; }
  public:
	virtual		~EventHandler() = 0;

	/// Run the event loop.
	/// The default implementation iterates checking get_exit(), returning
	/// if its value is true and otherwise and calling iterate().
	/// If you provide your own implementation of loop(), you must check
	/// get_exit() and return from this method if its value is true.
	/// If you provide your own implementation of loop(), it's your choice
	/// whether or not to implement and call iterate().
	void		loop();

	/// Monitor a file descriptor in the event loop. Call a function if the
	/// file descriptor is ready for I/O.
	/// \param fd The file descriptor to monitor.
	/// \param type A bit-field of values defined in this class,
	///  indicating the kinds of events to listen for.
	/// \param private_data Private data to be passed to the event
	///  function.
	/// \param event A coroutine to call when there is a status change
	///  on the file descriptor. The arguments of the coroutine are
	///  - fd: The file descriptor that has an event.
	///  - type: A bit-field of FDStatus values indicating the events
	///    received.
	///  - private: The address of opaque data to be passed to the driver.
	virtual void	monitor(int fd, unsigned int type, void * private_data,
			 void (*event)(int fd, unsigned int type, void * private_data)
			 ) = 0;

	/// Remove all monitoring of the given file descriptor by the event
	/// loop handler.
	/// \param fd The file descriptor to be removed from monitoring.
	virtual void	unmonitor(int fd) = 0;
  };

  /// Virtual base class for protocol framers.
  class Framer : public ::FreeDV::Base {
  protected:
    /// Create a framer instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
      			Framer(const char * name, const char * parameters);

  public:
    /// Destroy a framer instance.
    virtual		~Framer() = 0;

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

    /// Return the maximum number of data bytes expected to store the unwrapped
    /// codec data. The result is invariant for a particular configuration.
    /// \return The maximum number of data bytes expected to store the unwrapped
    /// codec data.
    /// frame.
    virtual std::size_t
    			max_unwrapped_bytes_per_frame() const = 0;

    /// Return the minimum number of data bytes expected to store the unwrapped
    /// codec data. The result is invariant for a particular configuration.
    /// \return The minimum number of data bytes expected to store the unwrapped
    /// codec data.
    /// frame.
    virtual std::size_t
    			min_unwrapped_bytes_per_frame() const = 0;

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

    /// Return the maximum number of data bytes expected to store a wrapped
    /// protocol frame. The result is invariant for a particular configuration
    /// which may include such things as length of addresses and protocol
    /// options.
    /// \return The maximum number of data bytes expected to store a wrapped
    /// protocol frame.
    /// frame.
    virtual std::size_t
    			max_wrapped_bytes_per_frame() const = 0;

    /// Return the minimum number of data bytes expected to store a wrapped
    /// protocol frame. The result is invariant for a particular configuration
    /// which may include such things as length of addresses and protocol
    /// options.
    /// \return The minimum number of data bytes expected to store a wrapped
    /// protocol frame.
    /// frame.
    virtual std::size_t
    			min_wrapped_bytes_per_frame() const = 0;
  };

  /// Radio device keying driver.
  class KeyingOutput : public ::FreeDV::Base {
  protected:
    /// Create an radio keying output device instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
  			KeyingOutput(const char * name, const char * parameters);

  public:
    /// Destroy the radio keying device instance.
    virtual		~KeyingOutput() = 0;

    /// Key or un-key the transmitter.
    /// \param value If true, key the transmitter. If false, un-key.
    virtual void	key(bool value) = 0;

    /// Return the amount of bytes ready to write.
    virtual std::size_t	ready() = 0;
  };

  /// Softmodem driver.
  class Modem : public ::FreeDV::Base {
  protected:
    /// Create a softmodem device instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
  			Modem(const char * name, const char * parameters);

  public:
    virtual		~Modem() = 0;

    /// Return the number of data bytes output in a single modem frame.
    /// The data buffer provided to demodulate16 must be a multiple of
    /// this value. The result is invariant.
    /// \return The number of data bytes necessary to store a modem frame.
    virtual std::size_t
    			bytes_per_frame() const = 0;

    /// Demodulate from audio samples to data.
    /// \param i The array of audio samples to be demodulated, in an array
    /// of signed 16-bit integers.
    /// \param o The demodulated data, in an array of unsigned 8-bit integers.
    /// \param sample_length On call: The number of audio samples to be
    /// demodulated. On return: The number of audio samples consumed.
    /// \param data_length The number of bytes of data that may be demodulated.
    /// \return The number of bytes of data that were actually decoded.
    virtual std::size_t
    			demodulate16(
			 const std::int16_t * i,
			 std::uint8_t * o,
             		 std::size_t * sample_length,
			 std::size_t data_length) = 0;

    /// Modulate from data to audio samples.
    /// \param i The data, in an array of unsigned 8-bit integers.
    /// \param o The array of audio samples after modulation, in an array
    /// of signed 16-bit integers.
    /// \param length The number of bytes of data to be modulated.
    /// \return The number of std::int16_t elements in the modulated array.
    virtual std::size_t
    			modulate16(const std::uint8_t * i, std::int16_t * o,
             		 std::size_t length) = 0;

    /// Return the duration of a frame in milliseconds.
    /// \return The duration of a frame in milliseconds.
    virtual int
    			frame_duration() const = 0;

    /// Return the number of audio samples expected to create a modem
    /// frame at SampleRate. The result is invariant for a given SampleRate.
    /// The result will be accurate for transmitted data, but not for received
    /// data due to the potential for clock skew between stations.
    /// \return The number of audio samples expected to create a codec
    /// frame.
    virtual std::size_t
    			samples_per_frame() const = 0;
  };

  /// Push-to-talk input driver.
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
  class TextInput : public ::FreeDV::IODevice {
  protected:
    /// The child class calls this member in its parent to set the text.
    void		set(const char * text);


    /// Create a text source instance.
    /// \param name Name of the driver. This is expected to be a single
    ///  constant static string per driver class.
    /// \param parameters Driver-specific configuration parameters.
  			TextInput(const char * name, const char * parameters);

  public:
    /// Read the text data.
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
  /// loudspeaker, TextInput, both forms of PTT, and EventHandler.
  class UserInterface : public ::FreeDV::IODevice {
  protected:
    /// The external Interfaces object.
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

  /// Structure used to pass all of the drivers. Can be modified from
  class Interfaces {
    public:
			Interfaces() : codec(0), event_handler(0),
			 framer(0), keying_output(0), loudspeaker(0),
			 microphone(0), modem(0), ptt_input_digital(0),
			 ptt_input_ssb(0), receiver(0), text_input(0),
			 transmitter(0), user_interface(0)
			{
			}

    virtual		~Interfaces() final;

    /// The voice codec in use.
    Codec *		codec;
    /// The event loop handler. This is specific to a GUI, or POSIX.
    EventHandler *	event_handler;
    /// The Framer handles the protocol which wraps the codec data.
    /// It can decline to feed any audio on to the codec if the protocol says
    /// that should not happen, for example if the data isn't addressed to us.
    Framer * 		framer;
    /// The output used to key the transmitter.
    KeyingOutput *	keying_output;
    /// The audio output which drives the loudspeaker or headphone.
    AudioOutput *	loudspeaker;
    /// The audio input from the microphone.
    AudioInput *	microphone;
    /// The softmodem.
    Modem *		modem;
    /// The PTT input that indicates the transmission is to be digital audio.
    PTTInput *		ptt_input_digital;
    /// The PTT input that indicates the transmission is to be SSB.
    PTTInput *		ptt_input_ssb;
    /// The audio input from the receiver.
    AudioInput *	receiver;
    /// The text to be transmitted in our text side-channel.
    TextInput *		text_input;
    /// The audio output that drives the transmitter.
    AudioOutput *	transmitter;
    /// The user interface driver. Used for GUIs.
    UserInterface *	user_interface;

    /// Fill in default drivers if the user or UserInterface hasn't set any.
    void		fill_in();

    /// Write the command-line flags necessary to configure the drivers as 
    /// they are presently configured to the stream. This is used to save
    /// the configuration or debug the program.
    /// \param stream A reference to an instance of ostream on which the
    /// \return A reference to the provided stream, meant for the
    ///  usual successive call paradigm of ostream operator << .
    virtual std::ostream &
    			print(std::ostream & stream) const;
  };

  /// Write the driver information from the Interfaces object onto a stream,
  /// for debugging and dumping the configuration information.
  /// \param stream A reference to an instance of ostream upon which the
  ///  object information is to be rendered.
  /// \param interfaces a reference to an Interfaces object providing the
  ///  information.
  /// \return A reference to the provided stream, meant for the
  ///  usual successive call paradigm of ostream operator << .
  inline std::ostream &
  operator << (std::ostream & stream, const Interfaces & interfaces) {
    return interfaces.print(stream);
  }

  // Most of the functions in the Driver and Enumerator namespaces are
  // registered with the driver manager at run-time. There won't be many
  // reasons to reference them directly.
  namespace Driver {
    AudioInput *	Tone(const char * parameter);
    AudioInput *	AudioInALSA(const char * parameter);
    AudioInput *	AudioInDefault();
    AudioOutput *	AudioSink(const char * parameter);
    AudioOutput *	AudioOutALSA(const char * parameter);
    AudioOutput *	AudioOutDefault();
    Codec *		CodecNoOp(const char * parameter);
    Framer *		FramerNoOp(const char * parameter);
    KeyingOutput *	KeyingSink(const char * parameter);
    EventHandler *	LibEvent(const char * parameter);
    Modem *		ModemNoOp(const char * parameter);
    PTTInput *		PTTConstant(const char * parameter);
    TextInput *		TextConstant(const char * parameter);
    UserInterface *	BlankPanel(const char * parameter, Interfaces *);
  }

  namespace Enumerator {
    std::ostream &	Tone(std::ostream &);
    std::ostream &	AudioSink(std::ostream &);
    std::ostream &	AudioInALSA(std::ostream &);
    std::ostream &	AudioOutALSA(std::ostream &);
    std::ostream &	CodecNoOp(std::ostream &);
    std::ostream &	FramerNoOp(std::ostream &);
    std::ostream &	KeyingSink(std::ostream &);
    std::ostream &	LibEvent(std::ostream &);
    std::ostream &	ModemNoOp(std::ostream &);
    std::ostream &	PTTConstant(std::ostream &);
    std::ostream &	TextConstant(std::ostream &);
    std::ostream &	BlankPanel(std::ostream &);
  }
}

/// Namespace used for the entire program.
namespace FreeDV {
  /// Utility functions.
  
  /// Non-template version of min().
  inline std::size_t
  min(std::size_t a, std::size_t b)
  {
    return a < b ? a : b;
  }

  struct DriverList {
    const char *	key;
    union {
      FreeDV::Base *	(*creator)(const char *);
      FreeDV::Base *	(*creator_i)(const char *, Interfaces *);
    };
    std::ostream &	(*enumerator)(std::ostream &);
  };

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

  public:

    /// Initialize the driver manager.
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
    void		register_audio_input(const char * driver, AudioInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register an audio input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_audio_output(const char * driver, AudioOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a codec.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_codec(const char * driver, Codec * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a protocol framer.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_framer(const char * driver, Framer * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a keying output driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_keying_output(const char * driver, KeyingOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a modem driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_modem(const char * driver, Modem * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a PTT input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_ptt_input(const char * driver, PTTInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a text input driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
    void		register_text_input(const char * driver, TextInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &));

    /// Register a user interface driver.
    /// \param driver The name of the driver.
    /// \param creator The coroutine that will instantiate the driver.
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
  extern DriverManager * driver_manager();
}
