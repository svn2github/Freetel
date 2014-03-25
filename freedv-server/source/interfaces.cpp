/// \file interfaces.cpp
/// Implementation of the Interfaces class.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

// Empty string, for convenience in initializing drivers.
static const char empty[1] = { '\0' };

namespace FreeDV {
  Interfaces::Interfaces()
   : codec(0),
   framer(0), keying_output(0), loudspeaker(0),
   microphone(0), modem(0), ptt_input_digital(0),
   ptt_input_ssb(0), receiver(0), text_input(0),
   transmitter(0), user_interface(0)
  {
  }

  Interfaces::~Interfaces()
  {
  }

  void
  Interfaces::fill_in()
  {
    if ( !codec )
      codec = Driver::CodecNoOp(empty);

    if ( !framer )
      framer = Driver::FramerNoOp(empty);

    if ( !keying_output )
      keying_output = Driver::KeyingSink(empty);

    if ( !loudspeaker )
      loudspeaker = Driver::AudioOutDefault();

    if ( !microphone )
      // microphone = Driver::AudioInDefault();
      microphone = Driver::Tone("1000,1.0");

    if ( !modem )
      modem = Driver::ModemNoOp(empty);

    if ( !ptt_input_digital )
      ptt_input_digital = Driver::PTTConstant(empty);

    if ( !ptt_input_ssb )
      ptt_input_ssb = Driver::PTTConstant(empty);

    if ( !text_input )
      text_input = Driver::TextConstant(empty);

    if ( !transmitter )
      transmitter = Driver::AudioSink(empty);

    if ( !receiver )
      receiver = Driver::Tone("1000,1.0");

    if ( !user_interface )
      user_interface = Driver::BlankPanel(empty, this);
  }

  std::ostream &
  Interfaces::print(std::ostream & stream, const char * _program_name) const
  {
    if ( _program_name ) {
      stream << program_name << " \\" << std::endl;
    }
    stream << "--codec=\"" << *codec << "\" \\" << std::endl;
    stream << "--framer=\"" << *framer << "\" \\" << std::endl;
    stream << "--gui=\"" << *user_interface << "\" \\" << std::endl;
    stream << "--keying=\"" << *keying_output << "\" \\" << std::endl;
    stream << "--loudspeaker=\"" << *loudspeaker << "\" \\" << std::endl;
    stream << "--microphone=\"" << *microphone << "\" \\" << std::endl;
    stream << "--modem=\"" << *modem << "\" \\" << std::endl;
    stream << "--ptt-digital=\"" << *ptt_input_digital << "\" \\" << std::endl;
    stream << "--ptt-ssb=\"" << *ptt_input_ssb << "\" \\" << std::endl;
    stream << "--receiver=\"" << *receiver << "\" \\" << std::endl;
    stream << "--text=\"" << *text_input << "\" \\" << std::endl;
    stream << "--transmitter=\"" << *transmitter << "\"" << std::endl;

    return stream;
  }
}
