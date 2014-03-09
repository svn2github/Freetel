#include <iostream>
#include <alsa/asoundlib.h>
#include <sstream>

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
    snd_ctl_card_info_t *	info = 0;
  
    const int error = snd_pcm_open(
     &pcm_handle,
     "default",
     SND_PCM_STREAM_PLAYBACK,
     0);
  
    stream << "\"alsa:default\"";
    error_message(stream, error);
    stream << std::endl;
  
    while ( snd_card_next(&card_index) == 0 && card_index >= 0 ) {
      char		device_name[20];
      snd_ctl_t *		ctl_handle = 0;
      int			pcm_error = 0;
      char *		longname = 0;
  
      if ( snd_card_get_longname(card_index, &longname) == 0 ) {
        sprintf(device_name, "hw:%d", card_index);
        
        const int ctl_error = snd_ctl_open(&ctl_handle, device_name, 0);
        if ( ctl_error == 0 ) {
          pcm_error = snd_pcm_open(&pcm_handle, device_name, mode, 0);
        }
          
        const int error = ctl_error ? ctl_error : pcm_error;
    
        stream << "\"alsa:" << longname << '"';
        error_message(stream, error);
        stream << std::endl;
  
        if ( ctl_error == 0 ) {
          snd_ctl_close(ctl_handle);
    	if ( pcm_error == 0 )
            snd_pcm_close(pcm_handle);
        }
      }
    }
  
    return stream;
  }
}
