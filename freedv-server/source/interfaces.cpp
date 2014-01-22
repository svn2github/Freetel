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
    using namespace std;

    stream << "--codec=" << *codec << endl;
    stream << "--gui=" << *user_interface << endl;
    stream << "--keying=" << *keying_output << endl;
    stream << "--loudspeaker=" << *loudspeaker << endl;
    stream << "--microphone=" << *microphone << endl;
    stream << "--modem=" << *modem << endl;
    stream << "--ptt-digital=" << *ptt_input_digital << endl;
    stream << "--ptt-ssb=" << *ptt_input_ssb << endl;
    stream << "--receiver=" << *receiver << endl;
    stream << "--text=" << *text_input << endl;
    stream << "--transmitter=" << *transmitter << endl;

    return stream;
  }
}