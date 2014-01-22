/// The main loop of the program.

#include "drivers.h"
#include <unistd.h>
#include <iostream>

/// FIX:
///
/// Start of Transmit:
/// Make the delay at start of transmit and end of transmit adjustable.
/// The radio in general takes some time to begin transmitting, and thus we
/// should not send audio until that's done.
///
/// End of Transmit:
/// There is a lot to fill in for end-of-transmit.
/// On PTT-up, we should be sending the remaining audio in the microphone
/// device queue first, waiting for completion of its transmission, and then
/// un-keying the transmitter.
///
/// Codec to Modem connection:
/// We need a circular buffer between the codec and the modem. It's perfectly
/// possible for the modem to demodulate nothing, given an array of audio
/// samples. We can't invoke the codec until we have an entire frame for it
/// to work upon.

namespace FreeDV {
  static void key_down(Interfaces * i);
  static void key_up(Interfaces * i);
  static void receive(Interfaces * i);
  static void transmit_digital(Interfaces * i);
  static void transmit_ssb(Interfaces * i);
  
  static bool begin_transmit = false;
  static bool begin_receive = true;

  int
  run(Interfaces * i)
  {
    static bool ptt_digital = false;
    static bool ptt_ssb = false;

    while ( true ) {
      if ( i->ptt_input_digital->ready() ) {
        bool state = i->ptt_input_digital->state();
        if ( state && !ptt_digital && !ptt_ssb ) {
          ptt_digital = true;
          begin_transmit = true;
        }
        else if ( !state && ptt_digital ) {
          begin_receive = true;
          ptt_digital = false;
        }
      }
      if ( i->ptt_input_ssb->ready() ) {
        bool state = i->ptt_input_ssb->state();
        if ( state && !ptt_digital && !ptt_ssb ) {
          ptt_ssb = true;
          begin_transmit = true;
        }
        else if ( !state && ptt_ssb ) {
          begin_receive = true;
          ptt_ssb = false;
        }
      }
      if ( begin_transmit ) {
        key_down(i);
      }
      else if ( begin_receive ) {
        key_up(i);
      }
      else if ( ptt_digital ) {
        transmit_digital(i);
      }
      else if ( ptt_ssb ) {
        transmit_ssb(i);
      }
      else {
        receive(i);
      }
      usleep(20100);
    }
  }

  static void
  key_down(Interfaces * i)
  {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(true);
      begin_transmit = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  static void
  key_up(Interfaces * i)
  {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(false);
      begin_receive = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  static void
  receive(Interfaces * i)
  {
    const std::size_t	samples_to_decode = i->receiver->ready()
			 % i->modem->samples_per_frame();
  }
  
  static void
  transmit_digital(Interfaces * i)
  {
  }
  
  static void
  transmit_ssb(Interfaces * i)
  {
  }
}
