/// \file platform/linux/alsa.cpp
/// Functions to support the ALSA audio input and output device drivers on
/// Linux.
/// There is at least one other operating system that supports an ALSA-like
/// interface, Nucleus, but at this writing (early 2014) this driver is
/// untested on that system.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"
#include "alsa.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdio.h>

namespace FreeDV {
  static std::ostream &
  error_message(std::ostream & stream, int error) {
    switch ( error ) {
    case 0:
      break;
    case -ENOENT:
      stream << " (Not connected)";
      break;
    case -EBUSY:
      stream << " (In use by another program)";
      break;
    case -EACCES:
      stream << " (Access denied)";
      break;
    default:
      stream << " (" << snd_strerror(error) << ')';
    }
    return stream;
  }
  
  /// Enumerate all of the ALSA device drivers available on the system for
  /// the user.
  /// \param stream The stream upon which information will be rendered.
  /// \param mode The access mode of the stream, either SND_PCM_STREAM_CAPTURE
  ///  or SND_PCM_STREAM_PLAYBACK. 
  /// \return The stream that was passed as an argument, as is the convention
  /// for iostreams inserters.
  std::ostream &
  ALSAEnumerate(std::ostream & stream, snd_pcm_stream_t mode)
  {
    int			card_index = -1;
    snd_pcm_t *		pcm_handle;
  
    const int error = snd_pcm_open(
     &pcm_handle,
     "default",
     mode,
     0);
  
    stream << "\"alsa:default\"";
    error_message(stream, error);
    stream << std::endl;
  
    while ( snd_card_next(&card_index) == 0 && card_index >= 0 ) {
      char		device_name[20];
      snd_ctl_t *	ctl_handle = 0;
      int		pcm_error = 0;
      char *		longname = 0;
      char *		i;
  
      if ( snd_card_get_longname(card_index, &longname) == 0 ) {
        sprintf(device_name, "hw:%d", card_index);
        
        const int ctl_error = snd_ctl_open(&ctl_handle, device_name, 0);
        if ( ctl_error == 0 ) {
          pcm_error = snd_pcm_open(&pcm_handle, device_name, mode, 0);
        }
          
        const int card_error = ctl_error ? ctl_error : pcm_error;
    
        i = strstr(longname, ", full speed");
        if ( i )
          *i = '\0';
       
        i = strstr(longname, " irq ");
        if ( i )
          *i = '\0';

        if ( card_error != -ENOENT ) {
          stream << "\"alsa:" << longname << '"';
          error_message(stream, card_error);
          stream << std::endl;
        }
  
        if ( ctl_error == 0 ) {
      	  if ( pcm_error == 0 )
            snd_pcm_close(pcm_handle);
          snd_ctl_close(ctl_handle);
        }
      }
    }
  
    return stream;
  }

  static int
  open_by_longname(
   snd_pcm_t * *	handle,
   const char *		name,
   snd_pcm_stream_t	stream,
   int			mode)
  {
    const int	length = strlen(name);
    int		card_index = -1;

    // Let the normal open handle "default".
    if ( strcmp(name, "default") == 0 )
      return -ENODEV;

    while ( snd_card_next(&card_index) == 0 && card_index >= 0 ) {
      char		device_name[20];
      char *		longname = 0;
  
      if ( snd_card_get_longname(card_index, &longname) == 0 ) {
        if ( strncmp(name, longname, length) == 0 ) {
          sprintf(device_name, "plughw:%d", card_index);
          return snd_pcm_open(handle, device_name, stream, mode);
        }
      }
    }
  
    return -ENODEV;
  }

  NORETURN static void
  do_throw(
   const int error,
   const char * name,
   snd_pcm_stream_t access,
   const char * message = 0)
  {
    std::ostringstream str;

    str << "ALSA ";

    if ( access == SND_PCM_STREAM_CAPTURE )
      str << "input";
    else
      str << "output";

    str << " device \"" << name << "\" set-up error: ";
     if ( message )
       str << message << ": ";
     str << snd_strerror(error) << '.';
    throw std::runtime_error(str.str().c_str());
  }

