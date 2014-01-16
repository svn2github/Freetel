/// FreeDV driver interface definitions.
#include <cstdint>


/// Namespace used for all code in this program.
namespace FreeDV {

  /// Virtual base class for audio input drivers.
  class AudioInput {
  protected:
	/// Create an AudioInput device instance.
	/// \param parameters Driver-specific configuration parameters.
  			AudioInput(const char * parameters);

	/// Destroy an AudioInput device instance.
	virtual		~AudioInput() = 0;

  public:
	/// Return true if this object is owned by a UserInterface object.
	/// In that case we should not destroy it separately.
	/// The default implementation always returns false.
	/// \return True if this object is owned by a UserInterface object.
	/// The return value is invariant for the particular object.
	virtual bool const
			captive() const;

	/// Get the current audio level.
	/// \return The current audio level.
	/// The value is normalized to the range of 0.0 to 1.0.
	virtual float	level() = 0;

	/// Set the current audio level.
	/// \param value The new value for the current audio level.
	/// The value must be normalized within the range of 0.0 to 1.0.
	virtual void	level(float value) = 0;

        /// Read audio into an array of the signed 16-bit integer type.
	virtual std::size_t
			read16(int16_t * array, std::size_t length) = 0;
  };

  /// Virtual base class for audio output drivers.
  class AudioOutput {
  protected:
	/// Create an AudioOutput device instance.
	/// \param parameters Driver-specific configuration parameters.
  			AudioOutput(const char * parameters);

	/// Destroy an AudioOutput device instance.
	virtual		~AudioOutput() = 0;

  public:
	/// Return true if this object is owned by a UserInterface object.
	/// In that case we should not destroy it separately.
	/// The default implementation always returns false.
	/// \return True if this object is owned by a UserInterface object.
	/// The return value is invariant for the particular object.
	virtual bool const
			captive() const;

	/// Get the current audio level.
	/// The value is normalized to the range of 0.0 to 1.0.
	virtual float	level() = 0;

	/// Set the current audio level.
	/// The value must be within the range of 0.0 to 1.0.
	virtual float	level(float value) = 0;

        /// Write audio from an array of the signed 16-bit integer type.
	virtual std::size_t
			write16(const int16_t * array, std::size_t length) = 0;
  };

  /// Virtual base class for codecs.
  class Codec {
  protected:
	/// Create a codec instance.
	/// \param parameters Driver-specific configuration parameters.
  			Codec(const char * parameters);

	/// Destroy a codec instance.
	virtual		~Codec() = 0;

  public:
	/// Encode from an array of the signed 16-bit integer type to an
	/// array of the unsigned 8-bit integer type (this is usually
	/// implemented as unsigned char).
	/// \param i The array of audio samples to be encoded, in an array
	/// of signed 16-bit integers.
	/// \param o The encoded data, in an array of unsigned 8-bit integers.
	/// \return The number of uint8_t elements in the encoded array.
	virtual std::size_t
			encode16(const int16_t * i, uint8_t * o, \
         		 std::size_t length) = 0;

	/// Return the size of uint8_t array necessary to encode the given
	/// number of audio samples. Sample rate is 8K samples/second.
	/// The result is invariant for a given input.
	/// \param sample_count The number of audio samples to encode.
	/// Must be a multiple of frame_size().
	/// \return The size of uint8_t array necessary to encode the given
	/// number of audio samples.
	virtual std::size_t const	
			encoded_buffer_size(std::size_t sample_count) const = 0;

	/// Return the duration of a frame in milliseconds.
	/// \return The duration of a frame in milliseconds.
	virtual int const
			frame_duration() const = 0;

	/// Return the number of audio samples necessary to decode the given
	/// encoded buffer size. Sample rate is 8K samples/second.
	/// \param buffer_size is the size of the encoded buffer. It must
	///  encode a multiple of frame_size() audio samples.
	/// \return The number of audio samples necessary to decode the given
	/// encoded buffer size. The result is invariant for a given input.
	virtual std::size_t const	
			samples_per_buffer(std::size_t buffer_size) const = 0;

	/// Return the number of audio samples expected to create a codec
	/// frame. Samples provided to encode_buffer_size must be a multiple
	/// of this value. Sample rate is 8K samples/second.
	/// The result for a particular input is invariant.
	/// \return The number of audio samples expected to create a codec
	/// frame.
	virtual std::size_t const
			samples_per_frame() const = 0;

  };

  /// Event handler class, indirects the event handler of the particular GUI
  /// software or POSIX.
  class EventHandler {
  private:
	bool		do_exit;

  protected:
	/// Bit field of status values for file descriptor events.
	/// This is an argument to the coroutine called by monitor().
	/// This is a simplification on all of the values that POSIX
	/// poll() can return. Events that aren't read or write are mapped
	/// to one of those.
	const unsigned int Read = 1;
	const unsigned int Write = 2;

	/// Create an event handler instance.
			EventHandler()
			: do_exit(false)
			{
			}
	virtual		~EventHandler() = 0;

