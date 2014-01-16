/// The main loop of the program.

#include "drivers.h"

namespace FreeDV {
  static void
  ptt_digital(bool value)
  {
  }

  static void
  ptt_ssb(bool value)
  {
  }

  int
  run(Interfaces * i)
  {
    if ( i->ptt_input_digital )
      i->ptt_input_digital->set_callback(ptt_digital);
      
    if ( i->ptt_input_ssb )
      i->ptt_input_ssb->set_callback(ptt_ssb);

    return 0;
  }
}
