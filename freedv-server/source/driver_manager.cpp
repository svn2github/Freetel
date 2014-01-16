#ifndef NO_INITIALIZERS
/*
 * Don't use DriverManager and main.cpp in space-limited applications.
 * STL stuff it uses is too large.
 */
#include <iostream>
#include "drivers.h"

using namespace std;

namespace FreeDV {
  // Global instance of the driver manager used to register
  // drivers and to create driver instances.
 
  DriverManager::DriverManager()
  {
  }

  DriverManager::~DriverManager()
  {
  }

  void
  DriverManager::print(ostream & s)
  {
    s << "AudioInput: ";
    for (auto i = audio_input_drivers.begin(); i != audio_input_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "AudioOutput: ";
    for (auto i = audio_output_drivers.begin(); i != audio_output_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "Codec: ";
    for (auto i = codecs.begin(); i != codecs.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "Keying: ";
    for (auto i = keying_output_drivers.begin(); i != keying_output_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "Modem: ";
    for (auto i = modems.begin(); i != modems.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "PTTInput: ";
    for (auto i = ptt_input_drivers.begin(); i != ptt_input_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "TextInput: ";
    for (auto i = text_input_drivers.begin(); i != text_input_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
    s << "UserInterface: ";
    for (auto i = user_interface_drivers.begin(); i != user_interface_drivers.end(); i++ )
      s << i->first << " ";
    s << endl;
  }

  AudioInput *
  DriverManager::audio_input(const char * driver, const char * parameter)
  {
    AudioInput * (* const creator)(const char * parameter) = audio_input_drivers[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  AudioOutput *
  DriverManager::audio_output(const char * driver, const char * parameter)
  {
  }
 
  Codec *
  DriverManager::codec(const char * driver, const char * parameter)
  {
    Codec * (* const creator)(const char * parameter) = codecs[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  Keying *
  DriverManager::keying_output(const char * driver, const char * parameter)
  {
    Keying * (* const creator)(const char * parameter) = keying_output_drivers[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  Modem *
  DriverManager::modem(const char * driver, const char * parameter)
  {
    Modem * (* const creator)(const char * parameter) = modems[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  PTTInput *
  DriverManager::ptt_input(const char * driver, const char * parameter)
  {
    PTTInput * (* const creator)(const char * parameter) = ptt_input_drivers[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  TextInput *
  DriverManager::text_input(const char * driver, const char * parameter)
  {
    TextInput * (* const creator)(const char * parameter) = text_input_drivers[driver];

    if(creator)
      return creator(parameter);
    else
      return 0;
  }
 
  UserInterface *
  DriverManager::user_interface(const char * driver, const char * parameter, Interfaces * interfaces)
  {
    UserInterface * (* const creator)(const char * parameters, Interfaces *) = user_interface_drivers[driver];

    if(creator)
      return creator(parameter, interfaces);
    else
      return 0;
  }

  void
  DriverManager::register_audio_input(const char * driver, AudioInput * (*creator)(const char *))
  {
    audio_input_drivers[driver] = creator;
  }

  void
  DriverManager::register_audio_output(const char * driver, AudioOutput * (*creator)(const char *))
  {
    audio_output_drivers[driver] = creator;
  }

  void
  DriverManager::register_codec(const char * driver, Codec * (*creator)(const char *))
  {
    codecs[driver] = creator;
  }

  void
  DriverManager::register_keying_output(const char * driver, Keying * (*creator)(const char *))
  {
    keying_output_drivers[driver] = creator;
  }

  void
  DriverManager::register_modem(const char * driver, Modem * (*creator)(const char *))
  {
    modems[driver] = creator;
  }

  void
  DriverManager::register_ptt_input(const char * driver, PTTInput * (*creator)(const char *))
  {
    ptt_input_drivers[driver] = creator;
  }


  void
  DriverManager::register_text_input(const char * driver, TextInput * (*creator)(const char *))
  {
    text_input_drivers[driver] = creator;
  }

  void
  DriverManager::register_user_interface(const char * driver, UserInterface * (*creator)(const char *, Interfaces *))
  {
    user_interface_drivers[driver] = creator;
  }

  // This has to be a function to get around the static initalization order problem.
  DriverManager &
  init_driver_manager()
  {
    static DriverManager manager;
    return manager;
  }

  DriverManager & driver_manager = init_driver_manager();
}
#endif