	inline bool	get_exit() {
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

	inline void	set_exit() { do_exit = true; }
  public:
	/// Return true if this object is owned by a UserInterface object.
	/// In that case we should not destroy it separately.
	/// The default implementation always returns false.
	/// \return True if this object is owned by a UserInterface object.
	/// The return value is invariant for the particular object.
	virtual bool const
			captive() const;

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

  /// Radio device keying driver.
  class Keying {
  protected:
	/// Create an radio keying device instance.
	/// \param parameters Driver-specific configuration parameters.
  			Keying(const char * parameters);

	/// Destroy the radio keying device instance.
	virtual		~Keying() = 0;

  public:
	/// Key or un-key the transmitter.
	/// \param value If true, key the transmitter. If false, un-key.
	virtual void	key(bool value) = 0;
  };

  /// Softmodem driver.
  class Modem {
  protected:
	/// Create a modem instance.
	/// \param parameters Driver-specific configuration parameters.

  			Modem(const char * parameters);
	virtual		~Modem() = 0;

  public:
  };

  /// Push-to-talk input driver.
  class PTTInput {
  private:
	void	(*callback)(bool);

  protected:
	// The driver calls this member to inform FreeDV that the PTT switch value has changed.
	// the value is true for key-down, false for key-up.
	void		changed(bool value);

	/// Create a push-to-talk switch instance.
	/// \param parameters Driver-specific configuration parameters.

  			PTTInput(const char * parameters);
	virtual		~PTTInput() = 0;

  public:
	/// Return true if this object is owned by a
	/// UserInterface object. In that case we should
	/// not destroy it separately.
	/// The default implementation always returns false.
	/// \return True if this object is owned by a UserInterface object.
	/// The return value is invariant for the particular object.
	virtual bool const
			captive() const;

	void		set_callback(void (*value)(bool));
  };

  /// Driver for the text message source function.
  class TextInput {
  protected:
	// The driver calls this member to set the text.
	void		set(const char * text);


	/// Create a text source instance.
	/// \param parameters Driver-specific configuration parameters.

  			TextInput(const char * parameters);
	virtual		~TextInput() = 0;

  public:
	/// Return true if this object is owned by a
	/// UserInterface object. In that case we should
	/// not destroy it separately.
	/// The default implementation always returns false.
	/// \return True if this object is owned by a UserInterface object.
	/// The return value is invariant for the particular object.
	virtual bool const
			captive() const;
  };

  class Interfaces;

  /// Generic base class for user interfaces.
  /// They may be graphical, they may be server-client interfaces,
  /// they may be specialized hardware devices, especially if this
  /// software is embedded.
  /// There must be inputs and callbacks for many things here.
  /// UserInterfaces may provide their own drivers for microphone,
  /// loudspeaker, TextInput, both forms of PTT, and EventHandler.
  class UserInterface {
  protected:
	/// Create an instance of the UserInterface object.
	/// \param parameters Driver-specific configuration parameters.
	/// \param interfaces An Interface object. The UserInterface object
	/// may set various fields of Interface to be its own captive driver
	/// objects, and they may change during operation if the user changes
	/// device driver parameters.
  				UserInterface(const char * parameters, Interfaces * interfaces);
	virtual			~UserInterface() = 0;

  public:
  };

  /// Structure used to pass all of the drivers. Can be modified from
  /// UserInterface while within the event loop, if a user changes a device
  /// from the GUI.
  class Interfaces {
    public:
   			Interfaces() : codec(0), event_handler(0),
			 keying_output(0), loudspeaker(0), microphone(0),
			 modem(0), ptt_input_digital(0), ptt_input_ssb(0),
                         receiver(0), text(0), transmitter(0),
			 user_interface(0)
			{
			}

    Codec *		codec;
    EventHandler *	event_handler;
    Keying *		keying_output;
    AudioOutput *	loudspeaker;
    AudioInput *	microphone;
    Modem *		modem;
    PTTInput *		ptt_input_digital;
    PTTInput *		ptt_input_ssb;
    TextInput *		text;
    AudioOutput *	transmitter;
    AudioInput *	receiver;
    UserInterface *	user_interface;
  };
}

#ifndef NO_INITIALIZERS
#include <iostream>
#include <map>
#include <string>

/// Namespace used for the entire program.
namespace FreeDV {
  /// Device driver manager. Allows for registration and enumeration of device
  /// drivers. Instantiates device drivers on request.
  class DriverManager {
  private:
			std::map<std::string, AudioInput *(*)(const char *)> audio_input_drivers;
			std::map<std::string, AudioOutput *(*)(const char *)> audio_output_drivers;
			std::map<std::string, Codec *(*)(const char *)> codecs;
			std::map<std::string, Keying *(*)(const char *)> keying_output_drivers;
			std::map<std::string, Modem *(*)(const char *)> modems;
			std::map<std::string, PTTInput *(*)(const char *)> ptt_input_drivers;
			std::map<std::string, TextInput *(*)(const char *)> text_input_drivers;
			std::map<std::string, UserInterface *(*)(const char *, Interfaces *)> user_interface_drivers;
  public:
			DriverManager();
			~DriverManager();

        void		print(std::ostream &);

	AudioInput *	audio_input(const char * driver, const char * parameter);
	AudioOutput *	audio_output(const char * driver, const char * parameter);
	Codec *		codec(const char * driver, const char * parameter);
	Keying *	keying_output(const char * driver, const char * parameter);
	Modem *		modem(const char * driver, const char * parameter);
	PTTInput *	ptt_input(const char * driver, const char * parameter);
	TextInput *	text_input(const char * driver, const char * parameter);
	UserInterface *	user_interface(const char * driver, const char * parameter, Interfaces * interfaces);

	void		register_audio_input(const char * driver, AudioInput * (*creator)(const char *));
	void		register_audio_output(const char * driver, AudioOutput * (*creator)(const char *));
	void		register_codec(const char * driver, Codec * (*creator)(const char *));
	void		register_keying_output(const char * driver, Keying * (*creator)(const char *));
	void		register_modem(const char * driver, Modem * (*creator)(const char *));
	void		register_ptt_input(const char * driver, PTTInput * (*creator)(const char *));
	void		register_text_input(const char * driver, TextInput * (*creator)(const char *));
	void		register_user_interface(const char * driver, UserInterface * (*creator)(const char *, Interfaces *));
  };

  extern DriverManager & driver_manager;
  // This version has to be called from static initializers.
  extern DriverManager & init_driver_manager();
#endif
}
