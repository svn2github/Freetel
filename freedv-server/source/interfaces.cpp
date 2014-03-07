/// The Interfaces class.

#include "drivers.h"

// Empty string, for convenience in initializing drivers.
static const char empty[1] = { '\0' };

namespace FreeDV {
  void
  Interfaces::fill_in()
  {
    if ( !codec )
      codec = Driver::CodecNoOp(empty);

    if ( !keying_output )
      keying_output = Driver::KeyingSink(empty);

    if ( !loudspeaker )
      loudspeaker = Driver::AudioSink(empty);

    if ( !microphone )
      microphone = Driver::Tone(empty);

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
      receiver = Driver::Tone(empty);

    if ( !user_interface )
      user_interface = Driver::BlankPanel(empty, this);
  }

  std::ostream &
  Interfaces::print(std::ostream & stream) const
  {
    stream << "--codec=" << *codec << std::endl;
    stream << "--gui=" << *user_interface << std::endl;
    stream << "--keying=" << *keying_output << std::endl;
    stream << "--loudspeaker=" << *loudspeaker << std::endl;
    stream << "--microphone=" << *microphone << std::endl;
    stream << "--modem=" << *modem << std::endl;
    stream << "--ptt-digital=" << *ptt_input_digital << std::endl;
    stream << "--ptt-ssb=" << *ptt_input_ssb << std::endl;
    stream << "--receiver=" << *receiver << std::endl;
    stream << "--text=" << *text_input << std::endl;
    stream << "--transmitter=" << *transmitter << std::endl;

    return stream;
  }
}
