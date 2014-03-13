/// The main loop of the program.

#include "drivers.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/time.h>

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
    const std::size_t	TempSize = 10240;
    Interfaces * const	i;
    bool		begin_receive;
    bool		begin_transmit;
    FIFO		codec_fifo;
    FIFO		in_fifo;
    FIFO		out_fifo;
    bool		ptt_digital;
    bool		ptt_ssb;
    std::size_t		min_frame_duration;
    struct timespec	last_frame_time;
    struct timespec	next_frame_time;
 
    void		key_down();
    void		key_up();
    void		receive();
    void		reset();
    void		transmit_digital();
    void		transmit_ssb();
  public:
    			Run(Interfaces *);
    			~Run();

    void		run();
  };
  
  Run::Run(Interfaces * interfaces)
  : i(interfaces), begin_receive(true), begin_transmit(false),
    codec_fifo(TempSize * 2), in_fifo(TempSize * 2),
    out_fifo(TempSize * 2), ptt_digital(false), ptt_ssb(false)
  {
    reset();
  }

  Run::~Run()
  {
  }

  void
  Run::reset()
  {
    clock_gettime(CLOCK_MONOTONIC, &last_frame_time);
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
    struct timespec	start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Drain any data that the loudspeaker can take.
    const std::size_t	out_samples = min(
			 i->loudspeaker->ready(),
			 (out_fifo.get_available() / 2));

    if ( out_samples ) {
      const int result = i->loudspeaker->write16(
      				  (std::int16_t *)out_fifo.get(
				   out_samples * 2),
				  out_samples);

      if ( result > 0 )
        out_fifo.get_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Loudspeaker I/O error: " << strerror(errno) << std::endl;
    }
    
    // Fill any data that the receiver can provide.
    const std::size_t	in_samples = min(
			 i->receiver->ready(),
    			 (in_fifo.put_space() / 2));

    if ( in_samples ) {
      const int result = i->receiver->read16(
        (std::int16_t *)in_fifo.put(in_samples * 2),
        in_samples);

      if ( result > 0 )
        in_fifo.put_done(result * 2);
      else if ( result < 0 )
	std::cerr << "Receiver I/O error: " << strerror(errno) << std::endl;
    }
    
    if ( in_fifo.get_available() > 0 ) {
      std::size_t	samples_to_demodulate = in_fifo.get_available() / 2;
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

    if ( codec_fifo.get_available() > 0 ) {
      std::size_t bytes_to_decode = codec_fifo.get_available();

      const std::size_t samples_to_decode = out_fifo.put_space() / 2;

      const std::size_t result = i->codec->decode16(
				  codec_fifo.get(bytes_to_decode),
				  (std::int16_t *)out_fifo.put(
                                   samples_to_decode * 2),
				  &bytes_to_decode,
				  samples_to_decode);

      if ( bytes_to_decode > 0 )
        codec_fifo.get_done(bytes_to_decode);

      // TODO: We can do clock tuning here to maximize power saving, if it
      // results in any noticable gain.
      if ( result > 0 ) {
	// std::cerr << '.';
        out_fifo.put_done(result * 2);

        last_frame_time = start_time;
        const long duration = ((min_frame_duration - 1) * 1000000);

        next_frame_time.tv_sec = last_frame_time.tv_sec;
        next_frame_time.tv_nsec = last_frame_time.tv_nsec + duration;
        next_frame_time.tv_sec += next_frame_time.tv_nsec / 1000000000;
        next_frame_time.tv_nsec %= 1000000000;
      }
      else {
	// std::cerr << '+';
	// Add a microsecond. We really could poll the I/O interfaces instead.
        next_frame_time.tv_nsec += 1000000;
        next_frame_time.tv_sec += next_frame_time.tv_nsec / 1000000000;
        next_frame_time.tv_nsec %= 1000000000;
      }
    }
  }
  
  void
  Run::run()
  {
    // The no-op codec, modem, and framer may have very small durations.
    // So we set a minimum here.
    min_frame_duration = 10;
    min_frame_duration = max(min_frame_duration, i->modem->min_frame_duration());
    min_frame_duration = max(min_frame_duration, i->codec->min_frame_duration());
    min_frame_duration = max(min_frame_duration, i->framer->min_frame_duration());

    std::cerr << "minimum frame duration is " << min_frame_duration << std::endl;
    assert(min_frame_duration < 1000000);

    while ( true ) {
      receive();
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_frame_time, 0);
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
