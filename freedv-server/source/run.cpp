/// \file run.cpp
/// The main loop of the program.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include <string.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

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

    bool		drain_digital(bool final);
    bool		drain_ssb(bool final);
    void		receive();
    void		start_receive();
    void		transmit_digital();
    void		transmit_ssb();
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
  Run::drain_digital(bool final)
  {

    std::size_t		samples_to_encode = in_fifo.get_available() / 2;
    const std::size_t	bytes_to_encode = codec_fifo.put_space();

    // If draining the last frame, make sure we have enough samples to encode
    // the last codec frame. Fill with zero if necessary.
    // FIX: Replace this with a state before un-key which fades out the
    // microphone, ending at a codec frame boundary, thus avoiding the click
    // we make here.
    if ( final ) {
      const std::size_t samples_per_frame = i->codec->samples_per_frame();
      if ( samples_to_encode < samples_per_frame && samples_to_encode > 0 ) {
        const std::size_t fill = samples_per_frame - samples_to_encode;
        const std::size_t bytes = fill * 2;
        memset(in_fifo.put(bytes), 0, bytes);
        in_fifo.put_done(bytes);
        samples_to_encode += fill;
      }
    }

    if ( samples_to_encode > 0 && bytes_to_encode > 0 ) {
      const std::size_t bytes_encoded = i->codec->encode16(
				  (std::int16_t *)in_fifo.get(
                                   samples_to_encode * 2),
				  codec_fifo.put(bytes_to_encode),
				  bytes_to_encode,
				  &samples_to_encode);

      if ( samples_to_encode > 0 ) {
        in_fifo.get_done(samples_to_encode * 2);
        if ( bytes_encoded > 0 )
          codec_fifo.put_done(bytes_encoded);
      }
    }

    std::size_t		bytes_to_modulate = codec_fifo.get_available();
    const std::size_t	samples_to_modulate = out_fifo.put_space() / 2;

    // If the codec is drained and we are draining the last modem frame,
    // make sure we have enough bytes to encode the last modem frame.
    // Fill with zero if necessary.
    if ( final && in_fifo.get_available() == 0 ) {
      const std::size_t bytes_per_frame = i->modem->bytes_per_frame();
      if ( bytes_to_modulate < bytes_per_frame && bytes_to_modulate > 0 ) {
        const std::size_t fill = bytes_per_frame - bytes_to_modulate;
        memset(codec_fifo.put(fill), 0, fill);
        codec_fifo.put_done(fill);
        bytes_to_modulate += fill;
        // Leave this debugging message in place until I have a codec with
        // a frame size less than a modem frame, and can test this block.
        std::cerr << "Fill modem." << std::endl;
      }
    }

    if ( bytes_to_modulate > 0 && samples_to_modulate > 0 ) {
      const std::size_t samples_modulated = i->modem->modulate16(
       codec_fifo.get(bytes_to_modulate),
       (std::int16_t *)out_fifo.put(samples_to_modulate * 2),
       &bytes_to_modulate,
       samples_to_modulate);

      if ( bytes_to_modulate > 0 ) {
        codec_fifo.get_done(bytes_to_modulate);

        if ( samples_modulated > 0 )
          out_fifo.put_done(samples_modulated * 2);
      }
    }
    // Drain any data that the transmitter can take.
    const std::size_t	out_samples = min(
			 i->transmitter->ready(),
			 (out_fifo.get_available() / 2));

    if ( out_samples > 0 ) {
      const int result = i->transmitter->write16(
      				  (std::int16_t *)out_fifo.get(out_samples * 2),
				  out_samples);

      if ( result > 0 )
        out_fifo.get_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Transmitter I/O error: " << strerror(errno) << std::endl;
    }

    if ( final ) {
      if ( in_fifo.get_available() == 0
       &&  codec_fifo.get_available() == 0
       &&  out_fifo.get_available() == 0 ) {
        i->transmitter->drain();
        return true;
      }
    }

    return false;
  }

  bool
  Run::drain_ssb(bool final)
  {
    if ( final && in_fifo.get_available() == 0 ) {
      i->transmitter->drain();
      return true;
    }

    // Drain any data that the transmitter can take.
    const std::size_t	out_samples = min(
			 i->transmitter->ready(),
			 (in_fifo.get_available() / 2));

    if ( out_samples > 0 ) {
      const int result = i->transmitter->write16(
      				  (std::int16_t *)in_fifo.get(out_samples * 2),
				  out_samples);

      if ( result > 0 )
        in_fifo.get_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Transmitter I/O error: " << strerror(errno) << std::endl;
    }
    return false;
  }

  // FIX: Parallelize the modem and codec into their own threads. Make the
  // FIFO do locking.
  void
  Run::receive()
  {
    // Read any data that the receiver can provide.
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
    
    std::size_t		samples_to_demodulate = in_fifo.get_available() / 2;
    const std::size_t	bytes_to_demodulate = codec_fifo.put_space();

    // FIX: Implement soft squelch.

    if ( samples_to_demodulate > 0 && bytes_to_demodulate > 0 ) {
      const std::size_t bytes_demodulated = i->modem->demodulate16(
       (const std::int16_t *)in_fifo.get(
       samples_to_demodulate * 2),
       codec_fifo.put(bytes_to_demodulate),
       bytes_to_demodulate,
       &samples_to_demodulate);

      if ( bytes_demodulated > 0 )
        codec_fifo.put_done(bytes_demodulated);

      if ( samples_to_demodulate > 0 )
        in_fifo.get_done(samples_to_demodulate * 2);
    }

    std::size_t		bytes_to_decode = codec_fifo.get_available();
    const std::size_t	samples_to_decode = out_fifo.put_space() / 2;

    if ( bytes_to_decode > 0 && samples_to_decode > 0 ) {
      size_t samples_decoded = i->codec->decode16(
				  codec_fifo.get(bytes_to_decode),
				  (std::int16_t *)out_fifo.put(
                                   samples_to_decode * 2),
				  &bytes_to_decode,
				  samples_to_decode);

      if ( bytes_to_decode > 0 )
        codec_fifo.get_done(bytes_to_decode);

      if ( samples_decoded > 0 )
        out_fifo.put_done(samples_decoded * 2);
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
    i->keying_output->key(0);
    i->receiver->start();
    i->loudspeaker->start();
    if ( !add_poll_device(i->receiver) )
      add_poll_device(i->loudspeaker);

    for ( ; ; ) {
      for ( int j = 0; j < poll_fd_count; j++ ) {
        poll_fds[j].revents = 0;
      }
      
      int result = 0;

      // Sometimes there are not any poll-capable devices to wake us when
      // it's time to transmit the next audio frame. This happens when using
      // the testing drivers for both input and output, and during the UnKey
      // state. Testing drivers will run faster than the real rate if something
      // doesn't limit then.
      //
      // FIX: Instead of nanosleep(), use a wall-clock sleep that wakes up when
      // it is time for the next packet to arrive. This can more easily take
      // compute time into account, and can properly time the testing drivers.
      //
      if ( poll_fd_count > poll_fd_base )
        result = IODevice::poll(poll_fds, poll_fd_count, 1000);
      else if ( state != UnKey ) {
        struct timespec	request;
        struct timespec remainder;
        request.tv_sec = 0;
        request.tv_nsec = AudioFrameDuration * 1000000;
        nanosleep(&request, &remainder);
      }

      if ( result < 0 )
        do_throw(result, "Poll");

      if ( i->ptt_input_digital->ready() )
        ptt_digital = i->ptt_input_digital->state();
      if ( i->ptt_input_ssb->ready() )
        ptt_ssb = i->ptt_input_ssb->state();

      switch ( state ) {
      case DrainDigital:
        if ( drain_digital(true) ) {
          poll_fd_count = poll_fd_base;
          state = UnKey;
        }
        break;
      case DrainSSB:
        if ( drain_ssb(true) ) {
          poll_fd_count = poll_fd_base;
          state = UnKey;
        }
        break;
      case Receive:
        if ( ptt_digital || ptt_ssb ) {
	  i->receiver->stop();
          i->loudspeaker->drain();
          i->loudspeaker->stop();

	  // Stop polling the receiver devices.
          poll_fd_count = poll_fd_base;

	  // Flush all of the FIFO data.
	  reset();

	  // Start accumulating the audio samples for the first codec frame
	  // before we've finished keying the transmitter.
          i->microphone->start();

          i->keying_output->key(1);


          if ( ptt_digital )
            state = TransmitDigital;
          else
            state = TransmitSSB;

          i->transmitter->start();

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
          i->microphone->stop();
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
          i->microphone->stop();
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
	  // Reset polling from the Drain operation.
          poll_fd_count = poll_fd_base;

          i->microphone->start();
          // Start polling the transmitter devices.
          if ( !add_poll_device(i->microphone) )
            add_poll_device(i->transmitter);

          if ( ptt_digital )
            state = TransmitDigital;
          else
            state = TransmitSSB;
        }
        else {
          i->transmitter->stop();
	  // Stop polling the transmitter devices.
          poll_fd_count = poll_fd_base;

	  // Flush all of the FIFO data.
          reset();

          i->keying_output->key(0);

          state = Receive;
          i->receiver->start();
          i->loudspeaker->start();

	  // Start polling the receiver devices.
          if ( !add_poll_device(i->receiver) )
            add_poll_device(i->loudspeaker);
        }
        break;
      }
    }
  }

  void
  Run::transmit_digital()
  {
    // Fill any data that the microphone can provide.
    const std::size_t	in_samples = min(
			 i->microphone->ready(),
    			 (in_fifo.put_space() / 2));

    if ( in_samples > 0 ) {
      const int result = i->microphone->read16(
        (std::int16_t *)in_fifo.put(in_samples * 2),
        in_samples);

      if ( result > 0 )
        in_fifo.put_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Microphone I/O error: " << strerror(errno) << std::endl;
    }

    // Encode, modulate, and output any microphone audio that's been captured.
    drain_digital(false);
  }
   
  void
  Run::transmit_ssb()
  {
    // Fill any data that the microphone can provide.
    const std::size_t	in_samples = min(
			 i->microphone->ready(),
    			 (in_fifo.put_space() / 2));

    if ( in_samples > 0 ) {
      const int result = i->microphone->read16(
        (std::int16_t *)in_fifo.put(in_samples * 2),
        in_samples);

      if ( result > 0 )
        in_fifo.put_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Microphone I/O error: " << strerror(errno) << std::endl;
    }
    
    // Output any microphone audio that's been captured.
    drain_ssb(false);
  }
   
  int
  run(Interfaces * i)
  {
    Run * r = new Run(i);
    r->half_duplex();
    return 0;
  }
}
