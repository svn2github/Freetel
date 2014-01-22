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
/// On PTT-up, we should read the remaining audio in the microphone
/// device queue first, allow it all to drain out the transmitter, and then
/// un-key the transmitter.
///
/// Codec to Modem connection:
/// We need a circular buffer between the codec and the modem. It's perfectly
/// possible for the modem to demodulate nothing, given an array of audio
/// samples. We can't invoke the codec until we have an entire frame for it
/// to work upon.

namespace FreeDV {
  class Run {
  private:
    Interfaces *	in;
    bool		begin_transmit;
    bool		begin_receive;
    bool		ptt_digital;
    bool		ptt_ssb;
 
    void		key_down(Interfaces * i);
    void		key_up(Interfaces * i);
    void		receive(Interfaces * i);
    void		transmit_digital(Interfaces * i);
    void		transmit_ssb(Interfaces * i);
  public:
    int			run(Interfaces * i);

    			Run(Interfaces *);
    			~Run();
  };
  
  Run::Run(Interfaces * interfaces)
  : begin_receive(true), begin_transmit(false), ptt_digital(false),
    ptt_ssb(false)
  {
    in = interfaces;
  }

  Run::~Run()
  {
  }

  int
  Run::run(Interfaces * i)
  {
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

  void
  Run::key_down(Interfaces * i)
  {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(true);
      begin_transmit = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  void
  Run::key_up(Interfaces * i)
  {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(false);
      begin_receive = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  void
  Run::receive(Interfaces * i)
  {
    const std::size_t	samples_to_decode = i->receiver->ready()
			 % i->modem->samples_per_frame();
  }
  
  void
  Run::transmit_digital(Interfaces * i)
  {
  }
  
  void
  Run::transmit_ssb(Interfaces * i)
  {
  }

  int
  run(Interfaces * i)
  {
    Run * r = new Run(i);
    return r->run(i);
  }
}
