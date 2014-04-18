/// \file platform/linux/alsa.h
/// Definitions for use by ALSA drivers.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include <alsa/asoundlib.h>

namespace FreeDV {
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
   snd_pcm_uframes_t 	buffer_size);
}
