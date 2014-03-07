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
    while ( list->key ) {
      stream << list->key << ' ';
      list++;
    }
    return stream;
  }

  static FreeDV::Base *
  pick(const char * key, const char * parameters, const DriverList * list)
  {
    while ( list->key ) {
      if ( strcmp(key, list->key) == 0 ) {
        return (*(list->value))(parameters);
      }
      list++;
    }
    return 0;
  }

  static void
  place(const char * key, FreeDV::Base * (*value)(const char *), DriverList * * const list)
  {
    DriverList * next;

    if ( list ) {
      next = *list;
      while ( next->key )
	next++;
      const std::size_t length = (next - *list);

      DriverList * n = new DriverList[length + 2];
      memcpy(n, *list, sizeof(*list) * length);
      *list = n;
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
  DriverManager::audio_input(const char * driver, const char * parameter)
  {
    return (AudioInput *)pick(driver, parameter, audio_input_drivers);
  }
 
  AudioOutput *
  DriverManager::audio_output(const char * driver, const char * parameter)
  {
    return (AudioOutput *)pick(driver, parameter, audio_output_drivers);
  }
 
  Codec *
  DriverManager::codec(const char * driver, const char * parameter)
  {
    return (Codec *)pick(driver, parameter, codecs);
  }
 
  Framer *
  DriverManager::framer(const char * driver, const char * parameter)
  {
    return (Framer *)pick(driver, parameter, framers);
  }
 
  KeyingOutput *
  DriverManager::keying_output(const char * driver, const char * parameter)
  {
    return (KeyingOutput *)pick(driver, parameter, keying_output_drivers);
  }
 
  Modem *
  DriverManager::modem(const char * driver, const char * parameter)
  {
    return (Modem *)pick(driver, parameter, modems);
  }
 
  PTTInput *
  DriverManager::ptt_input(const char * driver, const char * parameter)
  {
    return (PTTInput *)pick(driver, parameter, ptt_input_drivers);
  }
 
  TextInput *
  DriverManager::text_input(const char * driver, const char * parameter)
  {
    return (TextInput *)pick(driver, parameter, text_input_drivers);
  }
 
  UserInterface *
  DriverManager::user_interface(const char * driver, const char * parameter, Interfaces * interfaces)
  {
    return (UserInterface *)pick(driver, parameter, user_interface_drivers);
  }

  void
  DriverManager::register_audio_input(const char * driver, AudioInput * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &audio_input_drivers);
  }

  void
  DriverManager::register_audio_output(const char * driver, AudioOutput * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &audio_output_drivers);
  }

  void
  DriverManager::register_codec(const char * driver, Codec * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &codecs);
  }

  void
  DriverManager::register_framer(const char * driver, Framer * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &framers);
  }

  void
  DriverManager::register_keying_output(const char * driver, KeyingOutput * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &keying_output_drivers);
  }

  void
  DriverManager::register_modem(const char * driver, Modem * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &modems);
  }

  void
  DriverManager::register_ptt_input(const char * driver, PTTInput * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &ptt_input_drivers);
  }


  void
  DriverManager::register_text_input(const char * driver, TextInput * (*creator)(const char *))
  {
    place(driver, (base_creator)creator, &text_input_drivers);
  }

  void
  DriverManager::register_user_interface(const char * driver, UserInterface * (*creator)(const char *, Interfaces *))
  {
    place(driver, (base_creator)creator, &user_interface_drivers);
  }

  /// Automatic initializer for the driver manager.
  /// This has to be a function to get around the static initalization order
  /// problem.
  DriverManager &
  init_driver_manager()
  {
    static DriverManager manager;
    return manager;
  }

  /// Global reference to the driver manager instance.
  DriverManager & driver_manager = init_driver_manager();
}
