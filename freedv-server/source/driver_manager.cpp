/// \file driver_manager.cpp
/// Device driver manager.
/// Don't use DriverManager and big_main.cpp in space-limited applications.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include <iostream>
#include <string.h>
#include "drivers.h"

namespace FreeDV {
  /// Creator functions passed to the various driver registration functions
  /// will be cast to this value.
  typedef Base * (*base_creator)(const char *);

  /// Internal structure which holds a list of device drivers. 
  ///
  struct DriverList {
      /// The name of the driver.
      ///
      const char *	key;
      union {
	  /// Instantiate a device driver.
          ///
          FreeDV::Base *	(*creator)(const char *);
          FreeDV::Base *	(*creator_i)(const char *, Interfaces *);
      };
      /// Enumerate the available device drivers.
      ///
      std::ostream &		(*enumerator)(std::ostream &);
  };

  static std::ostream &
  enumerate(std::ostream & stream, const char * name, const DriverList * list)
  {
    if ( list == 0 )
      return stream;

    stream << "# " << name << std::endl;

    while ( list->key ) {
      (*(list->enumerator))(stream);
      list++;
    }
    return stream;
  }

  // Ad-hoc list management functions are much smaller, and indeed faster for
  // our tiny data sets, than the STL containers. Since this program is
  // embedded, it's worth some extra effort to avoid STL containers.
  static FreeDV::Base *
  pick(const char * key, const char * type, const char * const parameters, const DriverList * list, Interfaces * interfaces = 0)
  {
    while ( list->key ) {
      if ( strcmp(key, list->key) == 0 ) {
	try {
	  if ( interfaces )
            return (*(list->creator_i))(parameters, interfaces);
          else
	    return (*(list->creator))(parameters);
        }
        catch(std::exception & e) {
          std::cerr << "Open " << type << " \"" << key << ':' << parameters << "\": " << e.what() << std::endl;
          return 0;
        }
      }
      list++;
    }
    std::cerr << "No " << type << " named \"" << key << "\"." << std::endl;
    return 0;
  }

  static void
  place(
   const char * const key,
   FreeDV::Base * (*creator)(const char *),
   std::ostream & (*enumerator)(std::ostream &),
   DriverList * * const list)
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
    next->creator = creator;
    next->enumerator = enumerator;
    ++next;
    next->key = 0;
    next->creator = 0;
    next->enumerator = 0;
  }

  // Global instance of the driver manager used to register
  // drivers and to create driver instances.
  DriverManager::DriverManager()
  : audio_input_drivers(0),
    audio_output_drivers(0),
    codecs(0),
    framers(0),
    keying_output_drivers(0),
    modems(0),
    ptt_input_drivers(0),
    text_input_drivers(0),
    user_interface_drivers(0)
  {
  }

  DriverManager::~DriverManager()
  {
  }

  std::ostream &
  DriverManager::print(std::ostream & s) const
  {
    enumerate(s, "Audio Input (--receiver, --microphone)", audio_input_drivers)
     << std::endl;
    enumerate(s, "Audio Output (--transmitter, --loudspeaker)",
     audio_output_drivers)
     << std::endl;
    enumerate(s, "Codec (--codec)", codecs) << std::endl;
    enumerate(s, "Protocol Framer (--framer)", framers) << std::endl;
    enumerate(s, "Keying Output (--keying)", keying_output_drivers)
     << std::endl;
    enumerate(s, "Modem (--modem)", modems) << std::endl;
    enumerate(s, "PTT Input (--ptt-digital, --ptt-ssb)", ptt_input_drivers)
     << std::endl;
    enumerate(s, "Text Input (--text)", text_input_drivers) << std::endl;
    enumerate(s, "User Interface (--gui)", user_interface_drivers) << std::endl;

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
    return (UserInterface *)pick(key, "user interface", parameter, user_interface_drivers, interfaces);
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

  DriverManager *
  driver_manager()
  {
    extern DriverManager * _globalDriverManager;
    if ( _globalDriverManager == 0 )
      _globalDriverManager = new DriverManager();

    return _globalDriverManager;
  }
}
