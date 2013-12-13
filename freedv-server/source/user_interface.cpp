#include "drivers.h"

namespace FreeDV {
  UserInterface::UserInterface()
  {
  }

  UserInterface::~UserInterface()
  {
  }

  AudioInput *
  UserInterface::microphone()
  {
    return 0;
  }

  AudioOutput *
  UserInterface::loudspeaker()
  {
    return 0;
  }

  TextInput *
  UserInterface::text_input()
  {
    return 0;
  }

  PTTInput *
  UserInterface::ptt_input()
  {
    return 0;
  }
}
