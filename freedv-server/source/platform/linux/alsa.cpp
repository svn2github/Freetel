#include "alsa.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>

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
          
        const int error = ctl_error ? ctl_error : pcm_error;
    
        i = strstr(longname, ", full speed");
        if ( i )
          *i = '\0';
       
        i = strstr(longname, " irq ");
        if ( i )
          *i = '\0';

        stream << "\"alsa:" << longname << '"';
        error_message(stream, error);
        stream << std::endl;
  
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

  static void
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
    int			error;
    snd_pcm_t *		handle = 0;
    snd_pcm_hw_params_t *	hw_params = 0;
 
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
 
    if ( (error = snd_pcm_hw_params_malloc(&hw_params)) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "ALSA hardare parameter allocation");
    }
 
    if ( (error = snd_pcm_hw_params_any(handle, hw_params )) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Get configuration space for device");
    }

    if ( (error = snd_pcm_hw_params_set_format(handle, hw_params, format )) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set format");
    }

    if ( (error = snd_pcm_hw_params_set_access(handle, hw_params, access )) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set access");
    }

    if ( (error = snd_pcm_hw_params_set_channels(handle, hw_params, channels )) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set channels");
    }

    if ( (error = snd_pcm_hw_params_set_rate(handle, hw_params, rate, 0 )) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set rate");
    }

    if ( (error = snd_pcm_hw_params_set_rate_resample(handle, hw_params, 0)) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Disable resampling");
    }

    if ( (error = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &period_size, 0)) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set I/O period size");
    }

    if ( (error = snd_pcm_hw_params_set_buffer_size_near(handle, hw_params, &buffer_size)) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "Set I/O buffer size");
    }
 
    if ( (error = snd_pcm_hw_params(handle, hw_params)) < 0 ) {
      snd_pcm_close(handle);
      do_throw(error, name, stream, "ALSA hardware parameter select");
    }
 
    snd_pcm_hw_params_free(hw_params);
 
    return handle;
  }
}
