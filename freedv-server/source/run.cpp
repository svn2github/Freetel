/// \file run.cpp
/// The main loop of the program.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>

// Start of Transmit:
// Make the delay at start of transmit and end of transmit adjustable.
// The radio in general takes some time to begin transmitting, and thus we
// should not send audio until that's done.
//
// End of Transmit:
// There is a lot to fill in for end-of-transmit.
// On PTT-up, we should read the remaining audio in the microphone
// device queue first, allow it all to drain out the transmitter, and then
// un-key the transmitter.
//

namespace FreeDV {
  /// This class implements the main loop of the FreeDV program.
  ///
  class Run {
  private:
    const std::size_t	FIFOSize = MaximumFrameSamples * sizeof(int16_t) * 2;
    Interfaces * const	i;
    bool		begin_receive;
    bool		begin_transmit;
    FIFO		codec_fifo;
    FIFO		in_fifo;
    FIFO		out_fifo;
    int			poll_fd_count;
    bool		ptt_digital;
    bool		ptt_ssb;
    bool		started;
    PollType		poll_fds[100];
 
    bool		add_poll_device(IODevice * device);
    NORETURN void	do_throw(int error, const char * message);
    void		key_down();
    void		key_up();
    void		receive();
    void		reset();
    void		transmit_digital();
    void		transmit_ssb();
  public:
    /// Construct the context for the main loop of FreeDV.
    /// \param interfaces the address of an Interfaces instance containing
    /// information on all of the selected device and algorithm drivers.
    			Run(Interfaces * interfaces);
    			~Run();

    /// Run the main loop of FreeDV.
    ///
    void		run();
  };
  
  Run::Run(Interfaces * interfaces)
  : i(interfaces), begin_receive(true), begin_transmit(false),
    codec_fifo(FIFOSize), in_fifo(FIFOSize),
    out_fifo(FIFOSize), poll_fd_count(0), ptt_digital(false), ptt_ssb(false),
    started(false)
  {
    reset();

    // FIX: This needs to be done at the start of the receive or transmit
    // loop, so that only the necessary devices will be polled.
    if ( !add_poll_device(i->receiver) )
      add_poll_device(i->loudspeaker);
    if ( !add_poll_device(i->microphone) )
      add_poll_device(i->transmitter);
    add_poll_device(i->ptt_input_digital);
    add_poll_device(i->ptt_input_ssb);
    add_poll_device(i->text_input);
    add_poll_device(i->user_interface);
  }

  Run::~Run()
  {
  }

  bool
  Run::add_poll_device(IODevice * device)
  {
    static const int	space = sizeof(poll_fds) / sizeof(*poll_fds);

    const int result = device->poll_fds(
     &poll_fds[poll_fd_count],
     space - poll_fd_count);

    if ( result >= 0 ) {
      const int new_size = poll_fd_count + result;

      if ( new_size < space ) {
        poll_fd_count = new_size;
        return new_size > 0;
      }
      else
        do_throw(0, "Too many file descriptors for poll");
    }
    else {
      std::ostringstream	str;

      device->print(str);
      do_throw(result, str.str().c_str());
    }
  }

  NORETURN void
  Run::do_throw(const int error, const char * message = 0)
  {
    std::ostringstream str;

    str << "Main loop error: ";
    if ( message )
      str << message << ": ";
    if ( error ) {
      str << ": ";
      str << strerror(error);
    }
    str << '.';
    throw std::runtime_error(str.str().c_str());
  }

  void
  Run::reset()
  {
    started = false;
    in_fifo.reset();
    codec_fifo.reset();
    out_fifo.reset();
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
    // Fill any data that the receiver can provide.
    const std::size_t	in_samples = min(
			 i->receiver->ready(),
    			 (in_fifo.put_space() / 2));

    if ( in_samples > 0 ) {
      const int result = i->receiver->read16(
        (std::int16_t *)in_fifo.put(in_samples * 2),
        in_samples);

      if ( result > 0 )
        in_fifo.put_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Receiver I/O error: " << strerror(errno) << std::endl;
    }
    
    std::size_t	samples_to_demodulate = in_fifo.get_available() / 2;
    if ( samples_to_demodulate > 0 ) {
      const std::size_t	bytes_to_demodulate = codec_fifo.put_space();

      std::size_t result = i->modem->demodulate16(
			    (const std::int16_t *)in_fifo.get(
			     samples_to_demodulate * 2),
			    codec_fifo.put(bytes_to_demodulate),
			    &samples_to_demodulate,
			    bytes_to_demodulate);


      if ( samples_to_demodulate > 0 )
        in_fifo.get_done(samples_to_demodulate * 2);

      if ( result > 0 )
        codec_fifo.put_done(result);
    }

    std::size_t bytes_to_decode = codec_fifo.get_available();
    if ( bytes_to_decode > 0 ) {

      const std::size_t samples_to_decode = out_fifo.put_space() / 2;

      const std::size_t result = i->codec->decode16(
				  codec_fifo.get(bytes_to_decode),
				  (std::int16_t *)out_fifo.put(
                                   samples_to_decode * 2),
				  &bytes_to_decode,
				  samples_to_decode);

      if ( bytes_to_decode > 0 )
        codec_fifo.get_done(bytes_to_decode);

      if ( result > 0 )
        out_fifo.put_done(result * 2);
    }

    // Drain any data that the loudspeaker can take.
    const std::size_t	out_samples = min(
			 i->loudspeaker->ready(),
			 (out_fifo.get_available() / 2));

    if ( out_samples > 0 ) {
      const int result = i->loudspeaker->write16(
      				  (std::int16_t *)out_fifo.get(
				   out_samples * 2),
				  out_samples);

      if ( result > 0 ) {
        started = true;
        out_fifo.get_done(result * 2);
      }
      else if ( result < 0 )
	std::cerr << "Loudspeaker I/O error: " << strerror(errno) << std::endl;
    }
  }
  
  void
  Run::run()
  {
    for ( ; ; ) {
      receive();

      for ( int i = 0; i < poll_fd_count; i++ )
        poll_fds[i].revents = 0;

      const int result = IODevice::poll(
       poll_fds,
       poll_fd_count,
       AudioFrameDuration);

      if ( result < 0 )
        do_throw(result, "Poll");
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
