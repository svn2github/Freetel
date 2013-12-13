/*
 * FreeDV driver interface definitions.
 */

#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>


namespace FreeDV {
  class AudioInput {
  protected:
	// Create an AudioInput device instance.

	// What shall we do about the audio rate?

  			AudioInput();
	virtual		~AudioInput() = 0;

  public:
	// Return true if this object is owned by a UserInterface object, in which case we should not
	// destroy it separately.
	virtual bool	captive();

	// Get the current audio level, normalized to the range of 0.0 to 1.0.
	virtual float	level() = 0;

	// Set the current audio level within the range of 0.0 to 1.0.
	virtual float	level(float value) = 0;

        // Read audio into the "short" type.
	virtual size_t	read_short(short * array, size_t length) = 0;
  };

  class AudioOutput {
  protected:
	// Create an AudioOutput device instance.

	// What shall we do about the audio rate?

  			AudioOutput();
	virtual		~AudioOutput() = 0;

  public:
	// Return true if this object is owned by a UserInterface object, in which case we should not
	// destroy it separately.
	virtual bool	captive();

	// Get the current audio level, normalized to the range of 0.0 to 1.0.
	virtual float	level() = 0;

	// Set the current audio level within the range of 0.0 to 1.0.
	virtual float	level(float value) = 0;

        // Write audio into the "short" type.
	virtual size_t	write_short(short * array, size_t length) = 0;
  };

  class Keying {
  protected:
	// Create an radio keying device instance.

  			Keying();
	virtual		~Keying() = 0;

  public:
	// If true, key the transmitter. If false, unkey.
	virtual void	key(bool value) = 0;
  };

  class PTTInput {
  protected:
	// The driver calls this member to inform FreeDV that the PTT switch value has changed.
	// the value is true for key-down, false for key-up.
	void		changed(bool value);

	// Create a push-to-talk switch instance.

  			PTTInput();
	virtual		~PTTInput() = 0;

  public:
	// Return true if this object is owned by a UserInterface object, in which case we should not
	// destroy it separately.
	virtual bool	captive();

  };

  class TextInput {
  protected:
	// The driver calls this member to set the text.
	void		set(const char * text);

	// Type for the text message source function. This is mostly used to set the text to a constant value.
	// Create a push-to-talk switch instance.

  			TextInput();
	virtual		~TextInput() = 0;

  public:
	// Return true if this object is owned by a UserInterface object, in which case we should not
	// destroy it separately.
	virtual bool	captive();

  };

  class UserInterface {
	// Generic interface to user interfaces. They may be graphical, they may be server-client interfaces,
	// they may be specialized hardware devices, especially if this software is embedded.
	// There must be inputs and callbacks for many things here.
	// UserInterfaces may provide their own drivers for microphone, loudspeaker, TextInput, and PTT.

  protected:
	// All of the callbacks from the user interface are implemented in the base class and declared here.

  				UserInterface();
	virtual			~UserInterface() = 0;

  public:
	// If this interface prodvides its own microphone driver, return it. Otherwise return 0.
	// This is guaranteed to return the same object every time, for the lifetime of this object.
	virtual AudioInput *	microphone();

	// If this interface prodvides its own loudspeaker driver, return it. Otherwise return 0.
	// This is guaranteed to return the same object every time, for the lifetime of this object.
	virtual AudioOutput *	loudspeaker();

	// If this interface prodvides its own text input driver, return it. Otherwise return 0.
	// This is guaranteed to return the same object every time, for the lifetime of this object.
	virtual TextInput *	text_input();

	// If this interface prodvides its own push-to-talk input driver, return it. Otherwise return 0.
	// This is guaranteed to return the same object every time, for the lifetime of this object.
	virtual PTTInput *	ptt_input();
  };

  class Interfaces {
    public:
   			Interfaces() : user_interface(0), keying(0), loudspeaker(0), microphone(0),
					ptt(0), receiver(0), text(0), transmitter(0), mode(0)
			{
			}

    UserInterface *	user_interface;
    Keying *		keying;
    AudioOutput *	loudspeaker;
    AudioInput *	microphone;
    PTTInput *		ptt;
    AudioInput *	receiver;
    TextInput *		text;
    AudioOutput *	transmitter;
    const char *	mode;
  };

  class DriverManager {
  private:
			std::map<std::string, AudioInput *(*)(const char *)> audio_input_drivers;
			std::map<std::string, AudioOutput *(*)(const char *)> audio_output_drivers;
			std::map<std::string, Keying *(*)(const char *)> keying_drivers;
			std::map<std::string, PTTInput *(*)(const char *)> ptt_input_drivers;
			std::map<std::string, TextInput *(*)(const char *)> text_input_drivers;
			std::map<std::string, UserInterface *(*)(const char *)> user_interface_drivers;
  public:
			DriverManager();
			~DriverManager();

        void		print(std::ostream &);

	AudioInput *	audio_input(const char * driver, const char * parameter);
	AudioOutput *	audio_output(const char * driver, const char * parameter);
	Keying *	keying(const char * driver, const char * parameter);
	PTTInput *	ptt_input(const char * driver, const char * parameter);
	TextInput *	text_input(const char * driver, const char * parameter);
	UserInterface *	user_interface(const char * driver, const char * parameter);

	void		register_audio_input(const char * driver, AudioInput * (*creator)(const char *));
	void		register_audio_output(const char * driver, AudioOutput * (*creator)(const char *));
	void		register_keying(const char * driver, Keying * (*creator)(const char *));
	void		register_ptt_input(const char * driver, PTTInput * (*creator)(const char *));
	void		register_text_input(const char * driver, TextInput * (*creator)(const char *));
	void		register_user_interface(const char * driver, UserInterface * (*creator)(const char *));
  };

  extern DriverManager & driver_manager;
  // This version has to be called from static initializers.
  extern DriverManager & init_driver_manager();
}
