#include <iostream>
#include "drivers.h"

using namespace std;

namespace FreeDV {
  // Global instance of the driver manager used to register
  // drivers and to create driver instances.
  DriverManager	driver_manager;
 
  DriverManager::DriverManager()
  {
  }

  DriverManager::~DriverManager()
  {
  }

  AudioInput *
  DriverManager::audio_input(const char * driver, const char * parameter)
  {
    return 0;
  }
 
  AudioOutput *
  DriverManager::audio_output(const char * driver, const char * parameter)
  {
    return 0;
  }
 
  Keying *
  DriverManager::keying(const char * driver, const char * parameter)
  {
    return 0;
  }
 
  PTTInput *
  DriverManager::ptt_input(const char * driver, const char * parameter)
  {
    return 0;
  }
 
  TextInput *
  DriverManager::text_input(const char * driver, const char * parameter)
  {
    return 0;
  }
 
  UserInterface *
  DriverManager::user_interface(const char * driver, const char * parameter)
  {
    return 0;
  }

  void
  DriverManager::register_audio_input(const char * driver, AudioInput * (*creator)(const char *))
  {
    clog << "Registered audio input driver \"" << driver << "\"." << endl;
  }

  void
  DriverManager::register_audio_output(const char * driver, AudioOutput * (*creator)(const char *))
  {
    clog << "Registered audio output driver \"" << driver << "\"." << endl;
  }

  void
  DriverManager::register_keying(const char * driver, Keying * (*creator)(const char *))
  {
    clog << "Registered keying driver \"" << driver << "\"." << endl;
  }

  void
  DriverManager::register_ptt_input(const char * driver, PTTInput * (*creator)(const char *))
  {
    clog << "Registered PTT driver \"" << driver << "\"." << endl;
  }


  void
  DriverManager::register_text_input(const char * driver, TextInput * (*creator)(const char *))
  {
    clog << "Registered text input driver \"" << driver << "\"." << endl;
  }

  void
  DriverManager::register_user_interface(const char * driver, UserInterface * (*creator)(const char *))
  {
    clog << "Registered user interface driver \"" << driver << "\"." << endl;
  }
};
