/// The main loop of the program.

#include "drivers.h"
#include <unistd.h>
#include <iostream>
#include <string.h>

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

namespace FreeDV {

  class Run {
  private:
    const std::size_t	TempSize = 2048;
    Interfaces * const	i;
    bool		begin_receive;
    bool		begin_transmit;
    FIFO		codec_fifo;
    FIFO		input_fifo;
    FIFO		output_fifo;
    bool		ptt_digital;
    bool		ptt_ssb;
 
    void		key_down();
    void		key_up();
    void		receive();
    void		reset_fifos();
    void		transmit_digital();
    void		transmit_ssb();
  public:
    			Run(Interfaces *);
    			~Run();

    void		run();
  };
  
  Run::Run(Interfaces * interfaces)
  : i(interfaces), begin_receive(true), begin_transmit(false),
    codec_fifo(TempSize * 2), input_fifo(TempSize * 2),
    output_fifo(TempSize * 2), ptt_digital(false), ptt_ssb(false)
  {
  }

  Run::~Run()
  {
  }

  void
  Run::reset_fifos()
  {
    input_fifo.reset();
    codec_fifo.reset();
    output_fifo.reset();
  }

  void
  Run::key_down() {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(true);
      begin_transmit = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  void
  Run::key_up()
  {
    if ( i->keying_output->ready() ) {
      i->keying_output->key(false);
      begin_receive = false;
    }
    else {
      std::cerr << "Keying output is stalled." << std::endl;
    }
  }
  
  // FIX: Parallelize the modem and codec into their own threads. Make the
  // FIFO do locking.
  void
  Run::receive()
  {
    // Drain any data that the loudspeaker can take.
    const std::size_t	output_samples = min(
			 i->loudspeaker->ready(),
			 (output_fifo.outgoing_available() / 2));

    if ( output_samples ) {
      const std::size_t result = i->loudspeaker->write16(
      				  (std::int16_t *)output_fifo.outgoing_buffer(
				   output_samples * 2),
				  output_samples);

      if ( result > 0 )
        output_fifo.outgoing_done(result * 2);
      else
	std::cerr << "Loudspeaker I/O error: " << strerror(errno) << std::endl;
    }
    
    // Fill any data that the receiver can provide.
    const std::size_t	input_samples = min(
			 i->receiver->ready(),
    			 (input_fifo.incoming_available() / 2));

    if ( input_samples ) {
      const std::size_t result = i->receiver->read16(
        (std::int16_t *)input_fifo.incoming_buffer(input_samples * 2),
        input_samples);

      if ( result > 0 )
        input_fifo.incoming_done(result * 2);
      else
	std::cerr << "Loudspeaker I/O error: " << strerror(errno) << std::endl;
    }
    
    const std::size_t	frames_to_demodulate = (codec_fifo.incoming_available()
			 / i->modem->bytes_per_frame());

    if ( frames_to_demodulate && input_fifo.outgoing_available() > i->modem->bytes_per_frame()) {
      std::size_t	samples_to_demodulate = input_fifo.outgoing_available()
			 / 2;
      const std::size_t	bytes_to_demodulate = frames_to_demodulate
			 * i->modem->bytes_per_frame();

      std::size_t result = i->modem->demodulate16(
			    (const std::int16_t *)input_fifo.outgoing_buffer(
			     samples_to_demodulate * 2),
			    codec_fifo.incoming_buffer(bytes_to_demodulate),
			    &samples_to_demodulate,
			    bytes_to_demodulate);

      if ( samples_to_demodulate > 0 )
        input_fifo.outgoing_done(samples_to_demodulate * 2);

      if ( result > 0 )
        codec_fifo.incoming_done(result);
    }

    const std::size_t	frames_to_decode = 
			 (output_fifo.incoming_available() / 2)
			  / i->codec->samples_per_frame();

    if ( frames_to_decode > 0
     && codec_fifo.outgoing_available() > i->codec->bytes_per_frame() ) {
      std::size_t bytes_to_decode = codec_fifo.outgoing_available();

      const std::size_t samples_to_decode = frames_to_decode \
					     * i->codec->samples_per_frame();

      const std::size_t result = i->codec->decode16(
				  codec_fifo.outgoing_buffer(bytes_to_decode),
				  (std::int16_t *)output_fifo.incoming_buffer(
                                   samples_to_decode * 2),
				  &bytes_to_decode,
				  samples_to_decode);

      if ( bytes_to_decode > 0 )
        codec_fifo.outgoing_done(bytes_to_decode);

      if ( result > 0 )
        output_fifo.incoming_done(result * 2);
    }
  }
  
  void
  Run::run()
  {
    while ( true ) {
      receive();
    }
  }

  void
  Run::transmit_digital()
  {
  }
  
  void
  Run::transmit_ssb()
  {
  }

  int
  run(Interfaces * i)
  {
    Run * r = new Run(i);
    r->run();
    return 0;
  }
}
