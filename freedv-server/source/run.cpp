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
    int			poll_fd_count;
    int			poll_fd_base;
    PollType		poll_fds[100];

    FIFO		codec_fifo;
    FIFO		in_fifo;
    FIFO		out_fifo;
 
    // Disable copy constructor and operator=().
    Run(const Run &);
    Run &		operator=(const Run &);

    bool		add_poll_device(IODevice * device);
    NORETURN void	do_throw(int error, const char * message);
    void		reset();

    bool		drain_digital();
    bool		drain_ssb();
    void		key();
    void		receive();
    void		start_receive();
    void		start_transmit_digital();
    void		start_transmit_ssb();
    void		stop_receive();
    void		transmit_digital();
    void		transmit_ssb();
    void		un_key();
  public:
    /// Construct the context for the main loop of FreeDV.
    /// \param interfaces the address of an Interfaces instance containing
    /// information on all of the selected device and algorithm drivers.
    			Run(Interfaces * interfaces);
    			~Run();

    /// Run the main loop of FreeDV in half-duplex mode.
    ///
    void		half_duplex();
  };
  
  Run::Run(Interfaces * interfaces)
  : i(interfaces), poll_fd_count(0), poll_fd_base(0),
    codec_fifo(FIFOSize), in_fifo(FIFOSize),
    out_fifo(FIFOSize)
  {
    reset();
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

    if ( result > 0 ) {
      poll_fd_count += result;
      return true;
    }
    else if ( result < 0 ) {
      std::ostringstream	str;

      device->print(str);
      do_throw(result, str.str().c_str());
    }
    return false;
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
    in_fifo.reset();
    codec_fifo.reset();
    out_fifo.reset();
  }

  bool
  Run::drain_digital()
  {
    static bool	done = false;
    if ( !done ) {
      done = true;
      return false;
    }
    else {
      done = false;
      return true;
    }
  }

  bool
  Run::drain_ssb()
  {
    static bool	done = false;
    if ( !done ) {
      done = true;
      return false;
    }
    else {
      done = false;
      return true;
    }
  }

  void
  Run::key()
  {
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
    
    std::size_t bytes_demodulated = 0;
    std::size_t	samples_to_demodulate = in_fifo.get_available() / 2;
    if ( samples_to_demodulate > 0 ) {
      const std::size_t	bytes_to_demodulate = codec_fifo.put_space();

      bytes_demodulated = i->modem->demodulate16(
       (const std::int16_t *)in_fifo.get(
       samples_to_demodulate * 2),
       codec_fifo.put(bytes_to_demodulate),
       bytes_to_demodulate,
       &samples_to_demodulate);


      if ( bytes_demodulated > 0 )
        codec_fifo.put_done(bytes_demodulated);
    }

    if ( bytes_demodulated > 0 ) {
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
    }
    else {
      std::size_t length = samples_to_demodulate * 2;
      // Did not demodulate any data. Push it to the loudspeaker.
      //
      // For this to work, we need the modem to:
      // * Consume input samples demodulating no data.
      // or
      // * Consume input samples demodulating them all as data.
      // and
      // * Never consume input samples demodulating less than the complete
      //   amount consumed as data.
      // So, the modem should return before it either starts demodulating data
      // or stops doing so.
      // 
      // The problem is that we wish to continue to send to the loudspeaker
      // the correct number of samples per second regardless of what is
      // happening. And we want the time sequence of non-demodulated analog
      // signal (or noise) and demodulated digital data to be in the correct
      // order.
      //
      // FIX: Squelch here.
      const uint8_t * in = in_fifo.get(length);
      uint8_t * out = out_fifo.put(length);
      memcpy(out, in, length);
      out_fifo.put_done(length);
    }
    if ( samples_to_demodulate > 0 )
      in_fifo.get_done(samples_to_demodulate * 2);

    // Drain any data that the loudspeaker can take.
    const std::size_t	out_samples = min(
			 i->loudspeaker->ready(),
			 (out_fifo.get_available() / 2));

    if ( out_samples > 0 ) {
      const int result = i->loudspeaker->write16(
      				  (std::int16_t *)out_fifo.get(
				   out_samples * 2),
				  out_samples);

      if ( result > 0 )
        out_fifo.get_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Loudspeaker I/O error: " << strerror(errno) << std::endl;
    }
  }
   
  // FIX: Once everything else has been tested, make this program work with
  // three T/R devices for each of Digital and SSB. This accounts for the
  // GUI, a pedal, and a hand switch.
  void
  Run::half_duplex()
  {
    enum TRState {
      DrainDigital,
      DrainSSB,
      Receive,
      TransmitDigital,
      TransmitSSB,
      UnKey
    };


    TRState	state = Receive;
    bool	ptt_digital = false;
    bool	ptt_ssb = false;

    // Start polling the T/R devices.
    add_poll_device(i->ptt_input_digital);
    add_poll_device(i->ptt_input_ssb);

    // Any time we set poll_fd_count to poll_fd_base, it will stop polling
    // of the receive or transmit devices, and then we can set up what devices
    // we would like to poll now. We will do this when we switch from transmit
    // to receive, and when we switch from transmit to drain.
    poll_fd_base = poll_fd_count;

    // Always start in receive mode. If the T/R switches are pressed,
    // we'll catch up.
    un_key();
    start_receive();
    if ( !add_poll_device(i->receiver) )
      add_poll_device(i->loudspeaker);

    for ( ; ; ) {
      for ( int j = 0; j < poll_fd_count; j++ ) {
        poll_fds[j].revents = 0;
        poll_fds[j].events = POLLIN;
      }
      
      const int result = IODevice::poll(poll_fds, poll_fd_count, 1000);

      if ( result < 0 )
        do_throw(result, "Poll");

      if ( i->ptt_input_digital->ready() )
        ptt_digital = i->ptt_input_digital->state();
      if ( i->ptt_input_ssb->ready() )
        ptt_ssb = i->ptt_input_ssb->state();

      switch ( state ) {
      case DrainDigital:
        if ( drain_digital() ) {
          poll_fd_count = poll_fd_base;
          state = UnKey;
        }
        break;
      case DrainSSB:
        if ( drain_ssb() ) {
          poll_fd_count = poll_fd_base;
          state = UnKey;
        }
        break;
      case Receive:
        if ( ptt_digital || ptt_ssb ) {
          stop_receive();

          key();

          if ( ptt_digital ) {
            state = TransmitDigital;
            start_transmit_digital();
	  }
          else {
            state = TransmitSSB;
            start_transmit_ssb();
          }

	  // Stop polling the receiver devices.
          poll_fd_count = poll_fd_base;

          // Start polling the transmitter devices.
          if ( !add_poll_device(i->microphone) )
            add_poll_device(i->transmitter);
        }
        else
          receive();

        break;
      case TransmitDigital:
        if ( ptt_digital == false ) {
          state = DrainDigital;
          // Stop polling the microphone.
          poll_fd_count = poll_fd_base;

	  // Poll the transmitter until it's drained.
	  add_poll_device(i->transmitter);
        }
        else
          transmit_digital();
        break;
      case TransmitSSB:
        if ( ptt_ssb == false ) {
          state = DrainSSB;
          // Stop polling the microphone.
          poll_fd_count = poll_fd_base;

	  // Poll the transmitter until it's drained.
	  add_poll_device(i->transmitter);
        }
        else
          transmit_ssb();
        break;
      case UnKey:
        if ( ptt_digital || ptt_ssb ) {
          if ( ptt_digital ) {
            state = TransmitDigital;
            start_transmit_digital();
	  }
          else {
            state = TransmitSSB;
            start_transmit_ssb();
          }
        }
        else {
	  // Stop polling the transmitter devices.
          poll_fd_count = poll_fd_base;

          un_key();
          state = Receive;
          start_receive();

	  // Start polling the receiver devices.
          if ( !add_poll_device(i->receiver) )
            add_poll_device(i->loudspeaker);

        }
        break;
      }
    }
  }

  void	
  Run::start_receive()
  {
  }

  void
  Run::start_transmit_digital()
  {
  }

  void
  Run::start_transmit_ssb()
  {
  }

  void
  Run::stop_receive()
  {
  }

  void
  Run::transmit_digital()
  {
  }
  
  void
  Run::transmit_ssb()
  {
  }

  void
  Run::un_key()
  {
  }

  int
  run(Interfaces * i)
  {
    Run * r = new Run(i);
    r->half_duplex();
    return 0;
  }
}