  /// Open an ALSA device and set all of the configuration parameters on that
  /// device. This version opens by ALSA name (hw:*n* or plughw:*n*), or
  /// "longname", which is a possibly-shortened version of the name returned
  /// by snd_card_get_longname() that has previously been rendered to the user
  /// using ALSAEnumerate().
  ///
  /// All of the parameters are passed to ALSA functions, and are documented
  /// in the documentation for the ALSA functions mentioned below.
  ///
  /// \param name The name of the ALSA device to open, or its *longname* in
  /// a form previously rendered to the user using ALSAEnumerate(). This will
  /// be passed to snd_pcm_open().
  /// 
  /// \param stream One of SND_PCM_STREAM_CAPTURE or SND_PCM_STREAM_PLAYBACK,
  /// indicates whether to capture or play the audio stream. This will be
  /// passed to snd_pcm_open().
  /// 
  /// \param mode  One of SND_PCM_NONBLOCK or SND_PCM_ASYNC, or 0.
  /// 0 indicates to use conventional blocking I/O. SND_PCM_NONBLOCK
  /// indicates to use POSIX non-blocking I/O, and SND_PCM_ASYNC indicates
  /// to use non-blocking I/O with completion notification. This value will 
  /// be passed to snd_pcm_open().
  ///
  /// \param format The format of the audio stream, selected from a large set 
  /// of date type and width selections defined for the ALSA snd_pcm_format_t
  /// type. This will be passed to snd_pcm_set_hw_params_format().
  ///
  /// \param access Selected from the set of values defined for
  /// snd_pcm_access_t. This designates if access to the device will be through
  /// mapped memory, or through read and write functions, and whether the
  /// arrangement of the data will be interleaved by channel, sequential arrays
  /// of values per channel (non-interleaved), or "complex". This will be
  /// passed to snd_pcm_set_hw_params_access().
  /// 
  /// \param channels The number of channels desired in the audio stream. Note
  /// that ALSA provides a channel-map API which can be used to determine the
  /// expected loudspeaker placement (relative to the listener) for each
  /// channel. This value is passed to snd_pcm_hw_params_set_channels().
  ///
  /// \param rate The number of audio frames per second. An ALSA audio frame
  /// consists of one sample for each channel that is in use. This value is
  /// passed to snd_pcm_hw_params_set_rate().
  ///
  /// \param period_size The number of ALSA frames to capture or play between
  /// each hardware interrupt. This facilitates the use of poll() for
  /// event-driven operation. This value will be passed to
  /// snd_pcm_hw_params_set_period_*near*(). We aren't guaranteed the exact
  /// value passed in, because the hardware only supports a specific set of
  /// period and buffer-size combinations.
  ///
  /// \param buffer_size The number of ALSA frames to buffer for playback
  /// before the program must write more. The effect of this value on capture
  /// is uncertain, the programmer's system seems to be able to buffer a
  /// long duration of ALSA frames regardless of this value.
  ///
  /// \return A sound_pcm_t pointer containing context for the selected audio
  /// device, as returned by snd_pcm_open(), or 0 in case the device name
  /// passed can't be found in the system or the device can not be opened due
  /// to privilege or contention.
  ///
  snd_pcm_t *
  ALSASetup(
   const char *		name,
   snd_pcm_stream_t	stream,
   int			mode,
   snd_pcm_format_t  	format,
   snd_pcm_access_t  	access,
   unsigned int  	channels,
   unsigned int  	rate,
   snd_pcm_uframes_t 	period_size,
   snd_pcm_uframes_t 	buffer_size)
  {
    int				error;
    snd_pcm_t *			handle = 0;
    snd_pcm_hw_params_t *	hw_params = 0;
    snd_pcm_sw_params_t *	sw_params = 0;
 
    error = open_by_longname(&handle, name, stream, mode);
    if ( error < 0 ) {
      error = snd_pcm_open(
       &handle,
       name,
       stream,
       mode);

       if ( error < 0 )
         return 0;
    }
 
    try {
      if ( (error = snd_pcm_hw_params_malloc(&hw_params)) < 0 )
        do_throw(error, name, stream, "ALSA hardare parameter allocation");
   
      if ( (error = snd_pcm_hw_params_any(handle, hw_params )) < 0 )
        do_throw(error, name, stream, "Get configuration space for device");
  
      if ( (error = snd_pcm_hw_params_set_format(handle, hw_params, format )) < 0 )
        do_throw(error, name, stream, "Set format");
  
      if ( (error = snd_pcm_hw_params_set_access(handle, hw_params, access )) < 0 )
        do_throw(error, name, stream, "Set access");
  
      if ( (error = snd_pcm_hw_params_set_channels(handle, hw_params, channels )) < 0 )
        do_throw(error, name, stream, "Set channels");
  
      if ( (error = snd_pcm_hw_params_set_rate(handle, hw_params, rate, 0 )) < 0 )
        do_throw(error, name, stream, "Set rate");
  
      if ( (error = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &period_size, 0)) < 0 )
        do_throw(error, name, stream, "Set I/O period size");
  
      if ( (error = snd_pcm_hw_params_set_buffer_size_near(handle, hw_params, &buffer_size)) < 0 )
        do_throw(error, name, stream, "Set I/O buffer size");
   
      if ( (error = snd_pcm_hw_params(handle, hw_params)) < 0 )
        do_throw(error, name, stream, "ALSA hardware parameter select");
   
      snd_pcm_hw_params_free(hw_params);
   
      if ((error = snd_pcm_sw_params_malloc(&sw_params)) < 0)
        do_throw(error, name, stream, "ALSA software parameter allocate");
  
      if ((error = snd_pcm_sw_params_current(handle, sw_params)) < 0)
        do_throw(error, name, stream, "ALSA get software parameters");
  
      if ((error = snd_pcm_sw_params_set_avail_min(handle, sw_params,
       period_size)) < 0)
        do_throw(error, name, stream, "ALSA software set minimum available.");
  
      if (( error = snd_pcm_sw_params_set_period_event(handle, sw_params, 1))
       < 0 )
        do_throw(error, name, stream, "ALSA software set period event.");

      if ((error = snd_pcm_sw_params_set_start_threshold(handle, sw_params, period_size))
       < 0)
        do_throw(error, name, stream,
         "ALSA set software start threshold");

      if (( error = snd_pcm_sw_params_set_stop_threshold(handle, sw_params,
       buffer_size + 1)) < 0 )
        do_throw(error, name, stream,
         "ALSA set software stop threshold");

      if ((error = snd_pcm_sw_params(handle, sw_params)) < 0)
        do_throw(error, name, stream, "ALSA set software parameters");
      
#if 0
      // Dump sound parameters, for debugging.
      snd_output_t * output = 0;
      snd_output_stdio_attach(&output, stderr, 0);
      fprintf(stderr, "%s\n", name);
      snd_pcm_dump_setup(handle, output);
#endif

      snd_pcm_sw_params_free(sw_params);

      if ((error = snd_pcm_prepare(handle)) < 0)
        do_throw(error, name, stream, "ALSA prepare audio interface for use");
    }
    catch (...)
    {
      snd_pcm_close(handle);
      throw; // Re-throws the current exception without modification.
    }
    return handle;
  }
}
