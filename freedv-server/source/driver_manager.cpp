/// Device driver manager.
/// Don't use DriverManager and big_main.cpp in space-limited applications.
/// The STL templates it uses are too large.

#include <iostream>
#include <string.h>
#include "drivers.h"

namespace FreeDV {
  typedef Base * (*base_creator)(const char *);

  // Ad-hoc list management functions are much smaller, and indeed faster for
  // our tiny data sets, than the STL containers. Since this program is
  // embedded, it's worth some extra effort to avoid STL containers.

  static std::ostream &
  enumerate(std::ostream & stream, const DriverList * list)
  {
    if ( list == 0 )
      return stream;

    while ( list->key ) {
      stream << list->key << ' ';
      list++;
    }
    return stream;
  }

  static FreeDV::Base *
  pick(const char * key, const char * type, const char * const parameters, const DriverList * list)
  {
    while ( list->key ) {
      if ( strcmp(key, list->key) == 0 ) {
	try {
          return (*(list->value))(parameters);
        }
        catch(std::exception & e) {
          std::cerr << "Open " << type << " \"" << key << "\": " << e.what() << std::endl;
          return 0;
        }
      }
      list++;
    }
    std::cerr << "No " << type << " named \"" << key << "\"." << std::endl;
    return 0;
  }

  static void
  place(const char * const key, FreeDV::Base * (*value)(const char *), std::ostream & (*enumerator)(std::ostream &), DriverList * * const list)
  {
    DriverList * next;

    if ( *list ) {
      DriverList * l;
      l = *list;
      while ( l->key )
	l++;

      const std::size_t length = (l - *list);

      DriverList * const n = new DriverList[length + 2];
      memcpy(n, *list, sizeof(*n) * length);
      *list = n;
      next = &n[length];
    }
    else
      *list = next = new DriverList[2];

    next->key = strdup(key);
    next->value = value;
    ++next;
    next->key = 0;
    next->value = 0;
  }

  // Global instance of the driver manager used to register
  // drivers and to create driver instances.
  DriverManager::DriverManager()
  {
  }

  DriverManager::~DriverManager()
  {
  }

  std::ostream &
  DriverManager::print(std::ostream & s) const
  {
    s << "Audio Input: ";
    enumerate(s, audio_input_drivers) << std::endl;

    s << "Audio Output: ";
    enumerate(s, audio_output_drivers) << std::endl;

    s << "Codec: ";
    enumerate(s, codecs) << std::endl;

    s << "Framer: ";
    enumerate(s, framers) << std::endl;

    s << "Keying Output: ";
    enumerate(s, keying_output_drivers) << std::endl;

    s << "Modem: ";
    enumerate(s, modems) << std::endl;

    s << "PTT Input: ";
    enumerate(s, ptt_input_drivers) << std::endl;

    s << "Text Input: ";
    enumerate(s, text_input_drivers) << std::endl;

    s << "User Interface: ";
    enumerate(s, user_interface_drivers) << std::endl;

    return s;
  }

  AudioInput *
  DriverManager::audio_input(const char * key, const char * parameter) const
  {
    return (AudioInput *)pick(key, "audio input", parameter, audio_input_drivers);
  }
 
  AudioOutput *
  DriverManager::audio_output(const char * key, const char * parameter) const
  {
    return (AudioOutput *)pick(key, "audio output", parameter, audio_output_drivers);
  }
 
  Codec *
  DriverManager::codec(const char * key, const char * parameter) const
  {
    return (Codec *)pick(key, "codec", parameter, codecs);
  }
 
  Framer *
  DriverManager::framer(const char * key, const char * parameter) const
  {
    return (Framer *)pick(key, "protocol framer", parameter, framers);
  }
 
  KeyingOutput *
  DriverManager::keying_output(const char * key, const char * parameter) const
  {
    return (KeyingOutput *)pick(key, "keying output", parameter, keying_output_drivers);
  }
 
  Modem *
  DriverManager::modem(const char * key, const char * parameter) const
  {
    return (Modem *)pick(key, "modem", parameter, modems);
  }
 
  PTTInput *
  DriverManager::ptt_input(const char * key, const char * parameter) const
  {
    return (PTTInput *)pick(key, "PTT input", parameter, ptt_input_drivers);
  }
 
  TextInput *
  DriverManager::text_input(const char * key, const char * parameter) const
  {
    return (TextInput *)pick(key, "text input", parameter, text_input_drivers);
  }
 
  UserInterface *
  DriverManager::user_interface(const char * key, const char * parameter, Interfaces * interfaces) const
  {
    return (UserInterface *)pick(key, "user interface", parameter, user_interface_drivers);
  }

  void
  DriverManager::register_audio_input(const char * key, AudioInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &audio_input_drivers);
  }

  void
  DriverManager::register_audio_output(const char * key, AudioOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &audio_output_drivers);
  }

  void
  DriverManager::register_codec(const char * key, Codec * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &codecs);
  }

  void
  DriverManager::register_framer(const char * key, Framer * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &framers);
  }

  void
  DriverManager::register_keying_output(const char * key, KeyingOutput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &keying_output_drivers);
  }

  void
  DriverManager::register_modem(const char * key, Modem * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &modems);
  }

  void
  DriverManager::register_ptt_input(const char * key, PTTInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &ptt_input_drivers);
  }


  void
  DriverManager::register_text_input(const char * key, TextInput * (*creator)(const char *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &text_input_drivers);
  }

  void
  DriverManager::register_user_interface(const char * key, UserInterface * (*creator)(const char *, Interfaces *), std::ostream & (*enumerator)(std::ostream &))
  {
    place(key, (base_creator)creator, enumerator, &user_interface_drivers);
  }

  DriverManager * const
  driver_manager()
  {
    static DriverManager * const d(new DriverManager());
    return d;
  }
}
